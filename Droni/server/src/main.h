
#ifndef MAIN_H
#define MAIN_H

#define DEBUG 1000
#define HORIZON 100
#include <cstring>
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
#include <vector>
#include <queue>
#include <ctime>
#include <memory>
#include <cmath> // Per usare la funzione sqrt
#include <tuple>
#include <iostream>
#include <random>

#include "controlcenter.h"
#include "timeprogram.h"


void MonitorPosition(Con2DB db);
void MonitorSectorAssignment(Con2DB db);
void MonitorBatteryConsumption(Con2DB db);
void MonitorRechargeTime(Con2DB db);
void MonitorLogIntegrity(Con2DB db);

using namespace std;
#endif
