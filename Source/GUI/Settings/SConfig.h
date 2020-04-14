#pragma once

#include <QSettings>

#include "../../Common/CommonTypes.h"

class SConfig
{
public:
  static SConfig& getInstance();
  SConfig(SConfig const&) = delete;
  void operator=(SConfig const&) = delete;

  int getWatcherUpdateTimerMs() const;
  int getFreezeTimerMs() const;
  int getScannerUpdateTimerMs() const;
  int getViewerUpdateTimerMs() const;
  u32 getMEM1Size() const;
  u32 getMEM2Size() const;

  int getViewerNbrBytesSeparator() const;

  void setWatcherUpdateTimerMs(const int watcherUpdateTimerMs);
  void setFreezeTimerMs(const int freezeTimerMs);
  void setScannerUpdateTimerMs(const int scannerUpdateTimerMs);
  void setViewerUpdateTimerMs(const int viewerUpdateTimerMs);
  void setMEM1Size(const u32 g_mem1_size_real);
  void setMEM2Size(const u32 g_mem2_size_real);

  void setViewerNbrBytesSeparator(const int viewerNbrBytesSeparator);

private:
  SConfig();
  ~SConfig();

  QSettings* m_settings;
};
