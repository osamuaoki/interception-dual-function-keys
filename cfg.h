#include <sys/time.h>
#include <stddef.h>

#define CFG_PATH "/etc/interception.dfk.yaml"
//#define CFG_PATH "/home/alex/src/interception/dfk/examples/alex.yaml"
//#define CFG_PATH "/home/alex/src/interception/dfk/examples/home.yaml"

#define DEFAULT_TAP_MILLIS 200
#define DEFAULT_DOUBLE_TAP_MILLIS 150

#ifdef __cplusplus
extern "C" { //}
#endif

typedef enum State {
    RELEASED,
    PRESSED,
    TAPPED,
    DOUBLETAPPED,
    CONSUMED,
} State;

typedef struct Mapping {
    int key;
    int tap;
    int hold;
    State state;
    struct timeval changed;
    struct Mapping *n;
} Mapping;

typedef struct Cfg {
    int tap_millis;
    int double_tap_millis;
    Mapping *m;
} Cfg;

void read_cfg(Cfg *cfg);

#if __cplusplus
} // extern "C"
#endif

