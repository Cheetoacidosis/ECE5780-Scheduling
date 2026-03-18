#include "LLF.h"
#include "datatypes.h"
#include <fstream>
#include <deque>
#include <list>

using namespace std;

void LLFScheduler(ofstream &output_file, int num_tasks, int sim_time, Task* tasks){
   // Initialize ready and not ready queues
   Task* ready = new Task[num_tasks];
   Task* not_ready = new Task[num_tasks];

   for (int i = 0; i < num_tasks; i++) {
      ready[i] = tasks[i];
   }
   
   // Set initial deadline and priority
   for (int i = 0; i < num_tasks; i++){
      tasks[i].deadline = tasks[i].period;
      tasks[i].priority = tasks[i].exe_time;
      tasks[i].remaining_exe_time = tasks[i].exe_time;
   }

   // Schedule tasks
   for (int tick = 0; tick < sim_time; tick++){

      int num_ready_tasks = sizeof(ready) / sizeof(Task);
      for (int i = 0; i < num_ready_tasks; i++){
         // Check missed deadlines
         if (ready[i].deadline < tick){
            ready[i].missed_deadlines++;
            ready[i].deadline += ready[i].period;
         }
         // Update ready queue
         else if (ready[i].deadline == tick) {
            //PICK UP HERE
         }
      }
      
      
      //    Update task priorities
      //       for task in ready
      //          new_priority = deadline - time - remaining_execution_time
      
      //    Update execution time
      //       current task execution time --
      //       if current task remaining_execution_time == 0
      //          yay :D
      //          update deadline, move to not ready (unless time == deadline)
      
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
}