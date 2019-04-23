
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "device/control/device_controller.h"
#include "device/device_communication.h"
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
 * @param pid The parent pid
 * @param device_descriptor The descriptor of the device to be created
 */
static void controller_fork_child(pid_t pid, const DeviceDescriptor *device_descriptor);

/**
 * Add the new child process to the controller devices list
 * @param pid The pid of the child process
 */

/**
 * Add the new child process to the controller devices list for communication
 * @param pid The pid of the child process
 * @param device_descriptor The Device Descriptor of the new process
 * @param com_read File Descriptor for parent -> READ
 * @param com_write File Descriptor for parent -> WRITE
 */
static void controller_fork_parent(pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write);

/**
 * Change the state of the controller
 * @param state The state to change to
 * @return true if the operation was successful, false otherwise
 */
static bool controller_master_switch(bool state);

/**
 * Wake Up the controller to read the read pipes
 * @param signal_number SIGUSR1 signal for reading
 */
static void controller_read(int signal_number);

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
    /* Attach to SIGUSR1 the signal to force the controller to check new messages */
    signal(SIGUSR1, controller_read);

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

bool controller_fork_device(const DeviceDescriptor *device_descriptor) {
    pid_t pid_child;
    pid_t pid_parent;
    int write_parent_read_child[2];
    int write_child_read_parent[2];
    if (device_descriptor == NULL) return false;

    if (pipe(write_parent_read_child) == -1
        || pipe(write_child_read_parent) == -1) {
        perror("Controller Fork Pipe");
        exit(EXIT_FAILURE);
    }
    if (fcntl(write_parent_read_child[0], F_SETFL, O_NONBLOCK) == -1
        || fcntl(write_child_read_parent[0], F_SETFL, O_NONBLOCK) == -1) {
        perror("Controller Fork fcntl");
        exit(EXIT_FAILURE);
    }

    pid_parent = getpid();
    switch (pid_child = fork()) {
        case -1: {
            perror("Controller Fork Forking");
            exit(EXIT_FAILURE);
        }
        case 0: {
            close(write_parent_read_child[1]);
            close(write_child_read_parent[0]);
            /* Attach child stdout to write child pipe */
            dup2(write_child_read_parent[1], DEVICE_COMMUNICATION_CHILD_WRITE);

            controller_fork_child(pid_parent, device_descriptor);
            break;
        }
        default: {
            close(write_parent_read_child[0]);
            close(write_child_read_parent[1]);

            controller_fork_parent(pid_child, device_descriptor, write_child_read_parent[0],
                                   write_parent_read_child[1]);
            break;
        }
    }

    return true;
}

static void controller_fork_child(pid_t pid, const DeviceDescriptor *device_descriptor) {
    char device_name[DEVICE_NAME_LENGTH];
    char parent_pid[10];
    if (device_descriptor == NULL) return;

    strncpy(device_name, device_descriptor->name, DEVICE_NAME_LENGTH);
    sprintf(parent_pid, "%d", pid);

    char *const device_args[] = {
            parent_pid,
            device_name,
            NULL
    };

    if (execv(device_descriptor->file_name, device_args) == -1) {
        perror("Error exec Controller Fork Child");
        exit(EXIT_FAILURE);
    }
}

static void controller_fork_parent(pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write) {
    ControllerRegistry *registry;
    if (!controller_check_controller()) return;
    if (pid < 0 || com_read < 0 || com_write < 0) return;

    list_add_last(controller->devices, new_device_communication(pid, device_descriptor, com_read, com_write));
    registry = (ControllerRegistry *) controller->device->registry;
    registry->connected_directly = registry->connected_total = controller->devices->size;
}

static void controller_read(int signal_number) {
    if (signal_number == SIGUSR1) {
        DeviceCommunication *data;
        if (!controller_check_controller()) return;

        list_for_each(data, controller->devices) {
            device_communication_read(data->com_read);
        }
    }
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