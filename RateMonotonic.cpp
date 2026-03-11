#include "RateMonotonic.h"
#include <algorithm> //for sorting

void AssignPriorities(int num_tasks, Task* tasks, deque<int> periods_list);

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


    //Determine priorities
    deque<int> periods = UniquePeriods(num_tasks, tasks);
    int max_priority = periods.size();
    AssignPriorities(num_tasks, tasks, periods);

    //Create arrays of "Ready" and "Not Ready" Queues
    //Priorities are baked in by the position of the queue
    //Eg. readyq[0] is the queue of tasks with priority 0
    //    readyq[0].front() is the next task with priority 0 that should be run
    deque<int>* readyq = new deque<int>[max_priority];
    deque<int>* not_readyq = new deque<int>[max_priority];


    // Put every task into the Ready queue ("release" every task)
    for (int i = 0; i < num_tasks; i++){
        readyq[tasks[i].priority].push_back(i);
    }

    // Stores the index of the currently running task
    int running_task = -1; // anything less than zero is slack

    // Simulation
    for (int tick = 0; tick < sim_time; tick++){

        //
    }

    //Task iterator
    // for (int i = 0; i < num_tasks; i++){
    //     cout << "Task: " << tasks[i].ID << " Priority: " << tasks[i].priority << endl;
    // }

        // //See if this worked properly
    //     int list_size = periods.size();
    // for (int i = 0; i < list_size; i++){
    //     cout << "Period: " << periods[i] << endl;
    // }
}   



deque<int> UniquePeriods(int num_tasks, Task* tasks){
    /*
    Function: UniquePeriods

    Sifts through every task, and stores all the different periods that are used
    If multiple tasks share a period, it is only counted once

    Returns a deque of periods, in decending order (priority increasing order)
    */

    //For every task
        //Grab period
        //is period in list of known periods?
            //yes: do nothing
            //no: add to list & increment counter
    int period = 0;
    bool seen_before = false;
    deque<int> periods_list;

    //For each item in "tasks[]"
    for (int i = 0; i < num_tasks; i++){

        period = tasks[i].period;
        seen_before = false;

        //We will definitely have at least one period. Add it to the count if there is nothing
        if (periods_list.size() == 0){
            periods_list.push_back(period);

        } else {
            //Check if the current period is in the list
            for (int k = 0; k < periods_list.size(); k++){
                if (periods_list[k] == period){
                    seen_before = true;
                }
            }

            //If it was not in the list, add it and increment count
            if (seen_before == false){
                periods_list.push_back(period);
            }
        }
    }


    sort(periods_list.begin(), periods_list.end());
    reverse(periods_list.begin(), periods_list.end());

    return periods_list;
}


void AssignPriorities(int num_tasks, Task* tasks, deque<int> periods_list){
    /*
    Function: AssignPriorities

    Every task is given a priority. 
    The priority is added to the task struct's member variable "priority"
    Priorities are zero-indexed
    */

    //For each task
    for (int i = 0; i < num_tasks; i++){
        
        //Find task's matching period & assign priority 
        for (int k=0; k < periods_list.size(); k++){
            if (tasks[i].period == periods_list[k]){
                tasks[i].priority = k;
            }
        }

    }
}