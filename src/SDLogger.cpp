#include "SDLogger.h"

SDLogger::SDLogger() {}

SDLogger::~SDLogger() {}

void SDLogger::init() {
    if(cardMounted) return;
    
    // Mount the SD card.
    FR_Status = f_mount(&FatFs, "", 1);
	if (FR_Status != FR_OK) {
	  return;
	}

	// Get SD Card size and free space.
    f_getfree("", &FreeClusters, &FS_Ptr);
	TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
	FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);

    cardMounted = 1;
}

void SDLogger::startLogging(const char* logFileName) {
	if(logging) return;

    if(!cardMounted) {
        // Try to mount if not mounted.
        init();
        if(!cardMounted) return;
    }

    FR_Status = f_open(&Fil, logFileName, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
	if(FR_Status != FR_OK) {
	    return;
	}

    f_puts("#\n", &Fil);
    f_puts(logFileName, &Fil);
    f_puts("\n#\n", &Fil);
    f_sync(&Fil);

    logging = 1;
}

void SDLogger::stopLogging() {
	if(!cardMounted || !logging) return;

    f_close(&Fil);

    logging = 0;
}


void SDLogger::addLogLine(const char* logLine) {
    if(!cardMounted || !logging) return;

    f_puts(logLine, &Fil);
    f_sync(&Fil);
}

uint32_t SDLogger::getLogFileSize() {
    if(!cardMounted || !logging) return 0;

    return f_size(&Fil);
}
