#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/time.h>

/*
 * https://www.kernel.org/doc/html/v4.12/input/event-codes.html
 */

#define DUR(start, end) ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)

typedef struct {
    int from;
    int to;
    int modified;
    struct timeval pressed;
    struct timeval released;
} Key;

Key keys[] = {
    { .from = KEY_LEFTSHIFT, .to = KEY_BACKSPACE, },
    { .from = KEY_RIGHTSHIFT, .to = KEY_SPACE, },
    { .from = KEY_LEFTCTRL, .to = KEY_TAB, },
    { .from = KEY_RIGHTCTRL, .to = KEY_DELETE, },
    { .from = KEY_LEFTMETA, .to = KEY_ESC, },
    { .from = KEY_RIGHTMETA, .to = KEY_ENTER, },
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
            key->modified = key->modified && DUR(key->released, input.time) < 200000;
            if (key->modified)
                input.code = key->to;

        } else if (input.value == 0) {
            key->released = input.time;

            if (key->modified) {
                input.code = key->to;
            } else if (DUR(key->pressed, input.time) < 200000) {
                key->modified = 1;

                // release first
                fwrite(&input, sizeof(input), 1, stdout);

                // synthesise a press/release
                struct input_event synthetic;
                synthetic.type = EV_KEY;
                synthetic.value = 1;
                synthetic.code = key->to;
                fwrite(&synthetic, sizeof(input), 1, stdout);
                synthetic.value = 0;
                fwrite(&synthetic, sizeof(input), 1, stdout);

                // todo: clean this up as a return early
                continue;
            }
        }

        fwrite(&input, sizeof(input), 1, stdout);
    }
}

// vim: autowrite et ts=4 sw=4
