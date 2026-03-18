#ifndef EDF_H
#define EDF_H

#include <fstream>
#include "datatypes.h"

void EDFScheduler(std::ofstream &output_file, int num_tasks, int sim_time, Task* tasks);

#endif