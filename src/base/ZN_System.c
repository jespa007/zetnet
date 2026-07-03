#include "zetnet.h"

void ZN_System_SleepMicroSeconds(unsigned long microseconds)
{
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000ul;            // whole seconds
    ts.tv_nsec = (microseconds % 1000000ul) * 1000;  // remainder, in nanoseconds
    nanosleep(&ts, NULL);
}
