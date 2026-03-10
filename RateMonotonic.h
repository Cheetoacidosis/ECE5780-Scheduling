#ifndef _RATEMONOTONIC_H
#define _RATEMONOTONIC_H

#include <iostream>
#include <fstream>
#include "datatypes.h"
using namespace std;

void testfunc();

void RMScheduler(ofstream &output_file, int num_tasks, int sim_time, Task* periodic_tasks);

#endif