#ifndef _DATATYPES_H
#define _DATATYPES_H

#include <string>
using namespace std;

//Each group is an array of custom struct "Task"
//Eg. periodic_tasks[] is the array of all periodic tasks
//    periodic_tasks[0] is a Task struct for a single task. 
//    periodic_tasks[0].ID returns the name of the task

struct Task {
    string ID; //The ID or name of a task
    int exe_time; //Execution time in msec
    int period; //period T in msec
    int release_time; //Release time in msec

    int priority; //The priority of the task. Higher number = higher priority

    int preemptions; //Total number of times this task was preempted
    int missed_deadlines; //Total number of times this task missed a deadline

    int remaining_exe_time; //Tracks how long a task has run for. Resets to exe_time and counts down
    int deadline;           //Absolute deadline for this task. Set when the task is released
};
#endif