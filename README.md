# Domus

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

- ### Preparation

  1. **Clone** this repo on your host using [Git](https://git-scm.com)

     ```console
     $ git clone https://github.com/carlocorradini/domus.git
     ```

  2. **Change** current working **directory**

     ```console
     $ cd domus/project/
     ```

  3. **Compile** _Domus_ using [GNU Make](https://www.gnu.org/software/make/)

     > After compilation a `build` directory will be generated.
     >
     > `build` contains two subfolders:
     >
     > - `bin`
     >
     >   _Domus_ binaries
     >
     > - `obj`
     >
     >   _Domus_ object files

     ```console
     $ make build
     ```

- ### Run

  > All binaries are under `build/bin/` folder

  1. **Domus**

     > _Domus_ main program for controlling Devices automatically

     ```console
     $ ./domus
     ```

  2. **Domus Manual**

     > _Domus_ manual controller for Human interaction

     ```console
     $ ./domus_manual
     ```

- ### Connect

  > Perform the connection between _Domus Manual_ and _Domus_.
  >
  > Launch both programs as described above, and type:
  >
  > - `Domus`
  >
  >   Get _Domus_ unique PID
  >
  >   ```console
  >   > connect
  >   ```
  >
  > - `Domus Manual`
  >
  >   Connect to Domus using unique PID
  >
  >   ```console
  >   > connect <PID>
  >   ```
  >
  >   <br/>

## Supported commands

> Below is a table of supported command in _Domus_ & _Domus Manual_

- ### Domus

  | Command                     | Description                                                                                                            |
  | --------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
  | `add <device> [name]`       | Add a `<device>` to the system and show its features. Add `[name]` to define a custom name for the `<device>`          |
  | `clear`                     | Clear the CLI interface                                                                                                |
  | `del <id> [--all]`          | Delete the device with `<id>`. If `[--all]` delete all devices. If it's a control device, deletion is done recursively |
  | `device`                    | Display all supported devices and their description                                                                    |
  | `exit`                      | Close _Domus_                                                                                                          |
  | `help`                      | Display help information about _Domus_                                                                                 |
  | `hierarchy`                 | Display the current devices hierarchy in the system, described by `[name] <id>`                                        |
  | `info <id> [--all]`         | Show device info with `<id>`. Show all devices info with [--all]                                                       |
  | `link <id> to <id>`         | Connect two devices each other. One must be a control device                                                           |
  | `list`                      | Display all available devices and their features                                                                       |
  | `switch <id> <label> <pos>` | Switch the device with `<id>` the feature `<label>` into `<pos>`                                                       |
  | `connect`                   | Get unique _Domus_ `PID` for connecting _Domus Manual_ control interface to _Domus_                                    |

- ### Domus Manual

  | Command                     | Description                                                               |
  | --------------------------- | ------------------------------------------------------------------------- |
  | `clear`                     | Clear the CLI interface                                                   |
  | `connect <PID>`             | Connect to _Domus_ using unique `<PID>`                                   |
  | `device`                    | Display all supported devices and their description                       |
  | `exit`                      | Close _Domus_                                                             |
  | `help`                      | Display help information about _Domus_                                    |
  | `switch <id> <label> <pos>` | Manually switch the device with `<id>` the feature `<label>` into `<pos>` |

## Extra functionalities

- History navigation using arrows: **`↑`** & **`↓`**
- Autocomplete using **`Tab`**
- Info about command by adding **`?`** at the end of it (ex. _help ?_ )

## Built With

- [GNU Make](https://www.gnu.org/software/make/) - GNU Make is a tool which controls the generation of executables and other non-source files of a program from the program's source files.
- [Git](https://git-scm.com) - Git is a free and open source distributed version control system designed to handle everything from small to very large projects with speed and efficiency

## Versioning

We use [Git](https://git-scm.com) for versioning

## Authors

- **Carlo Corradini** - _Initial work_ - 192451
- **Simone Nascivera** - _Initial work_ - 193246

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

&copy; Domus 2019
