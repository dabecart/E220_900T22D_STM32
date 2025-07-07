#ifndef SD_LOGGER_h
#define SD_LOGGER_h

#include <stdio.h>
#include "app_fatfs.h"

class SDLogger {
    public:
    SDLogger();
    ~SDLogger();

    void init();
    void startLogging(const char* logFileName);
    void stopLogging();
    void addLogLine(const char* logLine);

    uint32_t getLogFileSize();

    private:
    FATFS FatFs;
    FIL Fil;
    FRESULT FR_Status;
    FATFS *FS_Ptr;
    UINT RWC, WWC; // Read/Write Word Counter
    DWORD FreeClusters;
    uint32_t TotalSize, FreeSpace;
    uint32_t lastSync = 0;

    public:
    uint8_t cardMounted = 0;
    uint8_t logging = 0;
};

#endif // SD_LOGGER_h