#include "LLF.h"
#include "datatypes.h"
#include <fstream>
#include <iostream>
#include <deque>
#include <list>
#include <algorithm>
#define DEBUG false

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
   
   // Schedule tasks
   Task* current_task = NULL;
   int total_missed_deadlines = 0;
   int total_preemptions = 0;
   
   for (int tick = 0; tick < sim_time; tick++){
      // Check for missed deadlines
      list<Task>::iterator ready_task;
      for (ready_task = ready.begin(); ready_task != ready.end(); ready_task++){
         if (ready_task->deadline < tick){
            ready_task->missed_deadlines++;
            ready_task->deadline += ready_task->period;
         }
         // output_file << "\t" << ready_task->ID << "\n";
      }
      
      // Update ready queue using erase-remove idiom
      // using remove_if method to move all the ready Tasks to the end and get the new logical end
      auto new_logical_end = std::remove_if(not_ready.begin(), not_ready.end(), [tick](Task nr_task){return tick % nr_task.period == 0;});
      
      // add the newly ready tasks to the ready list
      for(ready_task = new_logical_end; ready_task != not_ready.end(); ready_task++) {
         ready.push_front(*ready_task);
      }

      // erase newly ready tasks from the not_ready list
      not_ready.erase(new_logical_end, not_ready.end());
      
      // Update task priorities
      for(ready_task = ready.begin(); ready_task != ready.end(); ready_task++) {
         ready_task->priority = ready_task->deadline - tick - ready_task->remaining_exe_time;
      }

      if (DEBUG) {
         output_file << "--- Ready tasks ---" << endl;
         for(ready_task = ready.begin(); ready_task != ready.end(); ready_task++) {
            output_file << ready_task->ID << ":" << ready_task->priority << "\t";
         }
         output_file << endl;

         output_file << "--- Not ready tasks ---" << endl;
         for(ready_task = not_ready.begin(); ready_task != not_ready.end(); ready_task++) {
            output_file << ready_task->ID << ":" << ready_task->priority << "\t";
         }
         output_file << endl << endl;
      }
      /***** Get next task *****/
      int curr_task_priority = INT32_MAX;
      if (current_task != NULL) {
         curr_task_priority = current_task->priority;
      }
      
      // Check priority of all ready tasks
      Task* new_curr_task = NULL;
      for (ready_task = ready.begin(); ready_task != ready.end(); ready_task++) {
         if (ready_task->priority < curr_task_priority) {   //NOTE: the lowest number is highest priority, because it indicates how much slack the task has
            new_curr_task = &(*ready_task);
            curr_task_priority = ready_task->priority;
         }
      }

      // Move task from ready to current
      if (new_curr_task != NULL) {
         if (current_task != NULL) {
            ready.push_front(*current_task);
         }
         current_task = new_curr_task;
         ready.remove_if([&](const Task& t) {return t.ID == new_curr_task->ID;});; // This is really scary, so if tasks are randomly disappearing, this is prob why
      }

      if (DEBUG) {
         output_file << curr_task_priority << endl;
      }

      // Write to output file
      if (!DEBUG) {
         output_file << tick << ":\t";
         if (current_task != NULL) {
            output_file << current_task->ID << ":\t";
         }
         else {
            output_file << "Nothing :3" << ":\t";
         }
         output_file << endl;
      }

      // Update execution time of current task
      if (current_task != NULL) {
         current_task->remaining_exe_time--;
         
         // Check if task is completed
         if (current_task->remaining_exe_time == 0) {
            current_task->deadline += current_task->period;
            current_task->remaining_exe_time = current_task->exe_time;
            not_ready.push_front(*current_task);
            current_task = NULL;
         }
      }
   }

   delete tasks;
}
