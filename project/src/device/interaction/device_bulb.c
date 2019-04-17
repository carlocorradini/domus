
#include "device/interaction/device_bulb.h"
#include "printer.h"
#include <signal.h>

static void sighup();

/*
 * my poor bulb shared between all this functions
 */
Bulb* my_bulb;

Bulb* init_bulb(pid_t fid){
    Bulb *bulb = (Bulb *) malloc(sizeof(Bulb));

    if (bulb == NULL) {
        perror("Bulb Memory Allocation");
    }

    bulb->PID = fid;
    bulb->state = false;
    println_color(COLOR_BLUE, "\nSuccessfully started new Bulb with PID %d\n", fid);

    my_bulb = bulb;
    return bulb;
}

void bulb_life(Bulb* bulb){
    signal(SIGHUP, sighup);

    for (;;)
        ; /* loop for ever */
}

/*
 * When I receive a sighub signal, this should be run
 */
static void sighup(){
    my_bulb->state = !my_bulb->state;
    println_color(COLOR_BLUE, "\nHere %d\n", my_bulb->state);
}

int main(){
    println_color(COLOR_BLUE, "\nI'm alive\n");
}