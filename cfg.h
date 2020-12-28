#include <sys/time.h>
#include <stddef.h>

#define DEFAULT_TAP_MILLIS 200
#define DEFAULT_DOUBLE_TAP_MILLIS 150
#define DEFAULT_SYNTHETIC_KEYS_PAUSE_MILLIS 20

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

typedef struct Tap {
    int code;
    struct Tap *n;
} Tap;

typedef struct Hold {
    int code;
    struct Hold *n;
} Hold;

typedef struct Mapping {
    int key;
    Hold *hold;
    Tap *tap;
    State state;
    struct timeval changed;
    struct Mapping *n;
} Mapping;

typedef struct Cfg {
    int tap_millis;
    int double_tap_millis;
    int synthetic_keys_pause_millis;
    Mapping *m;
} Cfg;

void read_cfg(Cfg *cfg, const char *path);

#if __cplusplus
} // extern "C"
#endif

