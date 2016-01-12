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

    // Create the address of the y field
    FieldAddress yAddr(RAX, offsetof(point, y));
    // Create immediate value of 10
    Immediate ten(10);
    // Create immediate value of p
    Immediate pointImm(reinterpret_cast<int64_t>(p));
    // Move p's immediate value into RAX
    assm.movq(RAX, pointImm);
    // Add 10 to the value at y's address
    assm.addq(yAddr, ten);
    // Create the address of the x field
    FieldAddress xAddr(RAX, offsetof(point, x));
    // Add 10 to the value at x's address
    assm.addq(xAddr, ten);
    // Return
    assm.ret();
    // malloc & map the memory then cast it to a function pointer and call it
    assm.Finish<func>()();

    printf("%d\n", p->x);
    printf("%d\n", p->y);
    return 0;
}
