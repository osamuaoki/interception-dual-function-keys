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
add_mapping(Cfg *cfg, const string key, const string tap, const string hold) {
    Mapping *m, *p;

    m = (Mapping*)calloc(1, sizeof(Mapping));
    if (!cfg->m) {
        cfg->m = m;
    } else {
        for (p = cfg->m; p->n; p = p->n);
        p->n = m;
    }

    m->key = event_code(key);
    m->tap = event_code(tap);
    m->hold = event_code(hold);
}

} // namespace

void
read_cfg(Cfg *cfg, const char *path) {
    YAML::Node config;

    try {
        config = YAML::LoadFile(path);
    } catch (const exception &e) {
        fprintf(stderr, "dfk: cannot read '%s': %s\n", path, e.what());
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

        const auto &mappings = config["MAPPINGS"];
        for (const auto &mapping : mappings) {
            if (mapping["KEY"] && mapping["TAP"] && mapping["HOLD"]) {
                add_mapping(cfg,
                        mapping["KEY"].as<string>(),
                        mapping["TAP"].as<string>(),
                        mapping["HOLD"].as<string>());
            } else {
                stringstream out;
                out << mapping;
                throw invalid_argument("incomplete mapping:\n" + out.str());
            }
        }
    } catch (const exception &e) {
        fprintf(stderr, "dfk: cannot parse '%s': %s\n", path, e.what());
        exit(EXIT_FAILURE);
    }
}

