#!/bin/bash

PATH_BASHRC="$HOME/.bashrc"

if [ -f "$PATH_BASHRC" ]; then
    echo ".bashrc exists in your home directory."

    
    echo "export HELLO=$HOSTNAME" >> "$PATH_BASHRC"
    echo "LOCAL=\$(whoami)" >> "$PATH_BASHRC"

  
    gnome-terminal &
else
    echo ".bashrc not found in $HOME"
fi
