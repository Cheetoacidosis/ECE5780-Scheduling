#include "EDFScheduler.h"
#include <list>
#include <algorithm> //No code is complete without Al Gor
#include <limits>

int total_preemptions = 0;
int total_misses = 0;

//find task with earliest absolute deadline
int FindEDFTask(list<int>& readyq, Task* tasks) {
    int earlyBird = -1;
    int ED = numeric_limits<int>::max();

    for (int t : readyq) {
        if (tasks[t].deadline < ED) {
            ED =tasks[t].deadline;
            earlyBird = t;
        }
    }
    return earlyBird;
}

void EDFScheduler(ofstream &output_file, int num_tasks, int num_tasks_aperiodic, int sim_time, Task* periodic_tasks, Task* aperiodic_tasks){

    output_file << "Earliest deadline first scheduler\n";
    output_file << "time: running task\n";

    int TOTAL = num_tasks + num_tasks_aperiodic;

    //Combine all tasks into a single array
    Task* tasks = new Task[TOTAL];
    copy(periodic_tasks, periodic_tasks + num_tasks, tasks);
    copy(aperiodic_tasks, aperiodic_tasks + num_tasks_aperiodic, tasks + num_tasks);

    list<int> readyq;
    list<int> not_readyq;

    //initialize periodic tasks
    for (int i = 0; i < num_tasks; i++) {
        tasks[i].remaining_exe_time = tasks[i].exe_time;
        tasks[i].deadline = tasks[i].period; //first deadline
        tasks[i].release_time = 0;
        tasks[i].priority = 1;                   
        readyq.push_back(i);
    }

    //initialize aperiodic tasks
    for (int i = num_tasks; i < TOTAL; i++) {
        tasks[i].remaining_exe_time = tasks[i].exe_time;
        tasks[i].deadline = tasks[i].release_time + 500;
        tasks[i].priority = 0;                    
        not_readyq.push_back(i);
    }

    //release
    for (int i = 0; i < num_tasks; i++) {
       tasks[i].remaining_exe_time = tasks[i].exe_time;
       tasks[i].deadline = tasks[i].period; //first deadline
    }

    int running_task = -1;

    //release new jobs
    for (int tick = 0; tick < sim_time; tick++) {

        // Release aperiodic tasks
        {
            auto it = not_readyq.begin();
            while (it != not_readyq.end()) {
                int idx = *it;
                if (tasks[idx].release_time == tick) {
                    tasks[idx].remaining_exe_time = tasks[idx].exe_time;
                    tasks[idx].deadline = tick + 500;
                    readyq.push_back(idx);

                    it = not_readyq.erase(it);
                }
                else ++it;
            }
        }

        // Release periodic tasks
        for (int i = 0; i < num_tasks; i++) {
            if (tick != 0 && tick % tasks[i].period == 0) {

                if (tasks[i].remaining_exe_time > 0) {
                    tasks[i].missed_deadlines++;
                    total_misses++;
                    output_file << "! Task " << tasks[i].ID
                                << " missed its deadline\n";
                }

                tasks[i].remaining_exe_time = tasks[i].exe_time;
                tasks[i].deadline = tick + tasks[i].period;

                if (find(readyq.begin(), readyq.end(), i) == readyq.end())
                    readyq.push_back(i);
            }
        }

        //Select task
        int next_task = FindEDFTask(readyq, tasks);

        //Preemption check
        if (running_task != -1 && next_task != -1 &&
                tasks[next_task].deadline < tasks[running_task].deadline) {

            output_file << "! Task " << tasks[running_task].ID
                        << " Preempted by task " << tasks[next_task].ID << "\n";

           tasks[running_task].preemptions++;
           total_preemptions++; 
            readyq.push_back(running_task);
            running_task = -1;
        }

        //Dispatch
        if (running_task == -1 && next_task != -1) {
            running_task = next_task;
            readyq.remove(next_task);
        }

        //Execute
        if (running_task != -1) {
           tasks[running_task].remaining_exe_time--;
        }

        // //Finished execution
        // if (tasks[running_task].remaining_exe_time == 0) {
        //     running_task = -1;
        // }

        //Output
        if (running_task == -1) {
            output_file << tick << ":\t No running task; Slack\n";
        } else {
            output_file << tick << ":\t" 
                        <<tasks[running_task].ID 
                        << ":\t" 
                        <<tasks[running_task].remaining_exe_time << "\n";
        }

        //check if finished
        if(running_task != -1 && tasks[running_task].remaining_exe_time == 0){
            running_task = -1;
        }
    }

    output_file << "\nEDF summary\n";

    output_file << "Total Preemptions: " << total_preemptions << "\n";
    output_file << "Total Misses: " << total_misses << "\n";
}