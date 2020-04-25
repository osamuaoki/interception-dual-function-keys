#include "cfg.h"
#include <string>
#include <iostream>
#include <linux/input.h>
#include <libevdev/libevdev.h>
#include <yaml-cpp/yaml.h>

namespace { // }

using std::exception;
using std::invalid_argument;
using std::string;
using std::stringstream;

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

int event_code(const string code) {

    int ret = libevdev_event_code_from_name(EV_KEY, code.c_str());
    if (ret == -1)
        ret = strtol(code.c_str(), NULL, 10);

    // KEY_RESERVED is invalid
    if (ret == 0)
        throw invalid_argument("invalid key code: " + code);

    return ret;
}

void add_mapping(const string key, const string tap, const string hold) {
    cfg.mappings =
        (Mapping*)reallocarray(cfg.mappings, ++cfg.nmappings, sizeof(Mapping));

    cfg.mappings[cfg.nmappings - 1].key = event_code(key);
    cfg.mappings[cfg.nmappings - 1].tap = event_code(tap);
    cfg.mappings[cfg.nmappings - 1].hold = event_code(hold);
}

} // unnamed namespace

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
        if (timing["TAP_MILLISEC"])
            cfg.tap_millis = timing["TAP_MILLISEC"].as<int>();
        if (timing["DOUBLE_TAP_MILLISEC"])
            cfg.double_tap_millis = timing["DOUBLE_TAP_MILLISEC"].as<int>();

        const auto &mappings = config["MAPPINGS"];
        for (const auto &mapping : mappings) {
            if (mapping["KEY"] && mapping["TAP"] && mapping["HOLD"]) {
                add_mapping(mapping["KEY"].as<string>(),
                        mapping["TAP"].as<string>(),
                        mapping["HOLD"].as<string>());
            } else {
                stringstream out;
                out << mapping;
                throw invalid_argument("incomplete mapping:\n" + out.str());
            }
        }
    } catch (const exception &e) {
        fprintf(stderr, "dfk: cannot parse %s: %s\n", cfgfile, e.what());
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < cfg.nmappings; i++) {
        fprintf(stderr, "key = %d\n", cfg.mappings[i].key);
        fprintf(stderr, "tap = %d\n", cfg.mappings[i].tap);
        fprintf(stderr, "hold = %d\n", cfg.mappings[i].hold);
    }

    return &cfg;
}

