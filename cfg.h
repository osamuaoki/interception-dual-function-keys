#include <sys/time.h>

#ifdef __cplusplus
extern "C" { //}
#endif

typedef enum { RELEASED, PRESSED, TAPPED, DOUBLETAPPED, CONSUMED, } State;

typedef struct {
    int from;
    int to;
    State state;
    struct timeval changed;
} Key;

Key *read_keys(int *nkeys);


typedef struct {
    int code;
    int tap;
    int hold;
    State state;
    struct timeval changed;
} Mapping;

typedef struct {
    int tap_millis;
    int double_tap_millis;
    Mapping *mappings;
    int nmappings;
} Cfg;

const Cfg *read_cfg();

#if __cplusplus
}
#endif

