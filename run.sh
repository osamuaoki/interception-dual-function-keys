#!/bin/sh

DEVNODE=/dev/input/by-id/usb-Kinesis_Advantage2_Keyboard_314159265359-if01-event-kbd
intercept -g $DEVNODE | ./dfk | uinput -d $DEVNODE
