#include "RateMonotonic.h"
#include <algorithm> //for sorting
#include <list>
#include <string>

#define DEBUG(x) cout<<x<<endl;

void AssignPriorities(int num_tasks, Task* tasks, deque<int> periods_list);
int HighestAvailableTaskPriority(int max_priority, deque<int> queueArray[]);
void ReleaseTasks(int tick, list<int>* not_readyq, deque<int> readyq[], Task* tasks);
void PrintSummary(ofstream &output_file, Task* tasks, int num_tasks);

void RMScheduler(ofstream &output_file, int num_tasks, int num_tasks_aperiodic, int sim_time, Task* periodic_tasks, Task* aptasks){
    //Header
    output_file << "#######################################" << endl;
    output_file << "RATE MONOTONIC SCHEDULER" << endl;
    output_file << "#######################################" << endl;
    output_file << "time: running task ID" << endl;
    output_file << "#######################################" << endl;

    //Combine periodic and aperiodic tasks into one array, "tasks"
    Task* tasks = new Task[num_tasks + num_tasks_aperiodic];
    copy(periodic_tasks, periodic_tasks + num_tasks, tasks);
    copy(aptasks, aptasks + num_tasks_aperiodic, tasks + num_tasks);

    //Determine priorities
    deque<int> periods = UniquePeriods(num_tasks, periodic_tasks);
    int max_priority = periods.size() + 1;
    AssignPriorities(num_tasks + num_tasks_aperiodic, tasks, periods);

    //Create arrays of "Ready" and "Not Ready" Queues
    //Priorities are baked in by the position of the queue
    //Eg. readyq[0] is the queue of tasks with priority 0
    //    readyq[0].front() is the next task with priority 0 that should be run
    deque<int>* readyq = new deque<int>[max_priority + 1];
    list<int> not_readyq = *(new list<int>);
    deque<int> aperiod_readyq = *(new deque<int>);


    // Put every task into the Ready queue ("release" every task)
    for (int i = 0; i < num_tasks + num_tasks_aperiodic; i++){
        //If task is aperiodic, put into notreadyq instead
        int pri = tasks[i].priority;
        if (pri == 0){
            not_readyq.push_back(i);
        }
        else { 
            readyq[pri].push_back(i);
            tasks[i].deadline = tasks[i].period - 1;
            tasks[i].remaining_exe_time = tasks[i].exe_time;
        }
    }

    // Stores the index of the currently running task
    int running_task = -1; // anything less than zero is slack
    string missed_deadlines_string = "";

    // Simulation
    for (int tick = 0; tick < sim_time; tick++){

        missed_deadlines_string = "";

        //Do any "not ready's" need to be moved into ready?
        ReleaseTasks(tick, &not_readyq, readyq, tasks);

        //Does the running task complete this tick?
        if (tasks[running_task].remaining_exe_time <= 0){
            //Move currently running task to the not ready queue
            int priority = tasks[running_task].priority;
            not_readyq.push_back(running_task);
            //If aperiodic, track the response time
            if (priority == 0){
                tasks[running_task].response_time = tick - tasks[running_task].release_time;
            }
            
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

            int high_priority = HighestAvailableTaskPriority(max_priority, readyq);

            //Run highest priority task that is ready
            if (high_priority >= 0){
                int task_index = readyq[high_priority].front();
                readyq[high_priority].pop_front();
                running_task = task_index;
            }
        }


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
            // output_file << tick << ":\t" << tasks[running_task].ID << ":\t" << tasks[running_task].remaining_exe_time << ":\t" << tasks[running_task].deadline << ":\t" << missed_deadlines_string << endl;
            output_file << tick << ":\t" << tasks[running_task].ID << ":\t" << missed_deadlines_string << endl;
        }
    }

    PrintSummary(output_file, tasks, num_tasks + num_tasks_aperiodic);

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
    The priority is added to the task struct's property "priority"
    Priorities are zero-indexed
    */

    //For each task
    for (int i = 0; i < num_tasks; i++){
        //Sort periodic from nonperiodic

        //Periodic tasks
        if ((tasks[i].period != 0) && (tasks[i].release_time == 0)){
            //Find task's matching period & assign priority 
            for (int k=0; k < periods_list.size(); k++){
                if (tasks[i].period == periods_list[k]){
                    tasks[i].priority = k + 1;
                }
            }
        } 
        //Aperiodic tasks
        else {
            tasks[i].priority = 0;
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



void ReleaseTasks(int tick, list<int>* not_readyq, deque<int> readyq[], Task* tasks){
    /*
    Function: ReleaseTasks

    Moves periodic tasks from the not_readyq into their repective readyq's
    Only "releases" a periodic task if the current tick is N*period

    Releases aperiodic tasks when theyre ready
    */


    if (not_readyq->empty() == true){
        return;
    }

    list<int>::iterator it;
    int size = not_readyq->size();
    

    for (int i = 0; i < size; i++){
        

        it = not_readyq->begin();
        advance(it, i);

        int task_index = *it;
        
        //Aperiodic task that should be released this tick
        if (tasks[task_index].priority == 0){
            if (tasks[task_index].release_time == tick){
                tasks[task_index].deadline = tick + 500;
                tasks[task_index].remaining_exe_time = tasks[task_index].exe_time;
                readyq[0].push_back(task_index);
                not_readyq->erase(it);

                //If we delete something, make sure to keep iterator in exactly the same spot, and decrement size
                i--;
                size--;
            }
        }
        //Periodic task that should be released this tick
        else if ((tick % tasks[task_index].period) == 0) {

            int priority = tasks[task_index].priority;
            tasks[task_index].deadline = tick + tasks[task_index].period;
            tasks[task_index].remaining_exe_time = tasks[task_index].exe_time;
            readyq[priority].push_back(task_index);
            not_readyq->erase(it);
            //If we delete something, make sure to keep iterator in exactly the same spot, and decrement size
            i--;
            size--;
        }
    }


    return;
}


void PrintSummary(ofstream &output_file, Task* tasks, int num_tasks){
    output_file << "#######################################" << endl;
    output_file << "SUMMARY: " << endl;

    int total_preempt, total_miss = 0;

    //Task - # of preemptions - # missed deadlines - (aperiod) response time
    for (int i = 0; i < num_tasks; i++){
        output_file << "Task " << tasks[i].ID;
        output_file << ":\t # Preemptions " << tasks[i].preemptions;
        output_file << ":\t # Missed Deadlines " << tasks[i].missed_deadlines;

        if (tasks[i].priority == 0){
            output_file << ":\t Response Time " << tasks[i].response_time << " ms";
        }

        output_file << endl;

        total_miss += tasks[i].missed_deadlines;
        total_preempt += tasks[i].preemptions;
    }

    //Total - # of preemptions - # missed deadlines -
    output_file << endl;
    output_file << "TOTALS:" << endl;
    output_file << "Total Preemptions: \t\t" <<total_preempt << endl;
    output_file << "Total Missed Deadlines: " << total_miss << endl;
}