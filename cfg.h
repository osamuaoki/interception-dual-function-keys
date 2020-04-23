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

#if __cplusplus
}
#endif
