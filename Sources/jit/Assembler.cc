#include <stdlib.h>
#include "Assembler.h"

namespace Avian{
    namespace Flock{
        inline uword AllocUnsafe(intptr_t size){
            return reinterpret_cast<uword>(malloc((size_t) size));
        }

        static const intptr_t kMinGap = 32;
        static const intptr_t kInitialBufferCapacity = 4 * (1024 * 1024);

        static uword ComputeLimit(uword data, intptr_t cap){
            return data + cap + kMinGap;
        }

        AssemblerBuffer::AssemblerBuffer() {
            this->contents_ = AllocUnsafe(kInitialBufferCapacity);
            this->cursor_ = this->contents_;
            this->limit_ = ComputeLimit(this->contents_, kInitialBufferCapacity);
        }
    }
}