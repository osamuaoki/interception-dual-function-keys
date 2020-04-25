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

void add_mapping(const std::string key, const std::string tap, const std::string hold) {
    cfg.mappings = (Mapping*)reallocarray(cfg.mappings, ++cfg.nmappings, sizeof(Mapping));

    cfg.mappings[cfg.nmappings - 1].key = event_code(key);
    cfg.mappings[cfg.nmappings - 1].tap = event_code(tap);
    cfg.mappings[cfg.nmappings - 1].hold = event_code(hold);
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
                add_mapping(mapping["KEY"].as<std::string>(),
                        mapping["TAP"].as<std::string>(),
                        mapping["HOLD"].as<std::string>());
            } else {
                std::stringstream out;
                out << mapping;
                throw std::invalid_argument("incomplete mapping\n" + out.str());
            }
        }
    } catch (const std::exception &e) {
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

