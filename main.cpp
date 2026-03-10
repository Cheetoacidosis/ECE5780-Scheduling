#include <iostream>
#include <fstream>
using namespace std;
#include "RateMonotonic.h"

int main(int argc, char *argv[]) {
    //Ensure that exactly 2 command-line arguments have been passed
    if (argc != 3){
        cout << "Incorrect number of input arguments\n";
        cout << "Please use the format './scheduler input.txt output.txt'";
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
        cout << "Failed to open input file: " << input_name;
    }

    //Check that output opened successfully
    output_file.open(output_name);
    if(!output_file.is_open()){
        cout << "Failed to create output file: " << input_name;
    }

    return 0;
}