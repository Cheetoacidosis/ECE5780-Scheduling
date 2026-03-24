#ifndef LLF_h
#define LLF_H

#include <fstream>
#include "datatypes.h"

void LLFScheduler(std::ofstream &output_file, int num_tasks, int sim_time, Task* tasks);

#endif