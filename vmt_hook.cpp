#include "vmt_hook.h"

int VMTHook::calculate_length(void** vfptr) {
	int result = 0;
	MEMORY_BASIC_INFORMATION mem_info;

	while (VirtualQuery(vfptr[result], &mem_info, sizeof(mem_info))) {
		if (mem_info.State != MEM_COMMIT || !(mem_info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
			break;

		result++;
	}

	return result;
}

VMTHook::VMTHook() {
	m_base = nullptr;
	m_vtable = nullptr;
	m_original_vtable = nullptr;
	m_vtable_size = 0;
	m_original_protect = 0;
}

VMTHook::VMTHook(void* base) {
	init(base);
}

VMTHook::~VMTHook() {
	unhook_all();
	delete[] m_original_vtable;
}

void VMTHook::init(void* base) {
	m_base = base;
	m_vtable = *reinterpret_cast<void***>(m_base);
	assert(m_vtable);
	m_vtable_size = calculate_length(m_vtable);
	m_original_vtable = new void* [m_vtable_size];
	memcpy(m_original_vtable, m_vtable, PTRSIZE * m_vtable_size);
}

int VMTHook::get_vtable_size() {
	return m_vtable_size;
}

void* VMTHook::hook(int index, void* detour) {
	assert(index < m_vtable_size);

	remove_protect();
	m_vtable[index] = detour;
	restore_protect();

	return m_original_vtable[index];
}

void VMTHook::unhook(int index) {
	assert(index < m_vtable_size);
	m_vtable[index] = m_original_vtable[index];
}

void VMTHook::unhook_all() {
	assert(m_vtable);
	assert(m_original_vtable);

	remove_protect();

	remove_swap();
	memcpy(m_vtable, m_original_vtable, sizeof(void*) * m_vtable_size);

	restore_protect();
}

void VMTHook::swap(void* base) {
	void** new_vtable = *reinterpret_cast<void***>(base);
	assert(new_vtable);

	int new_vtable_size = calculate_length(new_vtable);
	assert(new_vtable_size == m_vtable_size);
	*reinterpret_cast<void***>(m_base) = new_vtable;
}

void VMTHook::remove_swap() {
	*reinterpret_cast<void***>(m_base) = m_vtable;
}