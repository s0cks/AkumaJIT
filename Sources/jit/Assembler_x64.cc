#include "Assembler_x64.h"

namespace Avian{
    namespace Flock{
        void Assembler::EmitOperand(int rm, Operand& op) {
            const intptr_t length = op.length_;
            EmitUint8((uint8_t) (op.encoding_[0] + (rm << 3)));
            for(intptr_t i = 1; i < length; i++){
                EmitUint8(op.encoding_[i]);
            }
        }

        void Assembler::EmitLabel(Label* label, intptr_t instr_size) {
            if(label->IsBound()){
                intptr_t offset = label->Position() - this->buffer_.Size();
                EmitInt32(offset - instr_size);
            } else{
                EmitLabelLink(label);
            }
        }

        void Assembler::EmitLabelLink(Label* label){
            intptr_t pos = this->buffer_.Size();
            EmitInt32(label->pos_);
            label->LinkTo(pos);
        }

        void Assembler::EmitImmediate(Immediate& imm) {
            EmitInt64(imm.value());
        }

        void Assembler::addq(Register dst, Register src) {
            Operand op(src);
            EmitOperandREX(dst, op, REX_W);
            EmitUint8(0x03);
            EmitOperand(dst & 7, op);
        }

        static const int kLabelSize = 5;

        void Assembler::call(Label* label) {
            EmitUint8(0xE8);
            EmitLabel(label, kLabelSize);
        }

        void Assembler::bind(Label* label) {
            intptr_t bound = this->buffer_.Size();
            while(label->IsLinked()){
                intptr_t pos = label->LinkPosition();
                intptr_t next = this->buffer_.Load<int32_t>(pos);
                this->buffer_.Store<int32_t>(pos, bound - (pos + 4));
                label->pos_ = next;
            }
            label->BindTo(bound);
        }

        void Assembler::addq(Register dst, Address& addr) {
            EmitOperandREX(dst, addr, REX_W);
            EmitUint8(0x03);
            EmitOperand(dst & 7, addr);
        }

        void Assembler::addq(Register dst, Immediate& imm){
            movq(TMP, imm);
            addq(dst, TMP);
        }

        void Assembler::movq(Register dst, Immediate& imm) {
            EmitRegisterREX(dst, REX_W);
            EmitUint8((uint8_t) (0xB8 | (dst & 7)));
            EmitImmediate(imm);
        }

        void Assembler::addq(Address& addr, Register src){
            EmitOperandREX(src, addr, REX_W);
            EmitUint8(0x01);
            EmitOperand(src & 7, addr);
        }

        void Assembler::addq(Address& addr, Immediate& imm) {
            movq(TMP, imm);
            addq(addr, TMP);
        }

        void Assembler::jmp(Label* label, bool near) {
            if(label->IsBound()){
                intptr_t offset = label->Position() - this->buffer_.Size();
                EmitUint8(0xE9);
                EmitInt32(offset - 5);
            } else if(near){
                EmitUint8(0xEB);
            } else{
                EmitUint8(0xE9);
                EmitLabelLink(label);
            }
        }

        void Assembler::ret() {
            EmitUint8(0xC3);
        }
    }
}