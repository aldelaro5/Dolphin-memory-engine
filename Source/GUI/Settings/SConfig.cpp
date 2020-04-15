#include "SConfig.h"

SConfig::SConfig()
{
  m_settings = new QSettings("settings.ini", QSettings::IniFormat);
}

SConfig::~SConfig()
{
  delete m_settings;
}

SConfig& SConfig::getInstance()
{
  static SConfig instance;
  return instance;
}

int SConfig::getWatcherUpdateTimerMs() const
{
  return m_settings->value("timerSettings/watcherUpdateTimerMs", 100).toInt();
}

int SConfig::getFreezeTimerMs() const
{
  return m_settings->value("timerSettings/freezeTimerMs", 10).toInt();
}

int SConfig::getScannerUpdateTimerMs() const
{
  return m_settings->value("timerSettings/scannerUpdateTimerMs", 10).toInt();
}

int SConfig::getViewerUpdateTimerMs() const
{
  return m_settings->value("timerSettings/viewerUpdateTimerMs", 100).toInt();
}

int SConfig::getViewerNbrBytesSeparator() const
{
  return m_settings->value("viewerSettings/nbrBytesSeparator", 1).toInt();
}

u32 SConfig::getMEM1Size() const
{
  return m_settings->value("memorySettings/MEM1Size", 0x01800000U).toUInt();
}

u32 SConfig::getMEM2Size() const
{
  return m_settings->value("memorySettings/MEM2Size", 0x04000000U).toUInt();
}

void SConfig::setWatcherUpdateTimerMs(const int updateTimerMs)
{
  m_settings->setValue("timerSettings/watcherUpdateTimerMs", updateTimerMs);
}

void SConfig::setFreezeTimerMs(const int freezeTimerMs)
{
  m_settings->setValue("timerSettings/freezeTimerMs", freezeTimerMs);
}

void SConfig::setScannerUpdateTimerMs(const int scannerUpdateTimerMs)
{
  m_settings->setValue("timerSettings/scannerUpdateTimerMs", scannerUpdateTimerMs);
}

void SConfig::setViewerUpdateTimerMs(const int viewerUpdateTimerMs)
{
  m_settings->setValue("timerSettings/viewerUpdateTimerMs", viewerUpdateTimerMs);
}

void SConfig::setViewerNbrBytesSeparator(const int viewerNbrBytesSeparator)
{
  m_settings->setValue("viewerSettings/nbrBytesSeparator", viewerNbrBytesSeparator);
}

void SConfig::setMEM1Size(const u32 g_mem1_size_real)
{
  m_settings->setValue("memorySettings/MEM1Size", g_mem1_size_real);
}

void SConfig::setMEM2Size(const u32 g_mem2_size_real)
{
  m_settings->setValue("memorySettings/MEM2Size", g_mem2_size_real);
}