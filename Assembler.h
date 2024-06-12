#pragma once

#include <keystone/keystone.h>

class Assembler
{
	public:
	Assembler();
	~Assembler();

	size_t Write(
		std::string_view string,
		const void* address);

private:
	ks_engine* m_ks = nullptr;
};
