#include <sys/time.h>
#include <stddef.h>

#define CFG_PATH "/etc/interception.dfk.yaml"
//#define CFG_PATH "/home/alex/src/interception/dfk/dfk.yaml"

#define DEFAULT_TAP_MILLIS 200
#define DEFAULT_DOUBLE_TAP_MILLIS 150

#ifdef __cplusplus
extern "C" { //}
#endif

typedef enum { RELEASED, PRESSED, TAPPED, DOUBLETAPPED, CONSUMED, } State;

typedef struct {
    int key;
    int tap;
    int hold;
    State state;
    struct timeval changed;
} Mapping;

typedef struct {
    int tap_millis;
    int double_tap_millis;
    Mapping *m;
    size_t nm;
} Cfg;

void
read_cfg(Cfg *cfg);

#if __cplusplus
} // extern "C"
#endif

