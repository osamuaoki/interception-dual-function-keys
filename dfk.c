#include "cfg.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/input.h>

#define DUR_MILLIS(start, end) \
    ((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000)

/* https://www.kernel.org/doc/html/latest/input/event-codes.html */
#define INPUT_VAL_PRESS 1
#define INPUT_VAL_RELEASE 0
#define INPUT_VAL_REPEAT 2

static Cfg cfg;

int
read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void
write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

void
syn_pause() {
    static const struct input_event syn = {
        .type = EV_SYN,
        .code = SYN_REPORT,
        .value = 0,
    };
    static struct timespec p = { .tv_sec = 0, .tv_nsec = 0 };
    if (!p.tv_nsec)
            p.tv_nsec = cfg.synthetic_keys_pause_millis * 1e6;

    write_event(&syn);
    nanosleep(&p, &p);
}

void
tap(Mapping *m, unsigned int value) {
    static struct input_event input = { .type = EV_KEY, };
    Tap *t;

    input.value = value;
    for (t = m->tap; t; t = t->n) {
        input.code = t->code;
        write_event(&input);
        if (t->n)
            syn_pause();
    }
}

void
hold(Mapping *m, unsigned int value) {
    static struct input_event input = { .type = EV_KEY, };
    Hold *h;

    input.value = value;
    for (h = m->hold; h; h = h->n) {
        input.code = h->code;
        write_event(&input);
        if (h->n)
            syn_pause();
    }
}

void
handle_press(Mapping *m, struct input_event *input) {

    // state
    switch (m->state) {
        case TAPPED:
        case DOUBLETAPPED:
            if (DUR_MILLIS(m->changed, input->time) < cfg.double_tap_millis)
                m->state = DOUBLETAPPED;
            else
                m->state = PRESSED;
            break;
        case RELEASED:
        case CONSUMED:
            m->state = PRESSED;
            break;
        case PRESSED:
            break;
    }
    m->changed = input->time;

    // action
    switch (m->state) {
        case TAPPED:
        case DOUBLETAPPED:
            tap(m, INPUT_VAL_PRESS);
            break;
        case RELEASED:
        case PRESSED:
        case CONSUMED:
            hold(m, INPUT_VAL_PRESS);
            break;
    }
}

void
handle_release(Mapping *m, struct input_event *input) {

    // state
    switch (m->state) {
        case PRESSED:
            if (DUR_MILLIS(m->changed, input->time) < cfg.tap_millis)
                m->state = TAPPED;
            else
                m->state = RELEASED;
            break;
        case TAPPED:
        case DOUBLETAPPED:
            break;
        case CONSUMED:
            m->state = RELEASED;
            break;
        case RELEASED:
            break;
    }
    m->changed = input->time;

    // action
    switch (m->state) {
        case TAPPED:
            // release
            hold(m, INPUT_VAL_RELEASE);
            syn_pause();

            // synthesize tap
            tap(m, INPUT_VAL_PRESS);
            syn_pause();
            tap(m, INPUT_VAL_RELEASE);
            break;
        case DOUBLETAPPED:
            tap(m, INPUT_VAL_RELEASE);
            break;
        case CONSUMED:
        case RELEASED:
        case PRESSED:
            hold(m, INPUT_VAL_RELEASE);
            break;
    }
}

void
consume_pressed() {

    // state
    for (Mapping *m = cfg.m; m; m = m->n) {
        switch (m->state) {
            case PRESSED:
                m->state = CONSUMED;
                break;
            case TAPPED:
            case DOUBLETAPPED:
            case RELEASED:
            case CONSUMED:
                break;
        }
    }
}

void
loop() {
    struct input_event input;
    Mapping *m;

    while (read_event(&input)) {
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        // forward anything that is not a key event, including SYNs
        if (input.type != EV_KEY) {
            write_event(&input);
            continue;
        }

        // consume all taps that are incomplete
        if (input.value == INPUT_VAL_PRESS)
            consume_pressed();

        // is this our key?
        for (m = cfg.m; m && m->key != input.code; m = m->n);

        // forward all other key events
        if (!m) {
            write_event(&input);
            continue;
        }

        switch (input.value) {
            case INPUT_VAL_PRESS:
                handle_press(m, &input);
                break;
            case INPUT_VAL_RELEASE:
                handle_release(m, &input);
                break;
            case INPUT_VAL_REPEAT:
                // linux console, X, wayland handles repeat
                break;
            default:
                fprintf(stderr, "unexpected .value=%d .code=%d, doing nothing",
                        input.value,
                        input.code);
                break;
        }
    }
}

void
print_usage(FILE *stream, const char *program) {
    fprintf(stream,
            "dual-function-keys plugin for interception tools:\n"
            "        https://gitlab.com/interception/linux/tools\n"
            "\n"
            "usage: %s [-v] [-h] -c /path/to/cfg.yaml\n"
            "\n"
            "options:\n"
            "    -v                     show version and exit\n"
            "    -h                     show this message and exit\n"
            "    -c /path/to/cfg.yaml   use cfg.yaml\n",
            program);
}

int
main(int argc, char *argv[]) {
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    int configured = 0;
    int opt;
    while ((opt = getopt(argc, argv, "vhc:")) != -1) {
        switch (opt) {
            case 'v':
                return fprintf(stdout, "dual-function-keys version %s\n", VERSION), EXIT_SUCCESS;
            case 'h':
                return print_usage(stdout, argv[0]), EXIT_SUCCESS;
            case 'c':
                if (configured)
                    break;
                read_cfg(&cfg, optarg);
                configured = 1;
                continue;
        }
    }

    if (!configured)
        return print_usage(stderr, argv[0]), EXIT_FAILURE;

    loop();
}

