#include <jit/Assembler.h>
#include <stddef.h>

using namespace Avian::Flock;

typedef void (*func)();

typedef struct{
    int x;
    int y;
} point;

int main(int argc, char** argv){
    Assembler assm;
    point* p = new point();
    p->x = 10;
    p->y = 100;

    FieldAddress yAddr(RAX, offsetof(point, y));
    Immediate ten(10);
    Immediate pointImm(reinterpret_cast<int64_t>(p));
    assm.movq(RAX, pointImm);
    assm.addq(yAddr, ten);
    FieldAddress xAddr(RAX, offsetof(point, x));
    assm.addq(xAddr, ten);
    assm.ret();
    assm.Finish<func>()();

    printf("%d\n", p->x);
    printf("%d\n", p->y);
    return 0;
}