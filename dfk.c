#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/time.h>

/*
 * https://www.kernel.org/doc/html/v4.12/input/event-codes.html
 */

#define DUR(start, end) ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))

typedef struct {
    struct timeval pressed;
    struct timeval released;
    int doubletapped;
    int from;
    int to;
} Key;

Key keys[] = {
    { .from = KEY_LEFTSHIFT, .to = KEY_BACKSPACE, .pressed = { 0, 0, }, .released = { 0, 0, }, .doubletapped = 0, },
    { .from = KEY_RIGHTSHIFT, .to = KEY_SPACE, .pressed = { 0, 0, }, .released = { 0, 0, }, .doubletapped = 0, },
    { .from = KEY_LEFTCTRL, .to = KEY_TAB, .pressed = { 0, 0, }, .released = { 0, 0, }, .doubletapped = 0, },
    { .from = KEY_RIGHTCTRL, .to = KEY_DELETE, .pressed = { 0, 0, }, .released = { 0, 0, }, .doubletapped = 0, },
    { .from = KEY_LEFTMETA, .to = KEY_ESC, .pressed = { 0, 0, }, .released = { 0, 0, }, .doubletapped = 0, },
    { .from = KEY_RIGHTMETA, .to = KEY_ENTER, .pressed = { 0, 0, }, .released = { 0, 0, }, .doubletapped = 0, },
};
int nkeys = 6;

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

int main(void) {
    struct input_event input;
    Key *key;

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        if (input.type != EV_KEY) {
            write_event(&input);
            continue;
        }

        // squish all repeats; downstream seems to process key repeats just fine
        if (input.value == 2) {
            continue;
        }

        key = NULL;
        for (int i = 0; i < nkeys; i++) {
            if (keys[i].from == input.code)
                key = &keys[i];
        }

        if (!key) {
            write_event(&input);
            continue;
        }

        if (input.value == 1) {
            key->pressed = input.time;

            if (DUR(key->released, input.time) < 200000) {

                // modify the press
                key->doubletapped = 1;
                input.code = key->to;
            }

        } else if (input.value == 0) {
            key->released = input.time;

            if (key->doubletapped) {

                // modify the release
                input.code = key->to;
                key->doubletapped = 0;
            } else if (DUR(key->pressed, input.time) < 200000) {

                // release the modifier first
                fwrite(&input, sizeof(input), 1, stdout);

                // synthesise a press/release
                struct input_event synthetic;
                synthetic.type = EV_KEY;
                synthetic.value = 1;
                synthetic.code = key->to;
                fwrite(&synthetic, sizeof(input), 1, stdout);
                synthetic.value = 0;
                fwrite(&synthetic, sizeof(input), 1, stdout);

                continue;
            }
        }

        // forward the maybe modified event
        fwrite(&input, sizeof(input), 1, stdout);
    }
}

// vim: autowrite et ts=4 sw=4
