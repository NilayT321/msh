# msh
My shell (`msh` for short) is a shell for Linux systems developed using C. 

The core functionality has been implemented so far and it can be used to do the basic functionality of a regular shell. Additional features and bug fixes will be implemented soon. Such features include 

- Signal handling
- More advanced job control 

## Usage 

At present, the core functionality of `msh` works as intended and can be used to perform simple tasks in the terminal. To test it out, one should clone this repository into a folder of their choice. After navigating to the `msh/` folder, a simple call to `make` 

```bash
$ make
```

should compile the program. It can then be run using 

```bash 
$ ./msh 
```

Again, the shell is still not complete and bugs are definitely present. 

## Known Bugs 

There are some known bugs when attempting to exit the shell, manifesting seemingly after an invalid operation has been completed. In this case, it is advised to not use the builtin `quit` command at this time. Rather, one should use Ctrl+D (end of file) characters to terminate the shell. Multiple inputs of Ctrl+D may be required at times. 
