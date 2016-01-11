#ifndef AVIAN3_CONSTANTS_X64_H
#define AVIAN3_CONSTANTS_X64_H

namespace Avian{
    namespace Flock{
        enum Register{
            RAX = 0,
            RCX = 1,
            RDX = 2,
            RBX = 3,
            RSP = 4,
            RBP = 5,
            RSI = 6,
            RDI = 7,
            R8  = 8,
            R9  = 9,
            R10 = 10,
            R11 = 11,
            R12 = 12,
            R13 = 13,
            R14 = 14,
            R15 = 15
        };

        enum ByteRegister{
            AL = 0,
            CL = 1,
            DL = 2,
            BL = 3,
            AH = 4,
            CH = 5,
            DH = 6,
            BH = 7,
        };

        enum XMMRegister{
            XMM0 = 0,
            XMM1 = 1,
            XMM2 = 2,
            XMM3 = 3,
            XMM4 = 4,
            XMM5 = 5,
            XMM6 = 6,
            XMM7 = 7,
            XMM8 = 8,
            XMM9 = 9,
            XMM10 = 10,
            XMM11 = 11,
            XMM12 = 12,
            XMM13 = 13,
            XMM14 = 14,
            XMM15 = 15,
        };

        typedef XMMRegister FPURegister;
        const FPURegister FPUTMP = XMM0;

        enum RexBits{
            REX_NONE = 0,
            REX_B = 1 << 0,
            REX_X = 1 << 1,
            REX_R = 1 << 2,
            REX_W = 1 << 3,
            REX_PREFIX = 1 << 6
        };

        const Register TMP = R11;

        enum ScaleFactor{
            TIMES_1 = 0,
            TIMES_2 = 1,
            TIMES_4 = 2,
            TIMES_8 = 3,
            TIMES_16 = 4
        };
    }
}

#endif