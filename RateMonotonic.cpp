#include "RateMonotonic.h"

void testfunc(){

    printf("Hey I'm a linked function!\n");

    return;
}

void RMScheduler(ofstream &output_file, int num_tasks, int sim_time, Task* tasks){
    //Debug. Make sure everything made it here okay
    // for (int i = 0; i<num_tasks ; i++){
    //     output_file << tasks[i].ID << ", " << tasks[i].exe_time << ", " << tasks[i].period << endl;
    // }

    //Header
    output_file << "#######################################" << endl;
    output_file << "RATE MONOTONIC SCHEDULER" << endl;
    output_file << "#######################################" << endl;
    output_file << "time, running task" << endl;
    output_file << "#######################################" << endl;
    
    // "#! TASK XX PREEMPTED BY TASK YY"
    // "#! TASK XX MISSED DEADLINE"
}