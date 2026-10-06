#include "DataFiles.hpp"

bool DataFiles::_sdFileExists(const char* filename) {
#ifdef USE_SD
    // Attempt to open the file strictly in read-only mode
    File testFile = SD.open(filename, FILE_READ);

    if (testFile) {
        // If it evaluates to true, the file opened successfully and exists!
        testFile.close(); // Clean up the file handle immediately
        return true;
    }
#endif

    // If it failed to open, the file does not exist on the SD card
    return false;
}
