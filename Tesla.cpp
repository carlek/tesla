// Tesla.cpp
//
// Your task is to write a command - line program to evaluate a set of equations.
// An equation is defined by :
// <LHS> = <RHS>
// • Each equation is specified on a separate line.
// • <LHS> is the left - hand side of the equation and is always a variable name.
// • A variable name is composed of letters from the alphabet(for which isalpha(c) is true).
// • <RHS> is the right hand side of the equation and can be composed of :
//    • variables
//    • unsigned integers
//    • the + operator

	
#include <fstream>
#include <string>
#include "Calculator.h"

using namespace std;

int main()
{
	// file containing a list of files containing tests
	ifstream input_file_list("UnitTests.txt");
	string file_name;

	// process each file
	while (getline(input_file_list, file_name)) {

		ifstream input_file(file_name);
		string line;

		// calculator object has all the goodies
		Calculator my_calc;

		// gather up all expressions
		my_calc.gather_expressions(input_file);

		// process all expressions  
		Calculator::Status is_error = my_calc.process_expressions();

		// if (a) error or (b) not all variables were set
		// then just print all expressions and go to the next test
		if (is_error == Calculator::Status::ERROR || !my_calc.all_variables_set()) {
			my_calc.display_expressions();
			continue;
		}

		// print variables and values.
		// since the variable map is a map, it is already sorted by key(variable name)
		my_calc.display_variable_map();
	}
}
