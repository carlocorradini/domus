#ifndef _COMMAND_CONNECT_MANUAL_H
#define _COMMAND_CONNECT_MANUAL_H

#include "cli/command/command.h"
#include "cli/cli.h"
#include "util/util_converter.h"
#include "util/util_printer.h"
#include "device/control_libs.h"

/**
 * Definition of connect Command
 * @return The connect Command
 */
Command *command_connect_manual(void);

#endif
