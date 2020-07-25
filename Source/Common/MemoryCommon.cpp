#include "MemoryCommon.h"

#include <bitset>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>
#include <locale>

#include "../Common/CommonTypes.h"
#include "../Common/CommonUtils.h"

namespace Common
{
size_t getSizeForType(const MemType type, const size_t length)
{
  switch (type)
  {
  case MemType::type_byte:
    return sizeof(u8);
  case MemType::type_halfword:
    return sizeof(u16);
  case MemType::type_word:
    return sizeof(u32);
  case MemType::type_float:
    return sizeof(float);
  case MemType::type_double:
    return sizeof(double);
  case MemType::type_string:
    return length;
  case MemType::type_byteArray:
    return length;
  default:
    return 0;
  }
}

bool shouldBeBSwappedForType(const MemType type)
{
  switch (type)
  {
  case MemType::type_byte:
    return false;
  case MemType::type_halfword:
    return true;
  case MemType::type_word:
    return true;
  case MemType::type_float:
    return true;
  case MemType::type_double:
    return true;
  case MemType::type_string:
    return false;
  case MemType::type_byteArray:
    return false;
  default:
    return false;
  }
}

int getNbrBytesAlignementForType(const MemType type)
{
  switch (type)
  {
  case MemType::type_byte:
    return 1;
  case MemType::type_halfword:
    return 2;
  case MemType::type_word:
    return 4;
  case MemType::type_float:
    return 4;
  case MemType::type_double:
    return 4;
  case MemType::type_string:
    return 1;
  case MemType::type_byteArray:
    return 1;
  default:
    return 1;
  }
}

char* formatStringToMemory(MemOperationReturnCode& returnCode, size_t& actualLength,
                           const std::string inputString, const MemBase base, const MemType type,
                           const size_t length, StrWidth stringWidth)
{
  if (inputString.length() == 0)
  {
    returnCode = MemOperationReturnCode::invalidInput;
    return nullptr;
  }

  std::stringstream ss(inputString);
  switch (base)
  {
  case MemBase::base_octal:
    ss >> std::oct;
    break;
  case MemBase::base_decimal:
    ss >> std::dec;
    break;
  case MemBase::base_hexadecimal:
    ss >> std::hex;
    break;
  }

  size_t size = getSizeForType(type, length);
  char* buffer = nullptr;
  if(type != Common::MemType::type_string)
    buffer = new char[size];

  switch (type)
  {
  case MemType::type_byte:
  {
    u8 theByte = 0;
    if (base == MemBase::base_binary)
    {
      unsigned long long input = 0;
      try
      {
        input = std::bitset<sizeof(u8) * 8>(inputString).to_ullong();
      }
      catch (std::invalid_argument)
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
      theByte = static_cast<u8>(input);
    }
    else
    {
      int theByteInt = 0;
      ss >> theByteInt;
      if (ss.fail())
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
      theByte = static_cast<u8>(theByteInt);
    }

    std::memcpy(buffer, &theByte, size);
    actualLength = sizeof(u8);
    break;
  }

  case MemType::type_halfword:
  {
    u16 theHalfword = 0;
    if (base == MemBase::base_binary)
    {
      unsigned long long input = 0;
      try
      {
        input = std::bitset<sizeof(u16) * 8>(inputString).to_ullong();
      }
      catch (std::invalid_argument)
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
      theHalfword = static_cast<u16>(input);
    }
    else
    {
      ss >> theHalfword;
      if (ss.fail())
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
    }

    std::memcpy(buffer, &theHalfword, size);
    actualLength = sizeof(u16);
    break;
  }

  case MemType::type_word:
  {
    u32 theWord = 0;
    if (base == MemBase::base_binary)
    {
      unsigned long long input = 0;
      try
      {
        input = std::bitset<sizeof(u32) * 8>(inputString).to_ullong();
      }
      catch (std::invalid_argument)
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
      theWord = static_cast<u32>(input);
    }
    else
    {
      ss >> theWord;
      if (ss.fail())
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
    }

    std::memcpy(buffer, &theWord, size);
    actualLength = sizeof(u32);
    break;
  }

  case MemType::type_float:
  {
    float theFloat = 0.0f;
    // 9 digits is the max number of digits in a float that can recover any binary format
    ss >> std::setprecision(9) >> theFloat;
    if (ss.fail())
    {
      delete[] buffer;
      buffer = nullptr;
      returnCode = MemOperationReturnCode::invalidInput;
      return buffer;
    }
    std::memcpy(buffer, &theFloat, size);
    actualLength = sizeof(float);
    break;
  }

  case MemType::type_double:
  {
    double theDouble = 0.0;
    // 17 digits is the max number of digits in a double that can recover any binary format
    ss >> std::setprecision(17) >> theDouble;
    if (ss.fail())
    {
      delete[] buffer;
      buffer = nullptr;
      returnCode = MemOperationReturnCode::invalidInput;
      return buffer;
    }
    std::memcpy(buffer, &theDouble, size);
    actualLength = sizeof(double);
    break;
  }

  case MemType::type_string:
  {
    std::string newTmpString = convertFromUTF8(inputString.c_str(), inputString.size(), stringWidth);
    buffer = new char[newTmpString.size()];
    std::memcpy(buffer, newTmpString.c_str(), newTmpString.size());
    actualLength = newTmpString.size();
    break;
  }

  case MemType::type_byteArray:
  {
    std::vector<std::string> bytes;
    std::string next;
    for (auto i : inputString)
    {
      if (i == ' ')
      {
        if (!next.empty())
        {
          bytes.push_back(next);
          next.clear();
        }
      }
      else
      {
        next += i;
      }
    }
    if (!next.empty())
    {
      bytes.push_back(next);
      next.clear();
    }

    if (bytes.size() > length)
    {
      delete[] buffer;
      buffer = nullptr;
      returnCode = MemOperationReturnCode::inputTooLong;
      return buffer;
    }

    int index = 0;
    for (const auto& i : bytes)
    {
      std::stringstream byteStream(i);
      ss >> std::hex;
      u8 theByte = 0;
      int theByteInt = 0;
      ss >> theByteInt;
      if (ss.fail())
      {
        delete[] buffer;
        buffer = nullptr;
        returnCode = MemOperationReturnCode::invalidInput;
        return buffer;
      }
      theByte = static_cast<u8>(theByteInt);
      std::memcpy(&(buffer[index]), &theByte, sizeof(u8));
      index++;
    }
    actualLength = bytes.size();
  }
  }
  return buffer;
}

std::string formatMemoryToString(const char* memory, const MemType type, const size_t length,
                                 const MemBase base, const bool isUnsigned, const bool withBSwap,
                                 const StrWidth stringWidth)
{
  std::stringstream ss;
  switch (base)
  {
  case Common::MemBase::base_octal:
    ss << std::oct;
    break;
  case Common::MemBase::base_decimal:
    ss << std::dec;
    break;
  case Common::MemBase::base_hexadecimal:
    ss << std::hex << std::uppercase;
    break;
  }

  switch (type)
  {
  case Common::MemType::type_byte:
  {
    if (isUnsigned || base == Common::MemBase::base_binary)
    {
      u8 unsignedByte = 0;
      std::memcpy(&unsignedByte, memory, sizeof(u8));
      if (base == Common::MemBase::base_binary)
        return std::bitset<sizeof(u8) * 8>(unsignedByte).to_string();
      // This has to be converted to an integer type because printing a uint8_t would resolve to a
      // char and print a single character.
      ss << static_cast<unsigned int>(unsignedByte);
      return ss.str();
    }
    else
    {
      s8 aByte = 0;
      std::memcpy(&aByte, memory, sizeof(s8));
      // This has to be converted to an integer type because printing a uint8_t would resolve to a
      // char and print a single character.  Additionaly, casting a signed type to a larger signed
      // type will extend the sign to match the size of the destination type, this is required for
      // signed values in decimal, but must be bypassed for other bases, this is solved by first
      // casting to u8 then to signed int.
      if (base == Common::MemBase::base_decimal)
        ss << static_cast<int>(aByte);
      else
        ss << static_cast<int>(static_cast<u8>(aByte));
      return ss.str();
    }
  }
  case Common::MemType::type_halfword:
  {
    char* memoryCopy = new char[sizeof(u16)];
    std::memcpy(memoryCopy, memory, sizeof(u16));
    if (withBSwap)
    {
      u16 halfword = 0;
      std::memcpy(&halfword, memoryCopy, sizeof(u16));
      halfword = Common::bSwap16(halfword);
      std::memcpy(memoryCopy, &halfword, sizeof(u16));
    }

    if (isUnsigned || base == Common::MemBase::base_binary)
    {
      u16 unsignedHalfword = 0;
      std::memcpy(&unsignedHalfword, memoryCopy, sizeof(u16));
      if (base == Common::MemBase::base_binary)
      {
        delete[] memoryCopy;
        return std::bitset<sizeof(u16) * 8>(unsignedHalfword).to_string();
      }
      ss << unsignedHalfword;
      delete[] memoryCopy;
      return ss.str();
    }
    s16 aHalfword = 0;
    std::memcpy(&aHalfword, memoryCopy, sizeof(s16));
    ss << aHalfword;
    delete[] memoryCopy;
    return ss.str();
  }
  case Common::MemType::type_word:
  {
    char* memoryCopy = new char[sizeof(u32)];
    std::memcpy(memoryCopy, memory, sizeof(u32));
    if (withBSwap)
    {
      u32 word = 0;
      std::memcpy(&word, memoryCopy, sizeof(u32));
      word = Common::bSwap32(word);
      std::memcpy(memoryCopy, &word, sizeof(u32));
    }

    if (isUnsigned || base == Common::MemBase::base_binary)
    {
      u32 unsignedWord = 0;
      std::memcpy(&unsignedWord, memoryCopy, sizeof(u32));
      if (base == Common::MemBase::base_binary)
      {
        delete[] memoryCopy;
        return std::bitset<sizeof(u32) * 8>(unsignedWord).to_string();
      }
      ss << unsignedWord;
      delete[] memoryCopy;
      return ss.str();
    }
    s32 aWord = 0;
    std::memcpy(&aWord, memoryCopy, sizeof(s32));
    ss << aWord;
    delete[] memoryCopy;
    return ss.str();
  }
  case Common::MemType::type_float:
  {
    char* memoryCopy = new char[sizeof(u32)];
    std::memcpy(memoryCopy, memory, sizeof(u32));
    if (withBSwap)
    {
      u32 word = 0;
      std::memcpy(&word, memoryCopy, sizeof(u32));
      word = Common::bSwap32(word);
      std::memcpy(memoryCopy, &word, sizeof(u32));
    }

    float aFloat = 0.0f;
    std::memcpy(&aFloat, memoryCopy, sizeof(float));
    // With 9 digits of precision, it is possible to convert a float back and forth to its binary
    // representation without any loss
    ss << std::setprecision(9) << aFloat;
    delete[] memoryCopy;
    return ss.str();
  }
  case Common::MemType::type_double:
  {
    char* memoryCopy = new char[sizeof(u64)];
    std::memcpy(memoryCopy, memory, sizeof(u64));
    if (withBSwap)
    {
      u64 doubleword = 0;
      std::memcpy(&doubleword, memoryCopy, sizeof(u64));
      doubleword = Common::bSwap64(doubleword);
      std::memcpy(memoryCopy, &doubleword, sizeof(u64));
    }

    double aDouble = 0.0;
    std::memcpy(&aDouble, memoryCopy, sizeof(double));
    // With 17 digits of precision, it is possible to convert a double back and forth to its binary
    // representation without any loss
    ss << std::setprecision(17) << aDouble;
    delete[] memoryCopy;
    return ss.str();
  }
  case Common::MemType::type_string:
  {
    return toUTF8String(memory, length, stringWidth);
  }
  case Common::MemType::type_byteArray:
  {
    // Force Hexadecimal, no matter the base
    ss << std::hex << std::uppercase;
    for (int i = 0; i < length; ++i)
    {
      u8 aByte = 0;
      std::memcpy(&aByte, memory + i, sizeof(u8));
      ss << std::setfill('0') << std::setw(2) << static_cast<int>(aByte) << " ";
    }
    std::string str = ss.str();
    // Remove the space at the end
    str.pop_back();
    return str;
  }
  default:
    return "";
    break;
  }
}

// really dumb c++ standard makes ~codecvt protected...
template<class I, class E, class S>
struct codecvt : std::codecvt<I, E, S>
{
    ~codecvt()
    { }
};

std::string toUTF8String(const char* buf, int len, StrWidth stringWidth)
{
  if(stringWidth == StrWidth::utf_16)
  {
    const char16_t* newBuf = reinterpret_cast<const char16_t*>(buf);
    for(int i = 0; i < len / sizeof(char16_t); i++)
    {
      if(newBuf[i] == 0)
      {
        len = i * sizeof(char16_t);
        break;
      }
    }
    std::wstring_convert<codecvt<char16_t, char, mbstate_t>, char16_t> converter;
    std::string converted;
    std::string originalStr = flipEndianness(std::string(buf, len), sizeof(char16_t));
    const char* loc = originalStr.c_str();
    try { converted = converter.to_bytes(reinterpret_cast<const char16_t*>(loc), reinterpret_cast<const char16_t*>(loc + len)); }
    catch(...) {}
    return converted;
  }
  else if(stringWidth == StrWidth::utf_32)
  {
    const char32_t* newBuf = reinterpret_cast<const char32_t*>(buf);
    for(int i = 0; i < len / sizeof(char32_t); i++)
    {
      if(newBuf[i] == 0)
      {
        len = i * sizeof(char32_t);
        break;
      }
    }
    std::wstring_convert<codecvt<char32_t, char, std::mbstate_t>, char32_t> converter;
    std::string converted;
    std::string originalStr = flipEndianness(std::string(buf, len), sizeof(char32_t));
    const char* loc = originalStr.c_str();
    try { converted = converter.to_bytes(reinterpret_cast<const char32_t*>(loc), reinterpret_cast<const char32_t*>(loc + len)); }
    catch(...) {}
    return converted;
  } 
  else
  {
    for(int i = 0; i < len; i++)
    {
      if(buf[i] == 0)
      {
        len = i;
        break;
      }
    }
    return std::string(buf, len);
  }
}

std::string convertFromUTF8(const char* buf, int len, StrWidth desiredWidth)
{
  if(desiredWidth == StrWidth::utf_16)
  {
    std::wstring_convert<codecvt<char16_t, char, mbstate_t>, char16_t> converter;
    std::u16string tmpString;
    try { tmpString = converter.from_bytes(buf, buf + len); }
    catch(...) {}
    return flipEndianness(std::string(reinterpret_cast<const char*>(tmpString.c_str()), tmpString.size() * sizeof(std::u16string::value_type)), sizeof(std::u16string::value_type));
  }
  else if(desiredWidth == StrWidth::utf_32)
  {
    std::wstring_convert<codecvt<char32_t, char, mbstate_t>, char32_t> converter;
    std::u32string tmpString;
    try { tmpString = converter.from_bytes(buf, buf + len); }
    catch(...) {}
    return flipEndianness(std::string(reinterpret_cast<const char*>(tmpString.c_str()), tmpString.size() * sizeof(std::u32string::value_type)), sizeof(std::u32string::value_type));
  } 
  else
  {
    return std::string(buf, len);
  }
}

std::string flipEndianness(std::string input, int charWidth)
{
  for(int i = 0; i < input.size(); i += charWidth)
  {
    for(int n = 0; n < charWidth / 2; n++)
    {
      char tmp = input[i + n];
      int idx = (i + charWidth - n) - 1;
      input[i + n] = input[idx];
      input[idx] = tmp;
    }
  }
  return input;
}

} // namespace Common
