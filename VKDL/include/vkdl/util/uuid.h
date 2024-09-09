#pragma once

#include <sstream>
#include <random>

class UUID
{
public:
	constexpr UUID() :
		hi(0), lo(0) 
	{}

	constexpr UUID(uint64_t hi, uint64_t lo) :
		hi(hi), lo(lo)
	{}

	constexpr UUID(const char* str) :
		hi(0), lo(0)
	{
		char buf[32] = {};

		for (int i = 0, j = 0; str[i]; ++i) {
			if (isdigit(str[i])) {
				buf[j++] = str[i] - '0';
			} else if (isalpha(str[i])) {
				auto alpha = toupper(str[i]);
				if ('F' < alpha || i != 0 && toupper(str[i - 1]) == 'I') continue;
				buf[j++] = alpha - 'A' + 10;
			}
		}

		for (int i = 0; i < 8; ++i) {
			hi |= (uint64_t)(buf[2 * i] << 4 | buf[2 * i + 1]) << (56 - 8 * i);
			lo |= (uint64_t)(buf[2 * i + 16] << 4 | buf[2 * i + 17]) << (56 - 8 * i);
		}
	}

	std::string to_string() const
	{
		std::stringstream ss;

		ss << std::hex << std::uppercase;
		ss << '{';
		ss << (hi >> 32) << '-';
		ss << (hi >> 16 & 0xffff) << '-';
		ss << (hi & 0xffff) << '-';
		ss << (lo >> 48) << '-';
		ss << (lo & 0xffffffffffff);
		ss << '}';

		return ss.str();
	}

	bool empty() const
	{
		return hi == 0 && lo == 0;
	}

	bool operator<(const UUID& rhs) const
	{
		if (hi == rhs.hi)
			return lo < rhs.lo;
		else
			return hi < rhs.hi;
	}

	bool operator>(const UUID& rhs) const
	{
		return rhs < *this;
	}

	bool operator<=(const UUID& rhs) const
	{
		return !(rhs < *this);
	}
	
	bool operator>=(const UUID& rhs) const
	{
		return !(*this < rhs);
	}

	bool operator==(const UUID& rhs) const
	{
		return hi == rhs.hi && lo == rhs.lo;
	}

	bool operator!=(const UUID& rhs) const
	{
		return !(*this == rhs);
	}

private:
	uint64_t hi;
	uint64_t lo;
};