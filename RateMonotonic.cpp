#include "RateMonotonic.h"
#include <algorithm> //for sorting
#include <list>
#include <string>

#define DEBUG(x) cout<<x<<endl;

void AssignPriorities(int num_tasks, Task* tasks, deque<int> periods_list);
int HighestAvailableTaskPriority(int max_priority, deque<int> queueArray[]);
void ReleasePeriodicTasks(int tick, list<int>* not_readyq, deque<int> readyq[], Task* tasks);

void testfunc(){

    printf("Hey I'm a linked function!\n");

    return;
}

void RMScheduler(ofstream &output_file, int num_tasks, int sim_time, Task* tasks){
    //Header
    output_file << "#######################################" << endl;
    output_file << "RATE MONOTONIC SCHEDULER" << endl;
    output_file << "#######################################" << endl;
    output_file << "time: running task: remaining execution time: deadline" << endl;
    output_file << "#######################################" << endl;


    //Determine priorities
    deque<int> periods = UniquePeriods(num_tasks, tasks);
    int max_priority = periods.size();
    AssignPriorities(num_tasks, tasks, periods);

    //Create arrays of "Ready" and "Not Ready" Queues
    //Priorities are baked in by the position of the queue
    //Eg. readyq[0] is the queue of tasks with priority 0
    //    readyq[0].front() is the next task with priority 0 that should be run
    deque<int>* readyq = new deque<int>[max_priority];
    list<int> not_readyq = *(new list<int>);


    // Put every task into the Ready queue ("release" every task)
    for (int i = 0; i < num_tasks; i++){
        readyq[tasks[i].priority].push_back(i);
        tasks[i].deadline = tasks[i].period - 1;
        tasks[i].remaining_exe_time = tasks[i].exe_time;
    }

    // Stores the index of the currently running task
    int running_task = -1; // anything less than zero is slack
    string missed_deadlines_string = "";

    // Simulation
    for (int tick = 0; tick < sim_time; tick++){

        missed_deadlines_string = "";

        //Do any "not ready's" need to be moved into ready?
        ReleasePeriodicTasks(tick, &not_readyq, readyq, tasks);

        //Does the running task complete this tick?
        if (tasks[running_task].remaining_exe_time <= 0){
            //Move currently running task to the not ready queue
            int priority = tasks[running_task].priority;
            not_readyq.push_back(running_task);
            
            // DEBUG("Added task to not_runningq: " << running_task)
            //Clear the running_task
            running_task = -1;
        }


        //Does a task miss a deadline this tick?
        int length = 0;
        int task_index;

        for (int i = 0; i < max_priority; i++){

            //Iterate through every priority queue
            length = readyq[i].size();
            for (int k = 0; k < length; k++){

                task_index = readyq[i][k];
                //Task missed a deadline
                if (tick == tasks[task_index].deadline){
                    tasks[task_index].missed_deadlines += 1;
                    missed_deadlines_string = missed_deadlines_string + tasks[task_index].ID + " missed a deadline: ";
                }
            }
        }


        //If not running task, find out if there are any ready and run them
        if (running_task < 0){
            //What is highest priority task that is ready?
            int high_priority = HighestAvailableTaskPriority(max_priority, readyq);
            
            // output_file << high_priority << endl;
            //TODO: if there are any aperiodic tasks that could run, run them and exit this IF

            //Run highest priority task that is ready
            if (high_priority >= 0){
                int task_index = readyq[high_priority].front();

                // output_file << task_index << endl;

                readyq[high_priority].pop_front();

                // output_file << readyq[high_priority].front() << endl;

                // tasks[task_index].remaining_exe_time = tasks[task_index].exe_time;
                running_task = task_index;
            }
        }



        //If running a task, 
            // grab its priority
            // if any higher priority tasks that should run
                // Preempt, and change higher priority task to the one that should run
        //Preemption - is there an available task of higher priority?
        else {
            int high_priority = HighestAvailableTaskPriority(max_priority, readyq);
            int low_priority = tasks[running_task].priority;
            if (high_priority > tasks[running_task].priority){
                // Preempt, and track that the preemption happened
                missed_deadlines_string = missed_deadlines_string + tasks[running_task].ID + " was preempted: ";
                tasks[running_task].preemptions += 1;
                readyq[low_priority].push_front(running_task);
                running_task = readyq[high_priority].front();
                readyq[high_priority].pop_front();
            }
        }


        //Decrement remaining_exe_time of running task (if there is a running task)
        if (running_task >= 0){
            tasks[running_task].remaining_exe_time -= 1;
        }

        //Report the time and currently running task 
        if (running_task < 0){
            output_file << tick << ":\t" << "NO RUNNING TASK - SLACK\n";
        } else {
            //TODO: Check if running task is a periodic or aperiodic task
            output_file << tick << ":\t" << tasks[running_task].ID << ":\t" << tasks[running_task].remaining_exe_time << ":\t" << tasks[running_task].deadline << ":\t" << missed_deadlines_string << endl;
            // output_file << tick << ":\t" << tasks[running_task].ID << ":\t" << missed_deadlines_string << endl;
        }
    }
}   



deque<int> UniquePeriods(int num_tasks, Task* tasks){
    /*
    Function: UniquePeriods

    Sifts through every task, and stores all the different periods that are used
    If multiple tasks share a period, it is only counted once.
    This is used when assigning task priorities

    Returns a deque of periods, in decending order (priority increasing order)
    */

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



//What is highest priority task that is ready?

int HighestAvailableTaskPriority(int max_priority, deque<int> queueArray[]){
    /*
    Function: HighestAvailableTaskPriority
    
    Looks at the "ready" tasks, and returns the highest priority that is ready to run
    */
    for (int i = max_priority - 1; i >= 0; i--){
        if(queueArray[i].empty() == false){
            return i;
        }
    }
    return -1;
}



void ReleasePeriodicTasks(int tick, list<int>* not_readyq, deque<int> readyq[], Task* tasks){
    /*
    Function: ReleasePeriodicTasks

    Moves periodic tasks from the not_readyq into their repective readyq's
    Only "releases" a periodic task if the current tick is N*period
    */

    if (not_readyq->empty() == true){
        return;
    }

    list<int>::iterator it;
    int size = not_readyq->size();
    // cout << "Starting for loop, tick #" << tick << " size = " << size << endl;

    for (int i = 0; i < size; i++){
        

        it = not_readyq->begin();
        advance(it, i);

        int task_index = *it;
        
        if ((tick % tasks[task_index].period) == 0) {
            int priority = tasks[task_index].priority;
            tasks[task_index].deadline = tick + tasks[task_index].period;
            tasks[task_index].remaining_exe_time = tasks[task_index].exe_time;
            readyq[priority].push_back(task_index);
            not_readyq->erase(it);
            //If we delete something, make sure to keep iterator in exactly the same spot, and decrement size
            i--;
            size--;
            // cout << "deleted from notreadyq: " << task_index << endl;
        }
    }

    // cout << "boutta return. Size = " << size << endl;
    return;
}