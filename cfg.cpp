#include "cfg.h"
#include <string>
#include <sstream>
#include <libevdev/libevdev.h>
#include <yaml-cpp/yaml.h>

namespace { // }

using std::exception;
using std::invalid_argument;
using std::string;
using std::stringstream;

int
event_code(const string code) {

    int ret = libevdev_event_code_from_name(EV_KEY, code.c_str());
    if (ret == -1)
        ret = strtol(code.c_str(), NULL, 10);

    // KEY_RESERVED is invalid
    if (ret == 0)
        throw invalid_argument("invalid key code: " + code);

    return ret;
}

void
add_mapping(Cfg *cfg, const string key, Tap *tap, Hold *hold, HoldStart hold_start) {
    Mapping *m, *p;

    m = (Mapping*)calloc(1, sizeof(Mapping));
    if (!cfg->m) {
        cfg->m = m;
    } else {
        for (p = cfg->m; p->n; p = p->n);
        p->n = m;
    }

    m->key = event_code(key);
    m->tap = tap;
    m->hold = hold;
    m->hold_start = hold_start;
}

Tap*
build_tap(const YAML::Node &node) {
    Tap *tap;
    Tap *t, *p = 0;
    size_t i;

    if (node.IsSequence()) {
        tap = (Tap*)calloc(node.size(), sizeof(Mapping));
        for (i = 0; i < node.size(); i++) {
            t = &tap[i];
            t->code = event_code(node[i].as<string>());
            if (p)
                p->n = t;
            p = t;
        }
    } else {
        tap = (Tap*)calloc(node.size(), sizeof(Mapping));
        tap->code = event_code(node.as<string>());
    }

    return tap;
}
Hold*
build_hold(const YAML::Node &node) {
    Hold *hold;
    Hold *h, *p = 0;
    size_t i;

    if (node.IsSequence()) {
        hold = (Hold*)calloc(node.size(), sizeof(Mapping));
        for (i = 0; i < node.size(); i++) {
            h = &hold[i];
            h->code = event_code(node[i].as<string>());
            if (p)
                p->n = h;
            p = h;
        }
    } else {
        hold = (Hold*)calloc(node.size(), sizeof(Mapping));
        hold->code = event_code(node.as<string>());
    }

    return hold;
}
HoldStart
parse_hold_start(const YAML::Node &node) {
    if (!node)
        return AFTER_PRESS;
    string s = node.as<string>();
    if (s == "BEFORE_CONSUME")
        return BEFORE_CONSUME;
    if (s == "BEFORE_CONSUME_OR_RELEASE")
        return BEFORE_CONSUME_OR_RELEASE;
    return AFTER_PRESS;
}

} // namespace

void
read_cfg(Cfg *cfg, const char *path) {
    YAML::Node config;

    try {
        config = YAML::LoadFile(path);
    } catch (const exception &e) {
        fprintf(stderr, "dual-function-keys: cannot read '%s': %s\n", path, e.what());
        exit(EXIT_FAILURE);
    }

    try {
        const auto &timing = config["TIMING"];
        if (timing["TAP_MILLISEC"])
            cfg->tap_millis = timing["TAP_MILLISEC"].as<int>();
        else
            cfg->tap_millis = DEFAULT_TAP_MILLIS;
        if (timing["DOUBLE_TAP_MILLISEC"])
            cfg->double_tap_millis = timing["DOUBLE_TAP_MILLISEC"].as<int>();
        else
            cfg->double_tap_millis = DEFAULT_DOUBLE_TAP_MILLIS;
        if (timing["SYNTHETIC_KEYS_PAUSE_MILLISEC"])
            cfg->synthetic_keys_pause_millis = timing["SYNTHETIC_KEYS_PAUSE_MILLISEC"].as<int>();
        else
            cfg->synthetic_keys_pause_millis = DEFAULT_SYNTHETIC_KEYS_PAUSE_MILLIS;

        const auto &mappings = config["MAPPINGS"];
        for (const auto &mapping : mappings) {
            if (mapping["KEY"] && mapping["TAP"] && mapping["HOLD"]) {
                 add_mapping(cfg,
                        mapping["KEY"].as<string>(),
                        build_tap(mapping["TAP"]),
                        build_hold(mapping["HOLD"]),
                        parse_hold_start(mapping["HOLD_START"]));

            } else {
                stringstream out;
                out << mapping;
                throw invalid_argument("incomplete mapping:\n" + out.str());
            }
        }
    } catch (const exception &e) {
        fprintf(stderr, "dual-function-keys: cannot parse '%s': %s\n", path, e.what());
        exit(EXIT_FAILURE);
    }
}

