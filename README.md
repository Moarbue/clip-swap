# clip-swap
A simple cross-platform program, written in C, to swap your clipboard content.

![Alt Text](https://github.com/Moarbue/clip-swap/blob/main/demo.gif)

## How it works
The program needs a lookup-table where it can search for matching words. 
If the word that you copy to the clipboard is somewhere in the lookup-table, it will replace the copied word with the word associated to it.

It even works in both directions. However to prevent an infinite swapping-loop you first have to copy another word before swapping back.

## Word Table
The provided lookup-table for the program has to be in a certain format:
```
word1;word2
word3;word4
...
```
To add the lookup-table to the project simply add a file named __word_list.txt__ to the project directory and add your words to it.
Alternatively you can also launch the program in the command line and provide the word list as argument.
e.g:
```
./clip-swap <your_list>
```

## Running the program
On Windows, the console which hosts the program is hidden by default. To check if the program is running simply open the Task Manager and search for it.

On Linux the program isn't hidden by default but you can run it in the background by adding a __&__ on executing.

#### Stopping the program
This might be a trivial matter, but because there is no GUI or something like that, you have to stop the task yourself (e.g. Task Manager).

## Dependencies
clip-swap uses a cross-platform [clipboard library](https://github.com/jtanx/libclipboard) which depends on ***cmake/make***. Additionally __git__ is needed for building. 

Dependencies:
```
git, cmake, make
```

## Installation
Clone this repository and run the following command inside the directory.

__Linux__
```sh
./build.sh
```
__Windows (Powershell/cmd)__
```sh
bash -c ./build.sh
```

Additionally you can set the compiler (default __gcc__), name of the executable (default __clip-swap__) or compilation flags (default __-Wall -Wextra -O2__).
```sh
# set compiler to the msv-compiler and set binary name to cbswappy-uwu
CC=cl.exe EXE=cbswappy-uwu ./build.sh
```

## Platforms
The program was tested on Arch Linux, Ubuntu (should work on every other distro too), on Windows using gcc with the mingw-toolchain and Windows with the MSVCompiler.

## Issues
If the CMake generator is not recognized correctly consider specifying it yourself in the __build.sh__ file:
```sh
# run this to get available generators
cmake -G
```
Inside build.sh:
```sh
# replace
cmake .

# with
cmake -G <generator> .
```