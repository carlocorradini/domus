
#include "util/util_os.h"

bool os_windows(void) {
    return WIN_32_OS || WIN_64_OS;
}

bool os_windows_32(void) {
    return WIN_32_OS;
}

bool os_windows_64(void) {
    return WIN_64_OS;
}

bool os_mac_os(void) {
    return MAC_OS_OS;
}

bool os_linux(void) {
    return LINUX_OS;
}

bool os_unix(void) {
    return UNIX_OS;
}