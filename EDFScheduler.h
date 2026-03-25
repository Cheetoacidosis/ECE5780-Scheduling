#ifndef EDF_H
#define EDF_H

#include <fstream>
#include "datatypes.h"

void EDFScheduler(ofstream &output_file,
                  int num_tasks,
                  int num_aperiodic,
                  int sim_time,
                  Task* periodic_tasks,
                  Task* aperiodic_tasks);

#endif