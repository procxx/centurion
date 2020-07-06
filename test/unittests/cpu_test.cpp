#include "cpu.hpp"

#include <catch.hpp>

using namespace centurion;

TEST_CASE("cpu::cache_line_size", "[cpu]")
{
  CHECK(cpu::cache_line_size() == SDL_GetCPUCacheLineSize());
}

TEST_CASE("cpu::cores", "[cpu]")
{
  CHECK(cpu::cores() == SDL_GetCPUCount());
}

TEST_CASE("cpu::is_little_endian", "[cpu]")
{
  CHECK(cpu::is_little_endian() == (SDL_BYTEORDER == SDL_LIL_ENDIAN));
}

TEST_CASE("cpu::is_big_endian", "[cpu]")
{
  CHECK(cpu::is_big_endian() == (SDL_BYTEORDER == SDL_BIG_ENDIAN));
}

TEST_CASE("cpu::has_rdtsc", "[cpu]")
{
  CHECK(cpu::has_rdtsc() == SDL_HasRDTSC());
}

TEST_CASE("cpu::has_altivec", "[cpu]")
{
  CHECK(cpu::has_altivec() == SDL_HasAltiVec());
}

TEST_CASE("cpu::has_mmx", "[cpu]")
{
  CHECK(cpu::has_mmx() == SDL_HasMMX());
}

TEST_CASE("cpu::has_3dnow", "[cpu]")
{
  CHECK(cpu::has_3dnow() == SDL_Has3DNow());
}

TEST_CASE("cpu::has_sse", "[cpu]")
{
  CHECK(cpu::has_sse() == SDL_HasSSE());
}

TEST_CASE("cpu::has_sse2", "[cpu]")
{
  CHECK(cpu::has_sse2() == SDL_HasSSE2());
}

TEST_CASE("cpu::has_sse3", "[cpu]")
{
  CHECK(cpu::has_sse3() == SDL_HasSSE3());
}

TEST_CASE("cpu::has_sse41", "[cpu]")
{
  CHECK(cpu::has_sse41() == SDL_HasSSE41());
}

TEST_CASE("cpu::has_sse42", "[cpu]")
{
  CHECK(cpu::has_sse42() == SDL_HasSSE42());
}

TEST_CASE("cpu::has_avx", "[cpu]")
{
  CHECK(cpu::has_avx() == SDL_HasAVX());
}

TEST_CASE("cpu::has_avx2", "[cpu]")
{
  CHECK(cpu::has_avx2() == SDL_HasAVX2());
}

TEST_CASE("cpu::has_avx512f", "[cpu]")
{
  CHECK(cpu::has_avx512f() == SDL_HasAVX512F());
}

TEST_CASE("cpu::has_neon", "[cpu]")
{
  CHECK(cpu::has_neon() == SDL_HasNEON());
}

TEST_CASE("cpu::has_arm_simd", "[cpu]")
{
  CHECK(cpu::has_arm_simd() == SDL_HasARMSIMD());
}

TEST_CASE("cpu::simd_alignment", "[cpu]")
{
  CHECK(cpu::simd_alignment() == SDL_SIMDGetAlignment());
}