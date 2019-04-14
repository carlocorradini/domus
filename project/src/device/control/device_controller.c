
#include <signal.h>
#include "device/device.h"
#include "device/control/device_controller.h"
#include "device/interaction/device_bulb.h"
#include "cli/command/command.h"
#include "author.h"
#include "cli/cli.h"
#include "printer.h"


/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist!!!
 */
static Device controller = {CONTROLLER_STATE, CONTROLLER_MASTER_SWITCH, CONTROLLER_REGISTRY};

/**
 * Initialize all Controller Components
 */
static void controller_init(void);

/**
 * Free all Controller Components
 */
static void controller_free(void);

void controller_start(void) {
    controller_init();
    cli_start();
    controller_free();
}

static void controller_init(void) {
    commands_init();
    authors_init();
}

static void controller_free(void) {
    commands_free();
    authors_free();
}

/**
 * Spawn new device process by forking the current one
 * @param device is used to choose which device has to be spawned
 */
void controller_process_spawn(int device) {
    pid_t fid;
    fid = fork();
    /*
     *  if fid = 0, then it's the child, otherwise it's the child fid
     */
    println_color(COLOR_YELLOW, "My PID: %d\n", getpid());
    if (fid == -1) {
        println_color(COLOR_RED, "Error: current process cannot be forked");
    }
    else {
        if (fid == 0) {
            pid_t my_pid = getpid();
            switch (device) {


                Bulb *bulb;
                case DEVICE_BULB:
                    /*
                     * if it's a bulb, then init it
                     * and "start its life"
                     */
                    bulb = init_bulb(my_pid);
                    bulb_life(bulb);
                    break;

                default:
                    println_color(COLOR_RED, "Device not implemented yet");
            }
        }
        else {
            /*
             * Simple test to check if signals work:
             * simply send a signal every 1 second.
             */
            while(1){
                sleep(1);
                kill(fid, SIGHUP);
            }
        }
    }

}