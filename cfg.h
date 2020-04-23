#include <sys/time.h>

enum State { RELEASED, PRESSED, TAPPED, DOUBLETAPPED, CONSUMED, };

typedef struct {
    int from;
    int to;
    State state;
    struct timeval changed;
} Key;

Key *read_keys(int *nkeys);
