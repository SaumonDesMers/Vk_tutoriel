#pragma once

namespace ft
{
	
	// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
	template<typename T, typename... Rest>
	void hashCombine(std::size_t &seed, const T &v, const Rest &... rest)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	}
	
}