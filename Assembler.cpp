#include "pch.h"
#include "Assembler.h"

using namespace fmt;

Assembler::Assembler()
{
	if (ks_open(KS_ARCH_X86, KS_MODE_32, &m_ks) != KS_ERR_OK)
	{
		throw std::exception("Failed to initialize keystone engine");
	}
}

Assembler::~Assembler()
{
	ks_close(m_ks);
}

size_t Assembler::Write(
	std::string_view mnemonic,
	const void* address)
{
	size_t size;
	size_t count;
	unsigned char* encode;

	if (ks_asm(m_ks, mnemonic.data(), (uint64)address, &encode, &size, &count) != KS_ERR_OK)
	{
		LogAndThrow("Failed to assemble code: write {} to {} = {:x}", mnemonic, (uint64)address, (int)ks_errno(m_ks));
	}

	// Change memory protection to allow writing
	DWORD oldProtect;
	if (!VirtualProtect(const_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		LogAndThrow("Setting page executable failed at {}", (uint64)address);
	}

	// Write assembled code to the target memory address
	memcpy(const_cast<void*>(address), encode, size);

	// Restore original memory protection
	if (!VirtualProtect(const_cast<void*>(address), size, oldProtect, &oldProtect))
	{
		LogAndThrow("Restoring page protection failed at {}", (uint64)address);
	}

	return size;
}