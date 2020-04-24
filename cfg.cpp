#include "cfg.h"
#include <string>
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

static Cfg cfg = {
    .tap_millis = 999,
    .double_tap_millis = 999,
    .mappings = NULL,
    .nmappings = 0,
};

Key *read_keys(int *nkeys) {
    *nkeys = 6;
    return keyss;
}

const Cfg *read_cfg() {

    const char *cfgfile = "dfk.yaml";

    YAML::Node config;
    try {
        config = YAML::LoadFile(cfgfile);
    } catch (YAML::Exception &e) {
        fprintf(stderr, "dfk: cannot read %s: %s\n", cfgfile, e.what());
        exit(EXIT_FAILURE);
    }

    try {
        const auto &timing = config["TIMING"];
        const auto &tap = timing["TAP_MILLISEC"];
        const auto &double_tap = timing["DOUBLE_TAP_MILLISEC"];
        if (tap)
            cfg.tap_millis = tap.as<int>();
        if (double_tap)
            cfg.double_tap_millis = double_tap.as<int>();

        const auto &keys = config["KEYS"];
        for (const auto &key : keys) {
            const auto &code = key["CODE"];
            const auto &tap = key["TAP"];
            const auto &hold = key["HOLD"];
            if (code && tap && hold) {
                fprintf(stderr, "CODE %s\n", code.as<std::string>().c_str());
                fprintf(stderr, "TAP %s\n", tap.as<std::string>().c_str());
                fprintf(stderr, "HOLD %s\n", hold.as<std::string>().c_str());
            }
        }
    } catch (YAML::Exception &e) {
        fprintf(stderr, "dfk: cannot parse %s: %s\n", cfgfile, e.what());
        exit(EXIT_FAILURE);
    }

    return NULL;
}

