#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/time.h>

#define DUR_MICRO_SEC(start, end) ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)

#define TAP_MICRO_SEC 200000
#define DOUBLE_TAP_MICRO_SEC 100000

/* https://www.kernel.org/doc/html/latest/input/event-codes.html */
#define INPUT_VAL_PRESS 1
#define INPUT_VAL_RELEASE 0
#define INPUT_VAL_REPEAT 2

typedef struct {
    int from;
    int to;
    char *label;
    enum { RELEASED, PRESSED, TAPPED, CONSUMED, } state;
    struct timeval changed;
} Key;

Key keys[] = {
    { .from = KEY_LEFTSHIFT, .to = KEY_BACKSPACE, .label = "ls_bs" },
    { .from = KEY_RIGHTSHIFT, .to = KEY_SPACE, .label = "rs_sp"},
    { .from = KEY_LEFTCTRL, .to = KEY_TAB, .label = "lc_ta"},
    { .from = KEY_RIGHTCTRL, .to = KEY_DELETE, .label = "rc_de"},
    { .from = KEY_LEFTMETA, .to = KEY_ESC, .label = "lm_es"},
    { .from = KEY_RIGHTMETA, .to = KEY_ENTER, .label = "rm_en"},
};
int nkeys = 6;

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

void print_state(const Key *key) {
    switch (key->state) {
        case RELEASED:
            fprintf(stderr, "RELEASED");
            break;
        case PRESSED:
            fprintf(stderr, "PRESSED");
            break;
        case TAPPED:
            fprintf(stderr, "TAPPED");
            break;
        case CONSUMED:
            fprintf(stderr, "CONSUMED");
            break;
    }
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

        key = NULL;
        for (int i = 0; i < nkeys; i++)
            if (keys[i].from == input.code)
                key = &keys[i];

        if (input.value == INPUT_VAL_PRESS) {

            // consume
            for (int i = 0; i < nkeys; i++) {
                if (keys[i].state == PRESSED) {
                    keys[i].state = CONSUMED;
                    fprintf(stderr, "%d CONSUMING %s ", input.code, keys[i].label);
                    if (key == &keys[i])
                        fprintf(stderr, "consumed itself... impossible! ");
                }
            }

            fprintf(stderr, "\n");
        }

        if (!key) {
            write_event(&input);
            continue;
        }

        if (input.value == INPUT_VAL_PRESS) {

            fprintf(stderr, "%s press:   ", key->label);
            print_state(key);
            fprintf(stderr, " -> ");

            switch (key->state) {
                case TAPPED:
                    if (DUR_MICRO_SEC(key->changed, input.time) < DOUBLE_TAP_MICRO_SEC) {
                        key->state = TAPPED;
                        input.code = key->to;
                    } else {
                        key->state = PRESSED;
                    }
                    break;
                case RELEASED:
                case CONSUMED:
                    key->state = PRESSED;
                    break;
                case PRESSED:
                default:
                    break;
            }

            print_state(key);
            fprintf(stderr, "\n");

            key->changed = input.time;

        } else if (input.value == INPUT_VAL_REPEAT) {

            // not sure if this is necessary
            continue;

        } else if (input.value == INPUT_VAL_RELEASE) {

            fprintf(stderr, "%s release: ", key->label);
            print_state(key);
            fprintf(stderr, " -> ");

            switch (key->state) {
                case PRESSED:
                    if (DUR_MICRO_SEC(key->changed, input.time) < TAP_MICRO_SEC) {
                        key->state = TAPPED;

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
                        print_state(key);
                        fprintf(stderr, "\n");
                        key->changed = input.time;
                        continue;
                    } else {
                        key->state = RELEASED;
                    }
                    break;
                case TAPPED:
                    input.code = key->to;
                    break;
                case CONSUMED:
                    key->state = RELEASED;
                    break;
                case RELEASED:
                default:
                    break;
            }

            print_state(key);
            fprintf(stderr, "\n");

            key->changed = input.time;
        }

        fwrite(&input, sizeof(input), 1, stdout);
    }
}

// vim: autowrite et ts=4 sw=4
