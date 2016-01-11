#ifndef AVIAN3_ASSEMBLER_H
#define AVIAN3_ASSEMBLER_H

#include <stdint.h>

namespace Avian{
    namespace Flock{
        typedef uintptr_t uword;

        class Assembler;

        class AssemblerBuffer{
        private:
            uword contents_;
            uword cursor_;
            uword limit_;

            friend class Assembler;
        public:
            AssemblerBuffer();
            ~AssemblerBuffer(){}

            template<typename T> void Emit(T value) {
                *reinterpret_cast<T*>(this->cursor_) = value;
                this->cursor_ += sizeof(T);
            }

            uword Address(intptr_t pos){
                return this->contents_ + pos;
            }

            intptr_t Size() const{
                return this->cursor_ - this->contents_;
            }

            template<typename T> T Load(intptr_t pos){
                return *reinterpret_cast<T*>(this->contents_ + pos);
            }

            template<typename T> void Store(intptr_t pos, T val){
                *reinterpret_cast<T*>(this->contents_ + pos) = val;
            }
        };
    }
}

#include "Assembler_x64.h"

#endif