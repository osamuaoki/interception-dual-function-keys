## NAME

dfk - dual function keys

## SYNOPSIS

dfk \[*-h*\] -c \[*yaml config path*\]

## DESCRIPTION

Tap for one key, hold for another.

Great for modifier keys e.g. hold for ctrl, tap for delete.

A plugin for [interception tools](https://gitlab.com/interception/linux).

## FUNCTIONALITY

In these examples we will use the left shift key (LS). It is configured to tap for backspace (BS) and hold for LS.

### Tap

Press and release within TAP\_MILLIS (default 200ms) for ESC. Until the tap is complete, we get LS.

``` 
                <---------200ms--------->     <---------200ms--------->
keyboard:       LS↓      LS↑                  LS↓                          LS↑
computer sees:  LS↓      LS↑ BS↓ BS↑          LS↓                          LS↑
```

### Double Tap

Tap then press again with DOUBLE\_TAP\_MILLIS (default 150ms) to hold BS.

``` 
                             <-------150ms------->
                <---------200ms--------->
keyboard:       LS↓         LS↑             LS↓                LS↑
computer sees:  LS↓         LS↑ BS↓ BS↑     BS↓   (repeats)    BS↑
```

You can continue double tapping so long as it is within the DOUBLE\_TAP\_MILLIS window.

### Consumption

Press or release another key during the TAP\_MILLIS window and the tap will not occur.

This is especially useful for modifiers e.g. a quick ctrl-C. In this example we press the ‘a’ key during the window.

Double taps do not apply after consumption; you will need to tap first.

``` 
                                                               <-------150ms------->
                                                 <---------200ms--------->
                                 <-------150ms------->
                <---------200ms--------->
keyboard:       LS↓   a↓  a↑     LS↑             LS↓          LS↑           LS↓
computer sees:  LS↓              LS↑             LS↓          LS↑ BS↓ BS↑   BS↓   (repeats)
```

## CONFIGURATION

There are two parts to be configured: dfk and udevmon, which launches dfk.

### dfk

``` yaml
TIMING:
    TAP_MILLISEC: 200
    DOUBLE_TAP_MILLISEC: 150

MAPPINGS:
    - KEY: KEY_A
      TAP: KEY_A
      HOLD: KEY_LEFTSHIFT

    - KEY: KEY_S
      TAP: KEY_S
      HOLD: KEY_LEFTCTRL

    - KEY: KEY_D
      TAP: KEY_D
      HOLD: KEY_LEFTALT

    - KEY: KEY_F
      TAP: KEY_F
      HOLD: KEY_LEFTMETA

    - KEY: KEY_J
      TAP: KEY_J
      HOLD: KEY_RIGHTMETA

    - KEY: KEY_K
      TAP: KEY_K
      HOLD: KEY_RIGHTALT

    - KEY: KEY_L
      TAP: KEY_L
      HOLD: KEY_RIGHTCTRL

    - KEY: KEY_SEMICOLON
      TAP: KEY_SEMICOLON
      HOLD: KEY_RIGHTSHIFT
```

### udevmon

``` yaml
- JOB: "intercept -g $DEVNODE | dfk -c /etc/dfk.home.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "Metadot - Das Keyboard D4269"

- JOB: "intercept -g $DEVNODE | /home/alex/src/interception/dfk/dfk -c /etc/dfk.home.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "OLKB Planck Keyboard"

- JOB: "intercept -g $DEVNODE | dfk -c /etc/dfk.ka2.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "Kinesis Advantage2 Keyboard"
    EVENTS:
      EV_KEY: [ KEY_LEFTSHIFT ]

- JOB: "intercept -g $DEVNODE | dfk -c /etc/dfk.ka2.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "ErgoDox EZ ErgoDox EZ Keyboard"

- JOB: "intercept -g $DEVNODE | dfk -c /etc/dfk.home.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "q.m.k HHKB mod Keyboard"
```

## FAQ

*I see you are using “q.m.k HHKB mod Keyboard” in your udevmon example. It’s using [QMK Firmware](https://qmk.fm/). Why not just use [Tap-Hold](https://docs.qmk.fm/#/tap_hold)?*

Good catch\! That does indeed provide the same functionality as dfk. Unfortunately there are some drawbacks:

1.  Few keyboards run QMK Firmware.
2.  There are some issues with that functionality, as noted in the doc(https://docs.qmk.fm/\#/tap\_hold).
3.  It requires a fast processor in the keyboard. My unscientific testing with an Ergodox (\~800 scans/sec) and HHKB (\~140) revealed that the slower keyboard is mushy and unuseably inaccurate.

*Why not use [xcape](https://github.com/alols/xcape)?*

Xcape only provides simple tap/hold functionality. It appears difficult (impossible?) to add the remaining functionality using its XTestFakeKeyEvent mechanisms.
