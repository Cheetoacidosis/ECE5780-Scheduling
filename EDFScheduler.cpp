#include "EDFScheduler.h"
#include <list>
#include <limits>

//find task with earliest absolute deadline
int FindEDFTask(list<int>& readyq, Task* tasks) {
    int earlyBird = -1;
    int ED = numeric_limits<int>::max();

    for (int t : readyq) {
        if (tasks[t].deadline < ED) {
            ED = tasks[t].deadline;
            earlyBird = t;
        }
    }
    return earlyBird;
}

void EDFScheduler(ofstream &output_file, int num_tasks, int sim_time, Task* tasks){

    output_file << "Earliest deadline first scheduler\n";
    output_file << "time: running task\n";

    list<int> readyq;
    list<int> not_readyq;

    //release
    for (int i = 0; i < num_tasks; i++) {
        tasks[i].remaining_exe_time = tasks[i].exe_time;
        tasks[i].deadline = tasks[i].period; //first deadline
        readyq.push_back(i);
    }

    int running_task = -1;

    for (int tick = 0; tick < sim_time; tick++) {

        //release new jobs
        for (int i = 0; i < num_tasks; i++) {
            if (tick != 0 && tick % tasks[i].period == 0) {

                //oops there it goes
                if (tasks[i].remaining_exe_time > 0) {
                    tasks[i].missed_deadlines++;
                    output_file << "! Task " << tasks[i].ID << " Missed deadline\n";
                }

                //Reset job
                tasks[i].remaining_exe_time = tasks[i].exe_time;
                tasks[i].deadline = tick + tasks[i].period;

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
                        << tasks[running_task].ID 
                        << ":\t" 
                        << tasks[running_task].remaining_exe_time << "\n";
        }

        //check if finished
        if(running_task != -1 && tasks[running_task].remaining_exe_time == 0){
            running_task = -1;
        }
    }
}