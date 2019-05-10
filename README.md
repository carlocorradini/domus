# ![alt text](https://github.githubassets.com/images/icons/emoji/unicode/1f3e1.png) Domus

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

Firstly, clone this repo on your host.
~~~~~ bash
$ git clone git@github.com:carlocorradini/domus.git
~~~~~
Then, compile everything by typing:
~~~~~ bash
$ cd project
$ make build
~~~~~
This command will create a build directory under **project**

To run Domus, go to **project/build/bin** and run **domus**:
~~~~~ bash
$ cd project/build/bin
$ ./domus
~~~~~

## Supported features

### Supported commands
~~~~~ bash
        add <device>                       Add a <device> to the system and show its features
        clear                              Clear the CLI interface
        del <id> [--all]                   Delete the device with <id>. If [--all] delete all devices. If it's a control device, deletion is done recursively
        device                             Display all supported devices and their description
        exit                               Close Domus
        help                               Display help information about Domus
        hierarchy                          Display the current devices hierarchy in the system, described by <name> <id>
        info <id> [--all]                  Show device info with <id>. Show all devices info with [--all]
        link <id> to <id>                  Connect two devices each other. One must be a control device
        list                               Display all available devices and their features
        switch <id> <label> <pos>          Switch the device with <id> the feature <label> into <pos>
~~~~~

### Extra functionalities
- history navigation using <img src="https://github.githubassets.com/images/icons/emoji/unicode/2b06.png" alt="drawing" width="30"/> and <img src="https://github.githubassets.com/images/icons/emoji/unicode/2b07.png" alt="drawing" width="30"/>
- autocomplete using Tab 
- get info about command by adding **?** at the end of it (ex. *help ?* )

## Built With

* [GNU Make](https://www.gnu.org/software/make/) - GNU Make is a tool which controls the generation of executables and other non-source files of a program from the program's source files.
* [Git](https://git-scm.com) - Git is a free and open source distributed version control system designed to handle everything from small to very large projects with speed and efficiency

## Versioning

We use [Git](https://git-scm.com) for versioning.

## Authors

- **Carlo Corradini** - _Initial work_ - 192451
- **Simone Nascivera** - _Initial work_ - 193246

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details