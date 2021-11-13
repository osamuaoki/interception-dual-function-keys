## NAME

interception - dual function keys

## DESCRIPTION

Tap for one key, hold for another.

Great for modifier keys like: hold for ctrl, tap for delete.

A hand-saver for those with restricted finger mobility.

A plugin for [interception tools](https://gitlab.com/interception/linux).

## FUNCTIONALITY

In these examples we will use the left shift key (LS).

It is configured to tap for delete (DE) and hold for LS.

### Tap

Press and release LS within TAP_MILLIS (default 200ms) for DE.

By default, until the tap is complete, we get LS. See [below](#changing-the-behavior-of-hold-keys) for other options.

``` text
                <---------200ms--------->     <---------200ms--------->
keyboard:       LS↓      LS↑                  LS↓                          LS↑
computer sees:  LS↓      LS↑ DE↓ DE↑          LS↓                          LS↑
```

### Double Tap

Tap then press again with DOUBLE_TAP_MILLIS (default 150ms) to hold DE.

``` text
                             <-------150ms------->
                <---------200ms--------->
keyboard:       LS↓         LS↑             LS↓               LS↑
computer sees:  LS↓         LS↑ DE↓ DE↑     DE↓ ..(repeats).. DE↑
```

You can continue double tapping so long as it is within the DOUBLE_TAP_MILLIS window.

### Consumption

Press or release another key during the TAP_MILLIS window and the tap will not occur.

This is especially useful for modifiers, for instance a quick ctrl-C. In this example we press the a key during the window.

Double taps do not apply after consumption; you will need to tap first.

Mouse and touchpad events (`EV_REL` and `EV_ABS`) can also consume taps, however you will need to use a [Multiple Devices](#multiple-devices) configuration.

``` text
                                                               <-------150ms------->
                                                 <---------200ms--------->
                                 <-------150ms------->
                <---------200ms--------->
keyboard:       LS↓      a↓  a↑  LS↑             LS↓          LS↑           LS↓
computer sees:  LS↓      a↓  a↑  LS↑             LS↓          LS↑ DE↓ DE↑   DE↓ ..(repeats)..
```

## INSTALLATION

### Arch Linux

Arch Linux users may install from the Arch community package repository: [interception-dual-function-keys](https://archlinux.org/packages/community/x86_64/interception-dual-function-keys/).

### From Source

See [runtime dependencies](https://gitlab.com/interception/linux/tools#runtime-dependencies).

Install [Interception Tools](https://gitlab.com/interception/linux/tools) first.

``` sh
git clone https://gitlab.com/interception/linux/plugins/dual-function-keys.git
cd dual-function-keys
make && sudo make install
```

Installation prefix defaults to `/usr/local`. This can be overridden in `config.mk`.

## CONFIGURATION

There are two parts to be configured: dual-function-keys and udevmon, which launches dual-function-keys.

See [examples](https://gitlab.com/interception/linux/plugins/dual-function-keys/-/tree/master/doc/examples.md) which contains dual-function-keys and udevmon.yaml configurations.

### dual-function-keys

This yaml file can go anywhere.

You can use raw (integer) keycodes, however it is easier to use the `#define`d strings from [input-event-codes.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h).

``` yaml
# optional
TIMING:
    TAP_MILLISEC: <integer>
    DOUBLE_TAP_MILLISEC: <integer>
    SYNTHETIC_KEYS_PAUSE_MILLISEC: <integer>

# necessary
MAPPINGS:
    - KEY: <integer | string>
      TAP: [ <integer | string>, ... ]
      HOLD: [ <integer | string>, ... ]
      # optional
      HOLD_START: [ AFTER_PRESS | BEFORE_CONSUME | BEFORE_CONSUME_OR_RELEASE ]
    - KEY: ...
```

Our example from the previous section looks like:

``` yaml
TIMING:
    TAP_MILLISEC: 200
    DOUBLE_TAP_MILLISEC: 150

MAPPINGS:
    - KEY: KEY_LEFTSHIFT
      TAP: KEY_DELETE
      HOLD: KEY_LEFTSHIFT
```

#### Combo Keys

You can configure the `TAP` as a “combo”, which will press then release multiple keys in order e.g. space cadet `(`:

``` yaml
MAPPINGS:
    - KEY: KEY_LEFTSHIFT
      TAP: [ KEY_LEFTSHIFT, KEY_9, ]
      HOLD: KEY_LEFTSHIFT
```

You can configure the `HOLD` as a “combo”, which will press then release multiple keys in order e.g. hyper modifier:

``` yaml
MAPPINGS:
    - KEY: KEY_TAB
      TAP: KEY_TAB
      HOLD: [ KEY_LEFTCTRL, KEY_LEFTMETA, KEY_LEFTALT, ]
```

By default, there will be a pause of 20ms between keys in the “combo”. This may be changed:

``` yaml
TIMING:
    SYNTHETIC_KEYS_PAUSE_MILLISEC: 10
```

#### Changing the Behavior of `HOLD` Keys

Additionally, you can use `HOLD_START` to configure the behavior of `HOLD` keys. The examples [above](#functionality) will be used again here.

-   If `HOLD_START` is unspecified, `AFTER_PRESS` or an unrecognized value, `HOLD` keys are pressed after `KEY` is pressed, and released when `KEY` is released. This this the default behavior used in examples [above](#functionality).

-   If `HOLD_START` is `BEFORE_CONSUME`, `HOLD` keys are pressed before `KEY` is consumed, and released when `KEY` is released. Therefore no extra keys beside `TAP` keys are sent when `KEY` is tapped, while `HOLD` keys can still be used as modifiers.

``` text
                <---------200ms--------->     <---------200ms--------->
keyboard:       LS↓      LS↑                  LS↓                          LS↑
computer sees:           DE↓ DE↑
```

``` text
                                                               <-------150ms------->
                                                 <---------200ms--------->
                                 <-------150ms------->
                <---------200ms--------->
keyboard:       LS↓      a↓  a↑   LS↑             LS↓          LS↑           LS↓
computer sees:       LS↓ a↓  a↑   LS↑                          DE↓ DE↑       DE↓ ..(repeats)..
```

-   If `HOLD_START` is `BEFORE_CONSUME_OR_RELEASE`, the behavior is like `BEFORE_CONSUME` except that when `KEY` is released and is neither tapped nor consumed before, `HOLD` keys are pressed in order and then released in order.

``` text
                <---------200ms--------->     <---------200ms--------->
keyboard:       LS↓      LS↑                  LS↓                          LS↑
computer sees:           DE↓ DE↑                                           LS↓ LS↑
```

#### Warning

Do not assign the same modifier to two keys that you intend to press at the same time, as they will interfere with each other. Use left and right versions of the modifiers e.g. alt-tab with space-caps:

``` yaml
MAPPINGS:
    - KEY: KEY_CAPSLOCK
      TAP: KEY_TAB
      HOLD: KEY_LEFTALT

    - KEY: KEY_SPACE
      TAP: KEY_SPACE
      HOLD: KEY_RIGHTALT
```

Alternatively, you can use `HOLD_START: BEFORE_CONSUME` or `HOLD_START: BEFORE_CONSUME_OR_RELEASE` and then assigning the same modifier will be fine:

``` yaml
MAPPINGS:
    - KEY: KEY_CAPSLOCK
      TAP: KEY_TAB
      HOLD: KEY_LEFTALT
      HOLD_START: BEFORE_CONSUME_OR_RELEASE

    - KEY: KEY_SPACE
      TAP: KEY_SPACE
      HOLD: KEY_LEFTALT
      HOLD_START: BEFORE_CONSUME_OR_RELEASE
```

### udevmon

udevmon needs to be informed that we desire Dual Function Keys. See [How It Works](https://gitlab.com/interception/linux/tools#how-it-works) for the full story.

``` yaml
- JOB: "intercept -g $DEVNODE | dual-function-keys -c </path/to/dual-function-keys.yaml> | uinput -d $DEVNODE"
  DEVICE:
    NAME: <keyboard name>
```

The name may be determined by executing:

``` sh
sudo uinput -p -d /dev/input/by-id/X
```

where X is the device with the name that looks like your keyboard. Ensure that all `EV_KEY`s are present under `EVENTS`. If you can’t find your keyboard under `/dev/input/by-id`, look at devices directly under `/dev/input`.

See [Interception Tools: How It Works](https://gitlab.com/interception/linux/tools#how-it-works) for more information on `uinput -p`.

Usually the name is sufficient to uniquely identify the keyboard, however some keyboards register many devices such as a virtal mouse. You can run dual-function-keys for all the devices, however I prefer to run it only for the actual keyboard.

My `/etc/interception/udevmon.d/my-keyboards.yaml`:

``` yaml
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/dual-function-keys/home-row-modifiers.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "Minimalist Keyboard ABC"
    EVENTS:
      EV_KEY: [ KEY_LEFTSHIFT ]
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/dual-function-keys/thumb-cluster.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "Split Keyboard XYZ"
    EVENTS:
      EV_KEY: [ KEY_LEFTSHIFT ]
```

### Multiple Devices

When using inputs from multiple devices e.g. ctrl-scroll it may be necessary to [mux](https://gitlab.com/interception/linux/tools#mux) those devices for dual-function-keys to work across these devices e.g. scroll consuming ctrl.

Example udevmon configuration for a mouse and keyboard:

``` yaml
- CMD: mux -c dfk -c my-keyboard -c my-mouse
- JOB:
    - mux -i dfk | dual-function-keys -c /etc/interception/udevmon.d/dual-function-keys/my-cfg.yaml | mux -o my-keyboard -o my-mouse
    - mux -i my-keyboard | uinput -c /etc/interception/udevmon.d/my-keyboard.yaml
    - mux -i my-mouse | uinput -c /etc/interception/udevmon.d/my-mouse.yaml
- JOB: intercept -g $DEVNODE | mux -o dfk
  DEVICE:
    NAME: AT Translated Set 2 keyboard
    EVENTS:
      EV_KEY: [ KEY_LEFTCTRL ]
- JOB: intercept -g $DEVNODE | mux -o dfk
  DEVICE:
    NAME: Razer Razer Naga Trinity
    EVENTS:
      EV_REL: [REL_WHEEL]
      EV_KEY: [BTN_LEFT]
```

In the above example, `my-keyboard.yaml` and `my-mouse.yaml` represent the virtual devices that udevmon will create to output events. They are generated once from the device itself e.g.

    sudo uinput -p -d /dev/input/by-id/usb-my-keyboard-kbd > my-keyboard.yaml

An alternative, if you want to [live dangerously](https://gitlab.com/interception/linux/plugins/dual-function-keys/-/issues/31#note_725722450), is to generate the virtual device configuration on the fly e.g.:

``` yaml
- CMD: mux -c dfk -c my-keyboard -c my-mouse
- JOB:
    - mux -i dfk | dual-function-keys -c /etc/interception/dual-function-keys/my-cfg.yaml | mux -o my-keyboard -o my-mouse
    - mux -i my-keyboard | uinput -d /dev/input/by-path/my-keyboard-event-kbd
    - mux -i my-mouse | uinput -d /dev/input/by-id/usb-my-mouse-event-mouse
- JOB: intercept -g $DEVNODE | mux -o dfk
  DEVICE:
    LINK: /dev/input/by-path/my-keyboard-event-kbd
- JOB: intercept -g $DEVNODE | mux -o dfk
  DEVICE:
    LINK: /dev/input/by-id/usb-my-mouse-event-mouse
```

## CAVEATS

As always, there is a caveat: dual-function-keys operates on raw *keycodes*, not *keysyms*, as seen by X11 or Wayland.

If you have anything modifying the keycode->keysym mapping, such as [XKB](https://www.x.org/wiki/XKB/) or [xmodmap](https://wiki.archlinux.org/index.php/Xmodmap), be mindful that dual-function-keys operates before them.

Some common XKB usages that might be found in your X11 configuration:

``` text
    Option "XkbModel" "pc105"
    Option "XKbLayout" "us"
    Option "XkbVariant" "dvp"
    Option "XkbOptions" "caps:escape"
```

## FAQ

### I have a new use case. Can you support it?

Please raise an issue.

dual-function-keys has been built for my needs. I will be intrigued to hear your ideas and help you make them happen.

As usual, PRs are very welcome.

### I see you are using q.m.k HHKB mod Keyboard in your udevmon. It uses [QMK Firmware](https://qmk.fm/). Why not just use [Tap-Hold](https://docs.qmk.fm/#/tap_hold)?

Good catch! That does indeed provide the same functionality as dual-function-keys. Unfortunately there are some drawbacks:

1.  Few keyboards run QMK Firmware.
2.  There are some issues with that functionality, as noted in the documentation [Tap-Hold](https://docs.qmk.fm/).
3.  It requires a fast processor in the keyboard. My unscientific testing with an Ergodox (~800 scans/sec) and HHKB (~140) revealed that the slower keyboard is mushy and unuseably inaccurate.

### Why not use [xcape](https://github.com/alols/xcape)?

Xcape only provides simple tap/hold functionality. It appears difficult (impossible?) to add the remaining functionality using its XTestFakeKeyEvent mechanisms.

### My Key Combination Isn’t Working

Ensure that your window manager is not intercepting that key combination.

### I Don’t Want Double Tap Functionality

Set DOUBLE_TAP_MILLISEC to 0. See [Key Combinations, No Double Tap](https://gitlab.com/interception/linux/plugins/dual-function-keys/-/blob/master/doc/examples.md#key-combinations-no-double-tap).

## CONTRIBUTORS

Please fork this repo and submit a PR.

If you are making changes to the documentation, please edit the pandoc flavoured `dual-function-keys.md` and run `make doc`. Please ensure that this `README.md` and the man page `dual-function-keys.1` has your changes and commit all three.

As usual, please obey `.editorconfig`.

## LICENSE

<a href="https://gitlab.com/interception/linux/plugins/caps2esc/blob/master/LICENSE.md"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/0b/License_icon-mit-2.svg/120px-License_icon-mit-2.svg.png" alt="MIT"></a>

Copyright © 2020 Alexander Courtis
