#pragma once
#include <string>

//Each group is an array of custom struct "Task"
//Eg. periodic_tasks[] is the array of all periodic tasks
//    periodic_tasks[0] is a Task struct for a single task. 
//    periodic_tasks[0].ID returns the name of the task

struct Task {
    string ID; //The ID or name of a task
    int exe_time; //Execution time in msec
    int period; //period T in msec
    int release_time; //Release time in msec
};