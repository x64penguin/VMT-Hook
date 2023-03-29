#include <iostream>

#include "vmt_hook.h"

VMTHook hook_ex;

class CTargetClass {
public:
    virtual void A(int a) {
        std::cout << "method A" << a << "\n" ;
    }

    virtual void B() {
        std::cout << "method B\n";
    }
};

class CSwapClass {
    virtual void A() {
        std::cout << "swapped A\n";
    }

    virtual void B() {
        std::cout << "swapped B\n";
    }
};

// First argument is "this" pointer
// Second argument "void* edx" must always be as second argument due to __fastcall
void hooked_A(CTargetClass* thisptr, int a /* other args */) {
    // original function can also be __fastcall but you will need to pass edx as second argument
    static auto original = hook_ex.get_original<void(*)(CTargetClass*, int)>(0);

    std::cout << "hooked A\n";

    original(thisptr, a); // method A
}

int main()
{
    // Usage example
    CTargetClass* target = new CTargetClass;

    hook_ex.init(target);

    target->A(2); // prints "method A"
    target->B(); // prints "method B"

    hook_ex.hook(0, hooked_A);

    target->A(2); // prints "hooked A method A" because original function called at the end of hook
    target->B(); // prints "method B"

    CSwapClass* swap = new CSwapClass;

    hook_ex.swap(swap);

    target->A(2); // prints "swapped A"
    target->B(); // prints "swapped B"

    hook_ex.unhook_all();

    target->A(2); // prints "method A"
    target->B(); // prints "method B"
}