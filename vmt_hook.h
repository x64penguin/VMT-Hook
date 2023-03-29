#pragma once
#include <Windows.h>
#include <assert.h>

#define PTRSIZE sizeof(void*)

class VMTHook {
	void*	m_base; // pointer to class instance
	void**	m_vtable; // pointer to virtual table
	void**	m_original_vtable; // pointer to original virtual table
	int		m_vtable_size; // number of virtual functions
	DWORD	m_original_protect; // original virtual table protect

	// Calculates VMT length based on memory protection check
	int calculate_length(void** vfptr);

	// Removes protect from virtual table to make it writable
	void remove_protect() {
		VirtualProtect(m_vtable, PTRSIZE * m_vtable_size, PAGE_EXECUTE_READWRITE, &m_original_protect);
	}

	// Restores protect back
	void restore_protect() {
		VirtualProtect(m_vtable, PTRSIZE * m_vtable_size, m_original_protect, &m_original_protect);
	}

public:
	VMTHook();
	VMTHook(void* class_instance);
	~VMTHook();

	// Copies original virtual table, calculates vtable size
	void init(void* class_instance);

	// Returns number of virtual functions
	int get_vtable_size();

	// Hooks function by index, returns original
	// original function can also be received using get_original method
	void* hook(int index, void* detour);

	// Returns original method by index
	// can be called even if function not hooked
	// works for swapped VMT too
	template <typename T = void*>
	T get_original(int index);

	// Unhooks function by index
	void unhook(int index);

	// Unhooks all functions
	void unhook_all();

	// Swaps entire virtual method table for this instance only
	// accepts class instance pointer, not vtable
	void swap(void* swap_class);

	// Swaps back virtual method table
	void remove_swap();
};

template <typename T>
T VMTHook::get_original(int index) {
	assert(index < m_vtable_size);
	return reinterpret_cast<T>(m_original_vtable[index]);
}