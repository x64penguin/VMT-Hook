# VMT Hook
x86/x64 Virtual Method Table hooking library

## `VMTHook` class methods:
- `hook(int index, void* detour)` hooks function and returns original
- `unhook(int index)` unhooks single function
- `unhook_all()` unhooks all functions and swaps the virtual table back if it was swapped
- `get_original(int index)` returns the original function
- `get_vtable_size()` returns number of virtual functions
- `swap(void* swap_class)` swaps the entire virtual table
- `remove_swap()` swaps the virtual table back, if some functions were hooked before `swap()`, they still remain hooked

## Usage example:
```c++
VMTHook hook;

class CTargetClass {
public:
    virtual void A() {
        std::cout << "method A\n";
    }
};

void __fastcall hooked_A(CTargetClass* thisptr, void* edx \*other args*\) {
    static auto original = hook.get_original<void(__thiscall*)(CTargetClass*)>(0);

    std::cout << "hooked A\n";
    original(thisptr); // method A
}

int main() {
    CTargetClass* obj = new CTargetClass;
    hook.init(obj);
    hook.hook(0, hooked_A);
    obj->A(); 
    // will print "hooked A" and "method A" because original is called
    hook.unhook_all();
}
```