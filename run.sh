#!/bin/sh

DEVNODE=/dev/input/by-id/usb-Metadot_-_Das_Keyboard_D4269-event-kbd
intercept -g $DEVNODE | ./dfk | uinput -d $DEVNODE
