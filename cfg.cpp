#include "cfg.h"
#include <linux/input.h>
#include <yaml-cpp/yaml.h>

static Key keyss[] = {
    { .from = KEY_LEFTSHIFT,    .to = KEY_BACKSPACE,    .state = RELEASED,  .changed = { 0, 0, }    ,},
    { .from = KEY_RIGHTSHIFT,   .to = KEY_SPACE,        .state = RELEASED,  .changed = { 0, 0, }    ,},
    { .from = KEY_LEFTCTRL,     .to = KEY_TAB,          .state = RELEASED,  .changed = { 0, 0, }    ,},
    { .from = KEY_RIGHTCTRL,    .to = KEY_DELETE,       .state = RELEASED,  .changed = { 0, 0, }    ,},
    { .from = KEY_LEFTMETA,     .to = KEY_ESC,          .state = RELEASED,  .changed = { 0, 0, }    ,},
    { .from = KEY_RIGHTMETA,    .to = KEY_ENTER,        .state = RELEASED,  .changed = { 0, 0, }    ,},
};

Key *read_keys(int *nkeys) {

    const char *cfgfile = "dfk.yaml";

    YAML::Node config;
    try {
        config = YAML::LoadFile(cfgfile);
    } catch (YAML::Exception &e) {
        fprintf(stderr, "dfk: cannot read %s: %s\n", cfgfile, e.what());
        exit(EXIT_FAILURE);
    }

    *nkeys = 6;
    return keyss;
}

