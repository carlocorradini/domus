
#include "device/interaction/device_bulb.h"
#include "printer.h"
#include <signal.h>

static void sighup();

Bulb* init_bulb(pid_t fid){
    Bulb *bulb = (Bulb *) malloc(sizeof(Bulb));

    if (bulb == NULL) {
        perror("Bulb Memory Allocation");
    }

    bulb->PID = fid;
    bulb->state = false;
    println_color(COLOR_BLUE, "\nSuccessfully started new Bulb with PID %d\n", fid);

    return bulb;
}

void bulb_life(Bulb* bulb){
    signal(SIGHUP, sighup);

    for (;;)
        ; /* loop for ever */
}

static void sighup(){
    signal(SIGHUP, sighup);
    print_color(COLOR_YELLOW, "\nCambio\n");
}