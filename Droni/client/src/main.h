#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stddef.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/times.h>
#include <gsl/gsl_fit.h>
#include <cassert>
#include <cerrno>
#include "../../../con2redis/src/con2redis.h"
#include "drone.h"

#define DEBUG 1000

int client_micro_sleep(long usec);



#endif // MAIN_H
