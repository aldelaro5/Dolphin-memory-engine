#include "DolphinAccessor.h"
#ifdef __linux__
#include "Linux/LinuxDolphinProcess.h"
#elif _WIN32
#include "Windows/WindowsDolphinProcess.h"
#elif __APPLE__
#include "Mac/MacDolphinProcess.h"
#endif

#include <cstring>
#include <memory>

#include "../Common/CommonUtils.h"
#include "../Common/MemoryCommon.h"

namespace DolphinComm
{
IDolphinProcess* DolphinAccessor::m_instance = nullptr;
DolphinAccessor::DolphinStatus DolphinAccessor::m_status = DolphinStatus::unHooked;

void DolphinAccessor::init()
{
  if (m_instance == nullptr)
  {
#ifdef __linux__
    m_instance = new LinuxDolphinProcess();
#elif _WIN32
    m_instance = new WindowsDolphinProcess();
#elif __APPLE__
    m_instance = new MacDolphinProcess();
#endif
  }
}

void DolphinAccessor::free()
{
    delete m_instance;
}

void DolphinAccessor::hook()
{
  init();
  if (!m_instance->findPID())
  {
    m_status = DolphinStatus::notRunning;
  }
  else if (!m_instance->obtainEmuRAMInformations())
  {
    m_status = DolphinStatus::noEmu;
  }
  else
  {
    m_status = DolphinStatus::hooked;
  }
}

void DolphinAccessor::unHook()
{
  delete m_instance;
  m_instance = nullptr;
  m_status = DolphinStatus::unHooked;
}

DolphinAccessor::DolphinStatus DolphinAccessor::getStatus()
{
  return m_status;
}

bool DolphinAccessor::readFromRAM(const u32 offset, char* buffer, const size_t size,
                                  const bool withBSwap)
{
  return m_instance->readFromRAM(offset, buffer, size, withBSwap);
}

bool DolphinAccessor::writeToRAM(const u32 offset, const char* buffer, const size_t size,
                                 const bool withBSwap)
{
  return m_instance->writeToRAM(offset, buffer, size, withBSwap);
}

int DolphinAccessor::getPID()
{
  return m_instance->getPID();
}

u64 DolphinAccessor::getEmuRAMAddressStart()
{
  return m_instance->getEmuRAMAddressStart();
}

bool DolphinAccessor::isMEM2Present()
{
  return m_instance->isMEM2Present();
}

bool DolphinAccessor::isValidConsoleAddress(const u32 address)
{
  if (getStatus() != DolphinStatus::hooked)
    return false;

  bool isMem1Address = address >= Common::MEM1_START && address < Common::MEM1_END;
  if (isMEM2Present())
    return isMem1Address || (address >= Common::MEM2_START && address < Common::MEM2_END);
  return isMem1Address;
}

std::string DolphinAccessor::getFormattedValue(const u32 ramIndex, Common::MemType memType,
                                               size_t memSize, Common::MemBase memBase,
                                               bool memIsUnsigned)
{
  auto data = std::make_unique<char[]>(memSize);
  readFromRAM(ramIndex, data.get(), memSize, false);
  return Common::formatMemoryToString(data.get(), memType, memSize, memBase,
                                      memIsUnsigned, Common::shouldBeBSwappedForType(memType));
}

void DolphinAccessor::copyRawMemory(char* dest, const u32 consoleAddress,
                                    const size_t byteCount)
{
  if (isValidConsoleAddress(consoleAddress) &&
      isValidConsoleAddress((consoleAddress + static_cast<u32>(byteCount)) - 1))
  {
    readFromRAM(Common::dolphinAddrToOffset(consoleAddress), dest, byteCount, false);
  }
}
} // namespace DolphinComm
