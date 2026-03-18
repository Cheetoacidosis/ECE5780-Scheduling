#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#include "RateMonotonic.h"
#include "datatypes.h"

int main(int argc, char *argv[]) {
    //Ensure that exactly 2 command-line arguments have been passed
    if (argc != 3){
        cout << "Incorrect number of input arguments\n";
        cout << "Please use the format './scheduler input.txt output.txt'\n";
        return 1;
    }

    //Attempt to open both files
    char* input_name = argv[1];
    char* output_name = argv[2];

    ifstream input_file;
    ofstream output_file;

    //Check that input opened successfully
    input_file.open(input_name);
    if(!input_file.is_open()){
        cout << "Failed to open input file: " << input_name << endl;
    }

    //Check that output opened successfully
    output_file.open(output_name);
    if(!output_file.is_open()){
        cout << "Failed to create output file: " << input_name << endl;
    }


    //Parse the input file into periodic and aperiodic tasks

    //Determine number of periodic tasks, and sim time
        // Should check that the value is valid, but I wont
    string num_tasks_string, sim_time_string;
    getline(input_file, num_tasks_string);
    getline(input_file, sim_time_string);
    int num_tasks = stoi(num_tasks_string);
    int sim_time = stoi(sim_time_string);

    //Read out all of the periodic tasks, and store them in an array.
    //Each group is an array of custom struct "Task"
    //Eg. periodic_tasks[] is the array of all periodic tasks
    //    periodic_tasks[0] is a Task struct for a single task. 
    //    periodic_tasks[0].ID returns the name of the task
    Task* periodic_tasks = new Task[num_tasks];

    string A;
    for(int i = 0; i < num_tasks; i++){
        getline(input_file, A, ',');
        // cout << A << ", ";
        periodic_tasks[i].ID = A;

        getline(input_file, A, ',');
        // cout << A << ", ";
        periodic_tasks[i].exe_time = stoi(A);

        getline(input_file, A);
        // cout << A << endl;
        periodic_tasks[i].period = stoi(A);

        periodic_tasks[i].preemptions = 0;
        periodic_tasks[i].missed_deadlines = 0;
    }

    //Check if we have any aperiodic tasks to store
    num_tasks_string = "0";
    getline(input_file, num_tasks_string); 
    int num_tasks_aperiodic = stoi(num_tasks_string); 

    //Store all aperiodic tasks in an array. If no aperiodic tasks are in the input file, this array has length 0
    Task* aperiodic_tasks = new Task[num_tasks_aperiodic];

    //Read aperiodic tasks
    if (num_tasks_string != "0"){ 
        for(int i = 0; i < num_tasks_aperiodic; i++){
            getline(input_file, A, ',');
            aperiodic_tasks[i].ID = A;

            getline(input_file, A, ',');
            aperiodic_tasks[i].exe_time = stoi(A);

            getline(input_file, A);
            aperiodic_tasks[i].release_time = stoi(A);

            aperiodic_tasks[i].preemptions = 0;
            aperiodic_tasks[i].missed_deadlines = 0;
        }
    } 

    //Simulate Rate Monotonic Scheduling, and write the simulation to the output file
    RMScheduler(output_file, num_tasks, num_tasks_aperiodic, sim_time, periodic_tasks, aperiodic_tasks);


    input_file.close();
    output_file.close();

    return 0;
}