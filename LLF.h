#ifndef LLF_h
#define LLF_H

#include <fstream>
#include "datatypes.h"

void LLFScheduler(ofstream &output_file, int num_tasks, int num_tasks_aperiodic, int sim_time, Task* periodic_tasks, Task* aperiodic_tasks);

#endif