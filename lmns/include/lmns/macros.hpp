#ifndef LMNS_INCLUDE_LMNS_MACROS_HPP
#define LMNS_INCLUDE_LMNS_MACROS_HPP

#include <cassert>
#include <cstdint>
#include <iostream>

#define LMNS_ASSERT(X)                                                                                            \
	if (!(X)) [[unlikely]] {                                                                                  \
		std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT FAILED: " << #X << "\nSTACKTRACE: TO DO \n"; \
		assert(X);                                                                                        \
	}
#define LMNS_ASSERT2(X, INFO)                                                                                                                 \
	if (!(X)) [[unlikely]] {                                                                                                              \
		std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT FAILED: " << #X << "\nERROR INFO: " << INFO << "\nSTACKTRACE: TO DO \n"; \
		assert(X);                                                                                                                    \
	}
#define ASSERT_RET_VAL(X, VAL)    \
	do {                      \
		if (X) [[likely]] \
			break;    \
		LMNS_ASSERT(X);   \
		return VAL;       \
	} while (false)
#define ASSERT_RET_VAL2(X, VAL, INFO)  \
	do {                           \
		if (X) [[likely]]      \
			break;         \
		LMNS_ASSERT2(X, INFO); \
		return VAL;            \
	} while (false)
#define ASSERT_RET_VOID(X)        \
	do {                      \
		if (X) [[likely]] \
			break;    \
		LMNS_ASSERT(X);   \
		return;           \
	} while (false)
#define ASSERT_RET_VOID2(X, INFO)      \
	do {                           \
		if (X) [[likely]]      \
			break;         \
		LMNS_ASSERT2(X, INFO); \
		return;                \
	} while (false)
#define ASSERT_NOEFFECT(X)        \
	do {                      \
		if (X) [[likely]] \
			break;    \
		LMNS_ASSERT(X);   \
	} while (false)
#define ASSERT_NOEFFECT2(X, INFO)      \
	do {                           \
		if (X) [[likely]]      \
			break;         \
		LMNS_ASSERT2(X, INFO); \
	} while (false)
#define ASSERT_CONTINUE(X)       \
	if (!(X)) [[unlikely]] { \
		LMNS_ASSERT(X);  \
		continue;        \
	} else {                 \
	}
#define ASSERT_CONTINUE2(X, INFO)      \
	if (!(X)) [[unlikely]] {       \
		LMNS_ASSERT2(X, INFO); \
		continue;              \
	} else {                       \
	}
#define ASSERT_BREAK(X)          \
	if (!(X)) [[unlikely]] { \
		LMNS_ASSERT(X);  \
		break;           \
	} else {                 \
	}
#define ASSERT_BREAK2(X, INFO)         \
	if (!(X)) [[unlikely]] {       \
		LMNS_ASSERT2(X, INFO); \
		break;                 \
	} else {                       \
	}

using i8 = std::int8_t;
using s8 = i8;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using s16 = i16;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using s32 = i32;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using s64 = i64;
using u64 = std::uint64_t;
using index_t = i64;
using sindex_t = i32;
using uindex_t = u64;
using suindex_t = u32;

#endif
