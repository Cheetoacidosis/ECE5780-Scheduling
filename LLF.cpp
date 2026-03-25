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
   // Task* tasks = new Task[num_tasks];
   // for (int i = 0; i < num_tasks; i++) {
   //    tasks[i] = periodic_tasks[i];
   // }
   
   Task* tasks = periodic_tasks;
   
   // Set initial deadline and priority
   for (int i = 0; i < num_tasks; i++){
      tasks[i].deadline = tasks[i].period;
      tasks[i].priority = tasks[i].exe_time;
      tasks[i].remaining_exe_time = tasks[i].exe_time;
      tasks[i].preemptions = 0;
      tasks[i].missed_deadlines = 0;
      tasks[i].response_time = 0;
   }

   output_file << "##### CREATURE REPORT #####" << endl;
   for (int i = 0; i < num_tasks; i++) {
      output_file << "Task: " << tasks[i].ID << "\t";
      output_file << "Period: " << tasks[i].period << "\t";
      output_file << endl;
   }
   
   // Initialize ready and not ready queues
   list<Task*> ready;                      //TODO: Ideally, I'd like these to be lists of pointers,
   list<Task*> not_ready;                  //but I don't think I have time to refactor shit
   list<Task*>::iterator task_it;
   for (int i = 0; i < num_tasks; i++) {
      ready.push_back(&periodic_tasks[i]);
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
            (*task_it)->deadline += (*task_it)->period;

            if (!DEBUG) {
               output_file << "Task " << (*task_it)->ID << "missed deadline" << ":\t";
            }
         }
      }
      
      // Update ready queue
      for (task_it = not_ready.begin(); task_it != not_ready.end();) {
         if (tick % (*task_it)->period == 0) {
            ready.push_back((*task_it));
            if (task_it == --not_ready.end()) { // This is so it doesn't get mad when deleting the last element
               not_ready.clear();
               task_it = not_ready.end();
            }
            else {
               task_it = not_ready.erase(task_it); // Get the next position after erasing the task pointer
            }
            if (DEBUG) {
               output_file << (*task_it)->ID << (*task_it)->period << endl;
            }
         }
         else {
            task_it++;
         }
      }
      
      // Update task priorities
      for(task_it = ready.begin(); task_it != ready.end(); task_it++) {
         (*task_it)->priority = (*task_it)->deadline - tick - (*task_it)->remaining_exe_time;
      }

      if (DEBUG) {
         output_file << "Tick " << tick << endl;
         output_file << "--- Current task ---" << endl;
         if (current_task != NULL) {
            output_file << current_task->ID << endl;
         }
         else {
            output_file << "None" << endl;
         }
         output_file << "--- Ready tasks ---" << endl;
         for(task_it = ready.begin(); task_it != ready.end(); task_it++) {
            output_file << (*task_it)->ID << ":" << (*task_it)->priority << "\t";
         }
         output_file << endl;

         output_file << "--- Not ready tasks ---" << endl;
         for(task_it = not_ready.begin(); task_it != not_ready.end(); task_it++) {
            output_file << (*task_it)->ID << ":" << (*task_it)->priority << "\t";
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
      for (task_it = ready.begin(); task_it != ready.end(); task_it++) {
         if ((*task_it)->priority < curr_task_priority) {   //NOTE: the lowest number is highest priority, because it indicates how much slack the task has
            new_curr_task = *task_it;
            curr_task_priority = (*task_it)->priority;
         }
      }

      // Update current task
      if (new_curr_task != NULL) {
         if (current_task != NULL) {
            current_task->preemptions++;
            total_preemptions++;
            if (!DEBUG) {
               output_file << "Task " << current_task->ID << " was preempted by Task " << new_curr_task->ID << current_task->preemptions << ":\t";
            }
            ready.push_back(current_task);
         }
         current_task = new_curr_task;
         ready.remove(new_curr_task);
         // ready.remove_if([new_curr_task](const Task* current_task) {return current_task->ID == new_curr_task->ID;});
      }

      // Write to output file
      if (!DEBUG) {
         if (current_task != NULL) {
            output_file << "Task " << current_task->ID << " is running" << endl;;
         }
         else {
            output_file << "Nothing is running" << endl;;
         }
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

   // Task* completed_tasks = new Task[num_tasks];
   // int i = 0;
   // for (task_it = ready.begin(); task_it != ready.end(); task_it++) {
   //    completed_tasks[i] = *task_it;
   //    i++;
   // }
   // for (task_it = not_ready.begin(); task_it != not_ready.end(); task_it++) {
   //    completed_tasks[i] = *task_it;
   //    i++;
   // }
   // if (current_task != NULL) {
   //    completed_tasks[i] = *current_task;
   // }

   // Report summary
   output_file << "################ SUMMARY ################" << endl;
   for (int i = 0; i < num_tasks; i++) {
      output_file << "Task ID " << tasks[i].ID << ": ";
      output_file << tasks[i].preemptions << " preemptions, ";
      output_file << tasks[i].missed_deadlines << " missed deadlines" << endl;
   }
   output_file << "################ TOTALS #################" << endl;
   output_file << "Total preemptions: " << total_preemptions << endl;
   output_file << "Total missed deadlines: " << total_missed_deadlines << endl;

   // delete tasks;
   return;
}
