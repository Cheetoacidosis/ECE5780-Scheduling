#include "LLF.h"
#include "datatypes.h"
#include <fstream>
#include <iostream>
#include <deque>
#include <list>
#include <algorithm>
#define DEBUG false

using namespace std;

void LLFScheduler(ofstream &output_file, int num_tasks, int num_tasks_aperiodic, int sim_time, Task* periodic_tasks, Task* aperiodic_tasks) {
   // Output header
   output_file << "#######################################" << endl;
   output_file << "LLF SCHEDULER" << endl;
   output_file << "#######################################" << endl;
   output_file << "Tick:\tWarning messages (if applicable):\tTask ID" << endl;
   
   // Initialize task list with periodic tasks
   Task* tasks = new Task[num_tasks + num_tasks_aperiodic];
   for (int i = 0; i < num_tasks; i++) {
      tasks[i] = periodic_tasks[i];

      tasks[i].priority = tasks[i].exe_time;

      tasks[i].preemptions = 0;
      tasks[i].missed_deadlines = 0;
      tasks[i].response_time = 0;

      tasks[i].remaining_exe_time = tasks[i].exe_time;
      tasks[i].deadline = tasks[i].period;
   }
   
   // Add aperiodic tasks to task list and initialize
   for (int i = 0; i < num_tasks_aperiodic; i++) {
      tasks[num_tasks + i] = aperiodic_tasks[i];

      tasks[num_tasks + i].priority = INT32_MAX;

      tasks[num_tasks + i].preemptions = 0;
      tasks[num_tasks + i].missed_deadlines = 0;
      tasks[num_tasks + i].response_time = 0;

      tasks[num_tasks + i].remaining_exe_time = tasks[num_tasks + i].exe_time;
      tasks[num_tasks + i].deadline = tasks[num_tasks + i].release_time + 500;
   }
   
   // Initialize ready and not ready queues
   list<Task*> ready;
   list<Task*> not_ready;
   list<Task*>::iterator task_it;
   for (int i = 0; i < num_tasks; i++) {
      ready.push_back(&tasks[i]);
   }

   for (int i = 0; i < num_tasks_aperiodic; i++) {
      if (tasks[i + num_tasks].release_time == 0) {
         ready.push_back(&tasks[i + num_tasks]);
      }
      else {
         not_ready.push_back(&tasks[i + num_tasks]);
      }
   }
   
   // Schedule tasks
   Task* current_task = NULL;
   int total_missed_deadlines = 0;
   int total_preemptions = 0;
   
   for (int tick = 0; tick < sim_time; tick++){
      if (!DEBUG){
         output_file << tick << ":\t";
      }
      // Check for missed deadlines
      for (task_it = ready.begin(); task_it != ready.end(); task_it++){
         if ((*task_it)->deadline < tick){
            (*task_it)->missed_deadlines++;
            total_missed_deadlines++;

            // If task is periodic, update deadline, otherwise move it to not ready
            if ((*task_it)->period != 0) {
               (*task_it)->deadline += (*task_it)->period;
            }
            else {
               not_ready.push_back(*task_it);
               ready.erase(task_it);
            }

            if (!DEBUG) {
               output_file << "!" << (*task_it)->ID << " missed deadline" << " : \t";
            }
         }
      }
      
      // Update ready queue
      for (task_it = not_ready.begin(); task_it != not_ready.end();) {  // No expression because items could be removed from list in the loop and just increasing after that would be bad
         // If the task is periodic, check if it's ready to release again
         if ((*task_it)->period != 0) {
            if (tick % (*task_it)->period == 0) {
               ready.push_back((*task_it));
               if (not_ready.size() == 1) { // This is so it doesn't get mad if it tries to delete the last element in not_ready
                  not_ready.clear();
                  task_it = not_ready.end();
               }
               else {
                  task_it = not_ready.erase(task_it); // Erase current item and get next item in list
               }
            }
            else {
               task_it++;
            }
         }
         // If the task is aperiodic, check if it's time to release
         else {
            if ((*task_it)->release_time == tick) {
               ready.push_back((*task_it));
               if (task_it == --not_ready.end()) { // This is so it doesn't get mad if it tries to delete the last element in not_ready
                  not_ready.clear();
                  task_it = not_ready.end();
               }
               else {
                  task_it = not_ready.erase(task_it); // Erase current item and get next item in list
               }
            }
            else {
               task_it++;
            }
         }
      }
      
      // Update task priorities (only for periodic tasks)
      for(task_it = ready.begin(); task_it != ready.end(); task_it++) {
         if ((*task_it)->period != 0) {
            (*task_it)->priority = (*task_it)->deadline - tick - (*task_it)->remaining_exe_time;
         }
      }

      // Prepare to get next task
      int curr_task_priority = INT32_MAX;
      if (current_task != NULL) {
         curr_task_priority = current_task->priority;
      }
      
      // Check priority of all ready tasks- should current be pre-empted?
      Task* new_curr_task = NULL;
      for (task_it = ready.begin(); task_it != ready.end(); task_it++) {
         if ((*task_it)->priority <= curr_task_priority) {   //NOTE: the lowest number is highest priority, because it indicates how much slack the task has
            new_curr_task = *task_it;
            curr_task_priority = (*task_it)->priority;
         }
      }

      // Update current task if necessary
      if (new_curr_task != NULL) {
         if (current_task != NULL) {
            current_task->preemptions++;
            total_preemptions++;
            if (!DEBUG) {
               output_file << "!" << current_task->ID << " pre-empted" << ":\t";
            }
            ready.push_back(current_task);
         }
         current_task = new_curr_task;
         ready.remove(new_curr_task);
      }

      // Write to output file
      if (!DEBUG) {
         if (current_task != NULL) {
            output_file << current_task->ID << endl;;
         }
         else {
            output_file << "NO RUNNING TASK - SLACK" << endl;;
         }
      }

      // Update response time of all aperiodic tasks
      for (task_it = ready.begin(); task_it != ready.end(); task_it++) {
         if ((*task_it)->period == 0) {
            (*task_it)->response_time++;
         }
      }
      if (current_task != NULL) {
         current_task->response_time++;
      }

      // Update execution time of current task
      if (current_task != NULL) {
         current_task->remaining_exe_time--;
         
         // Check if task is completed
         if (current_task->remaining_exe_time == 0) {
            current_task->deadline += current_task->period;
            current_task->remaining_exe_time = current_task->exe_time;
            not_ready.push_back(current_task);
            current_task = NULL;
         }
      }
   }

   // Report summary
   output_file << "################ SUMMARY ################" << endl;
   for (int i = 0; i < num_tasks + num_tasks_aperiodic; i++) {
      output_file << "Task ID " << tasks[i].ID << ": ";
      output_file  << "Number of pre-emptions: "<< tasks[i].preemptions << "\t";
      output_file << "Number of missed deadlines: " << tasks[i].missed_deadlines << "\t";
      if (tasks[i].period == 0) {
         output_file << "Response time: " << tasks[i].response_time << "ms";
      }
      output_file << endl;
   }
   output_file << "################ TOTALS #################" << endl;
   output_file << "Total pre-emptions: " << total_preemptions << endl;
   output_file << "Total missed deadlines: " << total_missed_deadlines << endl;

   return;
}
