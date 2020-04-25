#include <sys/time.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" { //}
#endif

typedef enum State { RELEASED, PRESSED, TAPPED, DOUBLETAPPED, CONSUMED, } State;

typedef struct Key {
    int from;
    int to;
    State state;
    struct timeval changed;
} Key;

Key *read_keys(int *nkeys);


typedef struct Mapping {
    int code;
    int tap;
    int hold;
    State state;
    struct timeval changed;
} Mapping;

typedef struct Cfg {
    int tap_millis;
    int double_tap_millis;
    Mapping *mappings;
    size_t nmappings;
} Cfg;

const Cfg *read_cfg();

#if __cplusplus
}
#endif

