#include <cassert>
#include <iostream>
#include <lmns/macros.hpp>
#include <type_traits>
#include <vector>

static_assert(sizeof(i8) == 1, "i8 must be exactly 1 byte");
static_assert(sizeof(u8) == 1, "u8 must be exactly 1 byte");
static_assert(sizeof(i16) == 2, "i16 must be exactly 2 bytes");
static_assert(sizeof(u16) == 2, "u16 must be exactly 2 bytes");
static_assert(sizeof(i32) == 4, "i32 must be exactly 4 bytes");
static_assert(sizeof(u32) == 4, "u32 must be exactly 4 bytes");
static_assert(sizeof(i64) == 8, "i64 must be exactly 8 bytes");
static_assert(sizeof(u64) == 8, "u64 must be exactly 8 bytes");

static_assert(std::is_signed_v<i8>, "i8 must be signed");
static_assert(std::is_unsigned_v<u8>, "u8 must be unsigned");
static_assert(std::is_signed_v<i16>, "i16 must be signed");
static_assert(std::is_unsigned_v<u16>, "u16 must be unsigned");
static_assert(std::is_signed_v<i32>, "i32 must be signed");
static_assert(std::is_unsigned_v<u32>, "u32 must be unsigned");
static_assert(std::is_signed_v<i64>, "i64 must be signed");
static_assert(std::is_unsigned_v<u64>, "u64 must be unsigned");

static_assert(std::is_same_v<i8, std::int8_t>, "i8 must be std::int8_t");
static_assert(std::is_same_v<u8, std::uint8_t>, "u8 must be std::uint8_t");
static_assert(std::is_same_v<i16, std::int16_t>, "i16 must be std::int16_t");
static_assert(std::is_same_v<u16, std::uint16_t>, "u16 must be std::uint16_t");
static_assert(std::is_same_v<i32, std::int32_t>, "i32 must be std::int32_t");
static_assert(std::is_same_v<u32, std::uint32_t>, "u32 must be std::uint32_t");
static_assert(std::is_same_v<i64, std::int64_t>, "i64 must be std::int64_t");
static_assert(std::is_same_v<u64, std::uint64_t>, "u64 must be std::uint64_t");

static_assert(std::is_same_v<index_t, i64>, "index_t must be i64");
static_assert(std::is_same_v<sindex_t, i32>, "sindex_t must be i32");
static_assert(std::is_same_v<uindex_t, u64>, "uindex_t must be u64");
static_assert(std::is_same_v<suindex_t, u32>, "suindex_t must be u32");

[[nodiscard]] int test_assert_retval() {
	ASSERT_RET_VAL(false, -1);
	return 0;
}

[[nodiscard]] int test_assert_retval2() {
	ASSERT_RET_VAL2(false, -2, "This should not trigger an assertion.");
	return 0;
}

int main() {
#ifdef LMNS_DEBUG
	std::cout << "This program is compiled in debug mode. Assert will terminate the program on failure." << std::endl;
#endif

#ifdef NDEBUG
	std::cout << "This program is compiled in release mode. Assert will do nothing on failure." << std::endl;
#endif

	// 以下在运行时检查，本质上就是一个if语句判断混合assert的包装
	ASSERT_NOEFFECT(sizeof(u8) == 1 && std::is_unsigned_v<u8>);
	ASSERT_NOEFFECT(sizeof(s8) == 1 && std::is_signed_v<s8>);
	ASSERT_NOEFFECT(sizeof(u16) == 2 && std::is_unsigned_v<u16>);
	ASSERT_NOEFFECT(sizeof(s16) == 2 && std::is_signed_v<s16>);
	ASSERT_NOEFFECT(sizeof(u32) == 4 && std::is_unsigned_v<u32>);
	ASSERT_NOEFFECT(sizeof(s32) == 4 && std::is_signed_v<s32>);
	ASSERT_NOEFFECT(sizeof(u64) == 8 && std::is_unsigned_v<u64>);
	ASSERT_NOEFFECT(sizeof(s64) == 8 && std::is_signed_v<s64>);
	ASSERT_NOEFFECT(sizeof(index_t) == 8 && std::is_signed_v<index_t>);
	ASSERT_NOEFFECT(sizeof(sindex_t) == 4 && std::is_signed_v<sindex_t>);
	ASSERT_NOEFFECT(sizeof(uindex_t) == 8 && std::is_unsigned_v<uindex_t>);
	ASSERT_NOEFFECT(sizeof(suindex_t) == 4 && std::is_unsigned_v<suindex_t>);

	ASSERT_NOEFFECT2(true, "This should not trigger an assertion.");
	int never_reached_condition = 5;
	int i = 0;
	for (i = 0; i < 10; ++i) {
		ASSERT_CONTINUE(i != never_reached_condition);
		std::cout << "AAA----- " << i << std::endl;
	}
	std::cout << "A------------------------------------------------------: " << i << std::endl;
	for (i = 0; i < 10; ++i)
		ASSERT_CONTINUE2(i != never_reached_condition, "loop index should not be " << i);
	std::cout << "B------------------------------------------------------: " << i << std::endl;
	for (i = 0; i < 10; ++i) {
		ASSERT_BREAK(i != never_reached_condition);
		std::cout << "BBB----- " << i << std::endl;
	}
	std::cout << "C------------------------------------------------------: " << i << std::endl;
	for (i = 0; i < 10; ++i)
		ASSERT_BREAK2(i != never_reached_condition, "loop index should not be " << i);
	std::cout << "D------------------------------------------------------: " << i << std::endl;
	for (i = 0; i < 10; ++i) {
		ASSERT_NOEFFECT(i != never_reached_condition);
		std::cout << "CCC----- " << i << std::endl;
	}
	std::cout << "E------------------------------------------------------: " << i << std::endl;
	for (i = 0; i < 10; ++i)
		ASSERT_NOEFFECT2(i != never_reached_condition, "loop index should not be " << i);
	std::cout << "F------------------------------------------------------: " << i << std::endl;

	auto retval1 = test_assert_retval();
	std::cout << "test_assert_retval() returned: " << retval1 << std::endl;
	auto retval2 = test_assert_retval2();
	std::cout << "test_assert_retval2() returned: " << retval2 << std::endl;

	return EXIT_SUCCESS;
}