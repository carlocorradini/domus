
#include <string.h>
#include "device/control/device_controller.h"
#include "cli/command/command.h"
#include "cli/cli.h"
#include "author.h"

/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist in the entire program!!!
 */
static ControlDevice *controller = NULL;

/**
 * Initialize all Controller Components
 */
static void controller_init(void);

/**
 * Free all Controller Components
 */
static void controller_tini(void);

/**
 * Check if the controller is correctly initialized
 * @return true if correctly initialized, false otherwise
 */
static bool controller_check_controller(void);

/**
 * Replaces the current running process with a new device process described in the Device Descriptor
 * @param device_descriptor The descriptor of the device to be created
 */
static void controller_fork_child(const DeviceDescriptor *device_descriptor);

/**
 * Add the new child process to the controller devices list
 * @param pid The pid of the child process
 */
static void controller_fork_parent(pid_t pid);

/**
 * Change the state of the controller
 * @param state The state to change to
 * @return true if the operation was successful, false otherwise
 */
static bool controller_master_switch(bool state);

void controller_start(void) {
    controller_init();
    cli_start();
    controller_tini();
}

static void controller_init(void) {
    /* Create the controller, only once in the entire program with id 0 */
    controller = new_control_device(
            new_device(getpid(),
                       0,
                       DEVICE_STATE,
                       new_controller_registry(),
                       controller_master_switch),
            new_list(NULL, NULL));

    command_init();
    author_init();
    device_init();
}

static void controller_tini(void) {
    free_control_device(controller);
    command_tini();
    author_tini();
    device_tini();
}

static bool controller_master_switch(bool state) {
    if (!controller_check_controller()) return false;
    controller->device->state = state;
    return true;
}

ControllerRegistry *new_controller_registry(void) {
    ControllerRegistry *controller_registry;
    if (controller_check_controller()) return NULL;

    controller_registry = (ControllerRegistry *) malloc(sizeof(ControllerRegistry));
    if (controller_registry == NULL) {
        perror("Controller Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    controller_registry->connected_directly = controller_registry->connected_total = 0;

    return controller_registry;
}

static bool controller_check_controller(void) {
    return controller != NULL && controller->device != NULL && controller->devices != NULL &&
           controller->device->registry != NULL && controller->device->master_switch != NULL;
}

/*
bool controller_fork_device(const DeviceDescriptor *device_descriptor) {
    pid_t pid;
    if (device_descriptor == NULL) return false;

    pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Controller Fork: Unable to fork\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        controller_fork_child(device_descriptor);
    } else {
        controller_fork_parent(pid);
        return true;
    }
    return false;
}
*/

/* /todo PIPE SUPPORT EXAMPLE!*/
#include <sys/types.h>
#include <sys/wait.h>

bool controller_fork_device(const DeviceDescriptor *device_descriptor) {
    int pid, n, c, p, k, nbread;
    char buf1[12], buf2[12];
    int fd1[2], fd2[2];

    pipe(fd1);
    pipe(fd2);
    pid = fork();

    if (pid == 0) {
        close(fd1[1]);
        close(fd2[0]);
        read(fd1[0], buf2, sizeof(buf2));
        n = atoi(buf2);
        printf("Child read %d\n", n);
        int i;
        for (i = 0; i < n; i++) {
            printf("child dozes...\n");
            sleep(3);
            printf("child wakes...\n");
            nbread = read(fd1[0], buf2, sizeof(buf2));
            if (nbread == -1) {
                fprintf(stderr, "child exits after read failure\n");
                exit(1);
            }
            c = atoi(buf2);
            c = c * 2;
            sprintf(buf2, "%d", c);
            write(fd2[1], buf2, sizeof(buf2));
            printf("Child wrote [%s]\n", buf2);
        }
        close(fd1[0]);
        close(fd2[1]);
        printf("Child done\n");
        exit(0);
    } else {
        close(fd1[0]);
        close(fd2[1]);
        printf("Enter integer: ");
        scanf("%d", &p);
        sprintf(buf1, "%d", p);
        write(fd1[1], buf1, sizeof(buf1));
        printf("Parent wrote [%s]\n", buf1);
        printf("parent dozes...\n");
        sleep(3);
        printf("parent wakes...\n");
        int i;
        for (i = 0; i < p; i++) {
            sprintf(buf1, "%d", i);
            write(fd1[1], buf1, sizeof(buf1));
            printf("parent wrote [%s]\n", buf1);
            read(fd2[0], buf2, sizeof(buf2));
            printf("number is: %s\n", buf2);
        }
        close(fd1[1]);
        close(fd2[0]);
        wait(NULL);
    }
}

static void controller_fork_child(const DeviceDescriptor *device_descriptor) {
    if (device_descriptor == NULL) return;

    char device_name[DEVICE_NAME_LENGTH];
    strncpy(device_name, device_descriptor->name, DEVICE_NAME_LENGTH);
    char *const device_args[] = {
            device_name,
            NULL
    };

    execv(device_descriptor->file_name, device_args);
}

/*\todo WARNING, CHANGE IN FUTURE*/
static void controller_fork_parent(pid_t pid) {
    ControllerRegistry *registry;
    /* todo Future this is a Struct */
    pid_t *pid_child;
    if (!controller_check_controller()) return;
    if (pid < 0) return;

    pid_child = (pid_t *) malloc(sizeof(pid_t));
    if (pid_child == NULL) {
        perror("Controller Fork Parent Memory Allocation");
        exit(EXIT_FAILURE);
    }

    *pid_child = pid;

    list_add_last(controller->devices, pid_child);
    registry = controller->device->registry;
    registry->connected_directly = registry->connected_total = controller->devices->size;
}

size_t controller_connected_directly(void) {
    ControllerRegistry *registry;
    if (!controller_check_controller()) return -1;

    registry = (ControllerRegistry *) controller->device->registry;
    if (controller->devices->size != registry->connected_directly) return -1;

    return registry->connected_directly;
}

size_t controller_connected_total(void) {
    ControllerRegistry *registry;
    if (!controller_check_controller()) return -1;

    registry = (ControllerRegistry *) controller->device->registry;

    return registry->connected_total;
}