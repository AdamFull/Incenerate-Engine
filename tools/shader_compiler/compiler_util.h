#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <udetail.hpp>

constexpr const uint64_t MOD = static_cast<uint64_t>(-1);

inline std::string make_shader_hashname(const std::vector<std::string>& tokens)
{
	uint64_t result_hash{ 0ull };
	for (auto& token : tokens)
		result_hash = (result_hash + utl::fnv1a_64_hash(token.c_str())) % MOD;

	std::stringstream ss;
	ss << std::hex << result_hash;
	return ss.str();
}