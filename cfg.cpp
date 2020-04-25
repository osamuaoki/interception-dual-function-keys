#include "cfg.h"
#include <string>
#include <iostream>
#include <linux/input.h>
#include <libevdev/libevdev.h>
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

int event_code(const std::string code) {

    int ret = libevdev_event_code_from_name(EV_KEY, code.c_str());
    if (ret == -1)
        ret = strtol(code.c_str(), NULL, 10);

    // KEY_RESERVED is invalid
    if (ret == 0)
        throw std::invalid_argument("invalid key code: " + code);

    return ret;
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

        cfg.mappings = (Mapping*)calloc(0, sizeof(Mapping));
        const auto &keys = config["KEYS"];
        for (const auto &key : keys) {
            const auto &code = key["CODE"];
            const auto &tap = key["TAP"];
            const auto &hold = key["HOLD"];
            if (code && tap && hold) {
                cfg.mappings = (Mapping*)reallocarray(cfg.mappings, ++cfg.nmappings, sizeof(Mapping));

                cfg.mappings[cfg.nmappings - 1].code = event_code(code.as<std::string>());
                cfg.mappings[cfg.nmappings - 1].tap = event_code(tap.as<std::string>());
                cfg.mappings[cfg.nmappings - 1].hold = event_code(hold.as<std::string>());
            } else {
                std::stringstream out;
                out << key;
                throw std::invalid_argument("incomplete mapping:\n" + out.str());
            }
        }
    } catch (const YAML::Exception &e) {
        fprintf(stderr, "dfk: cannot parse %s: %s\n", cfgfile, e.what());
        exit(EXIT_FAILURE);
    } catch (const std::invalid_argument &e) {
        fprintf(stderr, "dfk: cannot grok %s: %s\n", cfgfile, e.what());
        exit(EXIT_FAILURE);
    }

    return NULL;
}

