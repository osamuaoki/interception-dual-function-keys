#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/time.h>

#define DUR_MICRO_SEC(start, end) ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)

#define TAP_MICRO_SEC 150000
#define DOUBLE_TAP_MICRO_SEC 150000

/* https://www.kernel.org/doc/html/latest/input/event-codes.html */
#define INPUT_VAL_PRESS 1
#define INPUT_VAL_RELEASE 0
#define INPUT_VAL_REPEAT 2

typedef struct {
    int from;
    int to;
    enum { RELEASED, PRESSED, TAPPED, DOUBLETAPPED, CONSUMED, } state;
    struct timeval changed;
} Key;

Key keys[] = {
    { .from = KEY_LEFTSHIFT,    .to = KEY_BACKSPACE,    },
    { .from = KEY_RIGHTSHIFT,   .to = KEY_SPACE,        },
    { .from = KEY_LEFTCTRL,     .to = KEY_TAB,          },
    { .from = KEY_RIGHTCTRL,    .to = KEY_DELETE,       },
    { .from = KEY_LEFTMETA,     .to = KEY_ESC,          },
    { .from = KEY_RIGHTMETA,    .to = KEY_ENTER,        },
};
int nkeys = 6;

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

void handle_press(Key *key, struct input_event *input) {

    // state
    switch (key->state) {
        case TAPPED:
        case DOUBLETAPPED:
            if (DUR_MICRO_SEC(key->changed, input->time) < DOUBLE_TAP_MICRO_SEC)
                key->state = DOUBLETAPPED;
            else
                key->state = PRESSED;
            break;
        case RELEASED:
        case CONSUMED:
            key->state = PRESSED;
            break;
        case PRESSED:
            break;
    }
    key->changed = input->time;

    // action
    switch (key->state) {
        case TAPPED:
        case DOUBLETAPPED:
            input->code = key->to;
            break;
        case RELEASED:
        case PRESSED:
        case CONSUMED:
            break;
    }

    write_event(input);
}

void handle_release(Key *key, struct input_event *input) {

    // state
    switch (key->state) {
        case PRESSED:
            if (DUR_MICRO_SEC(key->changed, input->time) < TAP_MICRO_SEC)
                key->state = TAPPED;
            else
                key->state = RELEASED;
            break;
        case TAPPED:
        case DOUBLETAPPED:
            break;
        case CONSUMED:
            key->state = RELEASED;
            break;
        case RELEASED:
            break;
    }
    key->changed = input->time;

    // action
    switch (key->state) {
        case TAPPED:
            // release "from"
            write_event(input);

            // synthesize press/release "to"
            input->value = 1; 
            input->code = key->to;
            write_event(input);
            input->value = 0;
            write_event(input);
            break;
        case DOUBLETAPPED:
            input->code = key->to;
            write_event(input);
            break;
        case CONSUMED:
        case RELEASED:
        case PRESSED:
            write_event(input);
            break;
    }
}

void consume_pressed() {

    // state
    for (int i = 0; i < nkeys; i++) {
        switch (keys[i].state) {
            case PRESSED:
                keys[i].state = CONSUMED;
                break;
            case TAPPED:
            case DOUBLETAPPED:
            case RELEASED:
            case CONSUMED:
                break;
        }
    }
}

int main(void) {
    struct input_event input;
    Key *key;

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        // uinput doesn't need sync events
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        // forward anything that is not a key event
        if (input.type != EV_KEY) {
            write_event(&input);
            continue;
        }

        // uinput doesn't need hardware repeats; squish them all
        if (input.value == INPUT_VAL_REPEAT)
            continue;

        // consume all taps that are incomplete
        if (input.value == INPUT_VAL_PRESS)
            consume_pressed();

        // is this our key?
        key = NULL;
        for (int i = 0; i < nkeys; i++)
            if (keys[i].from == input.code)
                key = &keys[i];

        // forward all other key events
        if (!key) {
            write_event(&input);
            continue;
        }

        switch (input.value) {
            case INPUT_VAL_PRESS:
                handle_press(key, &input);
                break;
            case INPUT_VAL_RELEASE:
                handle_release(key, &input);
                break;
        }
    }
}

// vim: autowrite et ts=4 sw=4
