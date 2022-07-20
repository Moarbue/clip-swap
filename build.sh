#! /bin/bash

gcc -Wall -Wextra -ggdb main.c -o clip_swap -L . -lclipboard -lpthread -lxcb