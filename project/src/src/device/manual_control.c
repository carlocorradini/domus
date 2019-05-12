#include "device/manual_control.h"
#include "device/device_communication.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "string.h"
#include "util/util_converter.h"
#include "cli/cli.h"
#include "cli/command/command.h"


int main() {
    manual_command_init();
    cli_start();
    return 0;
}
