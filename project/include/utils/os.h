
#ifndef OS_H
#define OS_H

#include <stdbool.h>

/**
 * Check if Operating System is Windows
 * @return true if windows, false otherwise
 */
bool os_windows(void);

/**
 * Check if Operating System is Windows 32bit
 * @return true if windows 32bit, false otherwise
 */
bool os_windows_32(void);

/**
 * Check if Operating System is Windows 64bit
 * @return true if windows 64bit, false otherwise
 */
bool os_windows_64(void);

/**
 * Check if Operating System is Mac
 * @return true if mac, false otherwise
 */
bool os_mac(void);

/**
 * Check if Operating System is Linux
 * @return true if linux, false otherwise
 */
bool os_linux(void);

#endif
