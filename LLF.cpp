#include "LLF.h"
#include "datatypes.h"
#include <fstream>
#include <deque>
#include <list>
#include <algorithm>

using namespace std;

void LLFScheduler(ofstream &output_file, int num_tasks, int sim_time, Task* periodic_tasks) {
   // Output header
   output_file << "#######################################" << endl;
   output_file << "LLF SCHEDULER" << endl;
   output_file << "#######################################" << endl;

   // Copy tasks so we don't fuck em up for the other algorithms
   Task* tasks = new Task[num_tasks];
   for (int i = 0; i < num_tasks; i++) {
      tasks[i] = periodic_tasks[i];
   }
   
   // Set initial deadline and priority
   for (int i = 0; i < num_tasks; i++){
      tasks[i].deadline = tasks[i].period;
      tasks[i].priority = tasks[i].exe_time;
      tasks[i].remaining_exe_time = tasks[i].exe_time;
      tasks[i].preemptions = 0;
      tasks[i].missed_deadlines = 0;
      tasks[i].response_time = 0;
   }
   
   // Initialize ready and not ready queues
   list<Task> ready;                      //TODO: Ideally, I'd like these to be lists of pointers,
   list<Task> not_ready;                  //but I can't figure out a convenient way to populate them
   ready.assign(tasks, tasks+num_tasks);
   
   Task* current_task = NULL;
   // Schedule tasks
   for (int tick = 0; tick < sim_time; tick++){
      // Update execution time of current task
      if (current_task != NULL) {
         current_task->remaining_exe_time--;
         
         // Check if task is completed
         if (current_task->remaining_exe_time == 0) {
            current_task->deadline += current_task->period;
            not_ready.push_front(*current_task);
            current_task = NULL;
         }
      }

      // Check for missed deadlines
      int num_ready_tasks = ready.size();
      list<Task>::iterator ready_task = ready.begin();
      // output_file << "Begin iterator:\n";
      for (int i = 0; i < num_ready_tasks; i++){
         if (ready_task->deadline < tick){
            ready_task->missed_deadlines++;
            ready_task->deadline += ready_task->period;
         }
         // output_file << "\t" << ready_task->ID << "\n";
         advance(ready_task, 1);
      }
      
      // // Update ready queue using erase-remove idiom
      //       // using remove_if method to move all the ready Tasks to the end and get the new logical end
      //       auto new_logical_end = std::remove_if(
      //          not_ready.begin(), not_ready.end(), [](Task nr_task, int tick) { return tick % nr_task.period == 0; });
            
      //       // add the newly ready tasks to the ready list
      //       ready.assign(new_logical_end, not_ready.end());

      //       // printing vector after using remove_if()
      //       not_ready.erase(new_logical_end, not_ready.end());
      //    }
      
      
      //    Update task priorities
      //       for task in ready
      //          new_priority = deadline - time - remaining_execution_time
      
      
      //    Get next task
      //       if task running
      //          get current task priority
      //       else
      //          current task priority = -1
      //       for task in ready
      //          if task priority > current task priority
      //             put current task back in ready
      //             move task from ready to running
      
      //    Print to file
      //       if no task running
      //          print: nothing :D
      //       else
      //          print task running
      
      
   }

   delete tasks;
}