#ifndef AVIAN3_ASSEMBLER_X64_H
#define AVIAN3_ASSEMBLER_X64_H

#include <tiff.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <iostream>
#include "Constants_x64.h"
#include "Assembler.h"

namespace Avian{
    namespace Flock{
        static const int kWordSize = sizeof(intptr_t);

        class Label{
        private:
            intptr_t pos_;
            intptr_t unresolved_;

            void BindTo(intptr_t pos){
                this->pos_ = -pos - kWordSize;
            }

            void LinkTo(intptr_t pos){
                this->pos_ = pos + kWordSize;
            }

            friend class Assembler;
        public:
            Label(): pos_(0), unresolved_(0){}
            ~Label(){}

            intptr_t Position() const{
                return -this->pos_ - kWordSize;
            }

            intptr_t LinkPosition() const{
                return this->pos_ - kWordSize;
            }

            bool IsBound() const{
                return this->pos_ < 0;
            }

            bool IsLinked() const{
                return this->pos_ > 0;
            }
        };

        class Immediate{
        private:
            const int64_t value_;
        public:
            explicit Immediate(int64_t val): value_(val){}
            Immediate(const Immediate& other): value_(other.value_){}

            int64_t value() const{
                return this->value_;
            }
        };

        class Operand{
        private:
            uint8_t length_;
            uint8_t rex_;
            uint8_t encoding_[6];

            explicit Operand(Register reg): rex_(REX_NONE){
                SetModRM(3, reg);
            }

            uint8_t EncodingAt(intptr_t index) const{
                return this->encoding_[index];
            }

            bool IsRegister(Register reg) const{
                return ((reg > 7 ? 1 : 0) == (this->rex_ & REX_B))
                    && ((EncodingAt(0) & 0xF8) == 0xC0) &&
                        ((EncodingAt(0) & 0x07) == reg);
            }

            friend class Assembler;
        protected:
            Operand(): length_(0), rex_(REX_NONE){}

            void SetModRM(int mod, Register rm){
                if((rm > 7) && !((rm == R12) && (mod != 3))){
                    this->rex_ |= REX_B;
                }
                this->encoding_[0] = (uint8_t) ((mod << 6) | (rm & 7));
                this->length_ = 1;
            }

            void SetSIB(ScaleFactor scale, Register index, Register base){
                if(base > 7){
                    this->rex_ |= REX_B;
                }
                if(index > 7){
                    this->rex_ |= REX_X;
                }
                this->encoding_[1] = (scale << 6) | ((index & 7) << 3) | (base & 7);
                this->length_ = 2;
            }

            void SetDisp8(int8_t disp){
                this->encoding_[this->length_++] = static_cast<uint8_t>(disp);
            }

            void SetDisp32(int32_t disp){
                memmove(&this->encoding_[this->length_], &disp, sizeof(disp));
                this->length_ += sizeof(disp);
            }
        public:
            uint8_t rex() const{
                return this->rex_;
            }

            uint8_t mod() const{
                return (uint8_t) ((EncodingAt(0) >> 6) & 3);
            }

            Register rm() const{
                int rm_rex = (this->rex_ & REX_B) << 3;
                return static_cast<Register>(rm_rex + (EncodingAt(0) & 7));
            }

            Register index() const{
                int index_rex = (this->rex_ & REX_X) << 2;
                return static_cast<Register>(index_rex + ((EncodingAt(1) >> 3) & 7));
            }

            Register base() const{
                int base_rex = (this->rex_ & REX_B) << 3;
                return static_cast<Register>(base_rex + (EncodingAt(1) & 7));
            }

            Operand(const Operand& other):
                    length_(other.length_),
                    rex_(other.rex_){
                memmove(&encoding_[0], &other.encoding_[0], other.length_);
            }

            Operand& operator =(const Operand& other){
                this->length_ = other.length_;
                this->rex_ = other.rex_;
                memmove(&encoding_[0], &other.encoding_[0], other.length_);
                return *this;
            }
        };

        class Address: public Operand{
        private:
            Address(Register base, int32_t disp, bool fixed){
                SetModRM(2, base);
                if((base & 7) == RSP){
                    SetSIB(TIMES_1, RSP, base);
                }
                SetDisp32(disp);
            }
        public:
            Address(Register base, int32_t disp){
                if((disp == 0) && (base & 7) != RBP){
                    SetModRM(0, base);
                    if((base & 7) == RSP){
                        SetSIB(TIMES_1, RSP, base);
                    }
                } else{
                    SetModRM(2, base);
                    if((base & 7) == RSP){
                        SetSIB(TIMES_1, RSP, base);
                    }
                    SetDisp32(disp);
                }
            }

            Address(Register index, ScaleFactor scale, int32_t disp){
                SetModRM(0, RSP);
                SetSIB(scale, index, RBP);
                SetDisp32(disp);
            }

            Address(Register base, Register index, ScaleFactor scale, int32_t disp){
                if((disp == 0) && ((base & 7) != RBP)){
                    SetModRM(0, RSP);
                    SetSIB(scale, index, base);
                } else{
                    SetModRM(2, RSP);
                    SetSIB(scale, index, base);
                    SetDisp32(disp);
                }
            }

            Address(const Address& other): Operand(other){}

            Address& operator =(const Address& other){
                Operand::operator=(other);
                return *this;
            }
        };

        class FieldAddress: public Address{
        public:
            FieldAddress(Register base, int32_t disp):
                    Address(base, disp){}

            FieldAddress(Register base, Register index, ScaleFactor scale, int32_t disp):
                    Address(base, index, scale, disp){}

            FieldAddress(const FieldAddress& other): Address(other){}

            FieldAddress& operator =(const FieldAddress& other){
                Address::operator=(other);
                return *this;
            }
        };

        class Assembler{
        private:
            AssemblerBuffer buffer_;
        public:
            explicit Assembler(): buffer_(){}
            ~Assembler(){}

            void call(Label* label);
            void bind(Label* label);
            void addq(Register dst, Register src);
            void addq(Register dst, Immediate& imm);
            void addq(Register dst, Address& addr);
            void addq(Address& addr, Register src);
            void addq(Address& addr, Immediate& imm);
            void movq(Register dst, Immediate& imm);
            void jmp(Label* label, bool near);
            void ret();

            void EmitImmediate(Immediate& imm);
            void EmitOperand(int rm, Operand& op);
            void EmitLabel(Label* label, intptr_t instr_size);
            void EmitLabelLink(Label* label);

            inline void EmitUint8(uint8_t val){
                this->buffer_.Emit<uint8_t>(val);
            }

            inline void EmitOperandREX(int rm, Operand& op, uint8_t rex){
                rex |= (rm > 7 ? REX_R : REX_NONE) | op.rex();
                if(rex != REX_NONE){
                    EmitUint8(REX_PREFIX|rex);
                }
            }

            inline void EmitRegisterREX(Register reg, uint8_t rex){
                rex |= (reg > 7 ? REX_B : REX_NONE);
                if(rex != REX_NONE){
                    EmitUint8(REX_PREFIX | rex);
                }
            }

            inline void EmitInt32(int32_t val){
                this->buffer_.Emit<int32_t>(val);
            }

            inline void EmitInt64(int64_t val){
                this->buffer_.Emit<int64_t>(val);
            }

            void* Code() const{
                return reinterpret_cast<void*>(this->buffer_.contents_);
            }

            uword Size() const{
                return this->buffer_.cursor_ - this->buffer_.contents_;
            }

            template<typename T> T Finish() const{
                void* ptr = mmap(0, Size(), PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANON, -1, 0);
                if(ptr == NULL){
                    std::cerr << "Memory == NULL" << std::endl;
                    abort();
                }
                mempcpy(ptr, Code(), Size());
                return reinterpret_cast<T>(ptr);
            }
        };
    }
}

#endif