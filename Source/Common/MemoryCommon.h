#pragma once

#include <cstddef>
#include <string>

#include "CommonTypes.h"

namespace Common
{
extern u32 g_mem1_size_real;
extern u32 g_mem2_size_real;
extern u32 g_mem1_size;
extern u32 g_mem2_size;
extern u32 g_mem1_end;
extern u32 g_mem2_end;
extern const u32 g_mem1_start;
extern const u32 g_mem2_start;

void UpdateMemoryValues();

enum class MemType
{
  type_byte = 0,
  type_halfword,
  type_word,
  type_float,
  type_double,
  type_string,
  type_byteArray,
  type_num
};

enum class MemBase
{
  base_decimal = 0,
  base_hexadecimal,
  base_octal,
  base_binary,
  base_none // Placeholder when the base doesn't matter (ie. string)
};

enum class MemOperationReturnCode
{
  invalidInput,
  operationFailed,
  inputTooLong,
  invalidPointer,
  OK
};

size_t getSizeForType(const MemType type, const size_t length);
bool shouldBeBSwappedForType(const MemType type);
int getNbrBytesAlignementForType(const MemType type);
char* formatStringToMemory(MemOperationReturnCode& returnCode, size_t& actualLength,
                           const std::string inputString, const MemBase base, const MemType type,
                           const size_t length);
std::string formatMemoryToString(const char* memory, const MemType type, const size_t length,
                                 const MemBase base, const bool isUnsigned,
                                 const bool withBSwap = false);
} // namespace Common
