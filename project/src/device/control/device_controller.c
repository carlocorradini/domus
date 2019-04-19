
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
    controller = new_control_device(
            new_device(getpid(),
                       DEVICE_STATE,
                       new_controller_registry(),
                       controller_master_switch),
            new_list(NULL, NULL));

    command_init();
    author_init();
    device_init();
}

static void controller_tini(void) {
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

static void controller_fork_parent(pid_t pid) {
    ControllerRegistry *registry;
    if (!controller_check_controller()) return;
    if (pid < 0) return;

    list_add_last(controller->devices, pid);
    registry = controller->device->registry;
    registry->connected_directly = registry->connected_total = controller->devices->size; /* \todo WARNING, CHANGE IN FUTURE*/
}

size_t controller_connected_directly(void) {
    ControllerRegistry *registry;
    if (!controller_check_controller()) return -1;

    registry = (ControllerRegistry *) controller->device->registry;
    if(controller->devices->size != registry->connected_directly) return -1;

    return registry->connected_directly;
}

size_t controller_connected_total(void) {
    ControllerRegistry *registry;
    if (!controller_check_controller()) return -1;

    registry = (ControllerRegistry *) controller->device->registry;

    return registry->connected_total;
}