#pragma once
//
// Calculator.h : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <regex>
#include <sstream>
#include "utils.h"

using namespace std;

class Calculator {
	vector<string> expressions;		// list of expressions
	typedef tuple<string, string> rhs_t; // <rhs, variable> tuple
	vector<rhs_t> rhs_list;			// list of <rhs, variable> tuple
	map<string, int> variable_map;	// variable:value map: <key,value> = <variable, value>
	set<string> unknown_vars{};		// current unknown variables 


public:

	enum Status {
		UNKNOWN = -1,   
		OK = 0,
		CHANGED,
		NOT_CHANGED,
		ERROR,
	};

	// add a variable:value to the variable map
	void add_variable(string var_name, int var_value = UNKNOWN) {
		this->variable_map[var_name] = var_value;
	}

	// add <rhs, variable> to rhs tuple vector
 	void add_rhs(string rhs, string var) {
		this->rhs_list.push_back(rhs_t(rhs, var));
	}

	// add expression to expressions vector
	void add_expression(string expr) {
		this->expressions.push_back(expr);
	}

	// get method
	vector<rhs_t> get_rhs_list() {
		return this->rhs_list;
	}

	// display expressions
	void display_expressions() {
		for (string e : this->expressions) {
			cout << e << endl;
		}
	}

	// display variable:value map
	void display_variable_map() {
		for (auto& kv : this->variable_map) {
			cout << kv.first << " = "
				<< (kv.second == UNKNOWN ? "Unknown" : to_string(kv.second))
				<< std::endl;
		}
	}

	// return true if all variables have values
	bool all_variables_set() {
		bool rc = true;
		for (auto& kv : this->variable_map) {
			if (kv.second == UNKNOWN)
				rc = false;
		}
		return rc;
	}

	// return true if variable map has same set of unknowns
	// maintain unknown list
	bool same_unknowns() {
		set<string> current_unknowns;
		for (auto& kv : this->variable_map) {
			if (kv.second == UNKNOWN)
				current_unknowns.insert(kv.first);
		}
		if (current_unknowns.size() > 0 && current_unknowns == this->unknown_vars)
			return true;
		else {
			// save current unknowns
			this->unknown_vars = current_unknowns;
			return false;
		}
	}

	// gather expressions and populate variable table and rhs table
	void gather_expressions(ifstream& file) {
		string line;
		//regex expr_regex("([a-zA-Z]*)\\s*=(.*)");   // This crashes with g++  
		regex expr_regex("(.*) = (.*)");     // regular expression <LHS> = <RHS>
		smatch expr_match;

		while (getline(file, line)) {
			if (!regex_match(line, expr_match, expr_regex)) {
				continue;
			}
			this->add_expression(expr_match[0]);// full expression
			string var = trim_blanks(expr_match[1]);
			this->add_variable(var);			// left hand side: variable 
			this->add_rhs(expr_match[2], var);	// tuple to process rhs
		}
	}

	// process the righthand side of an equation
	// return 
	// if the variable was changed, not changed, or if a rhs has an unresolvable cycle
	//   NOT_CHANGED: variable not modified 
	//   CHANGED:     variable was modified
	//   ERROR:       lhs and rhs have same variable -> cannot compute
	Calculator::Status process_rhs(rhs_t& var_rhs) {
		string expr = get<0>(var_rhs);
		string var = get<1>(var_rhs);
		stringstream ss(expr);
		string token;
		vector<string> tokens;

		// split rhs by whitespace
		while (ss >> token) tokens.push_back(token);
		bool complete = true;
		Calculator::Status changed = NOT_CHANGED;
		unsigned long sum = 0;

		// build up sum
		for (string s : tokens) {
			// operator
			if (s == "+")
				continue;
			// digits: add to sum
			if (is_digits(s))
				sum += stoi(s);
			else {
				// variable: if same var is found then immediately return error 
				if (s == var)
					return ERROR;
				// if variable exists and has value then use it
				if (this->variable_map.count(s) && this->variable_map[s] != UNKNOWN)
					sum += this->variable_map[s];
				else
					complete = false;
			}
		}

		// if calculation is complete then assign rhs value to variable.
		if (complete) {
			if (this->variable_map[var] == sum)
				changed = NOT_CHANGED;
			else {
				changed = CHANGED;
				this->variable_map[var] = sum;
			}
		}
		return changed;
	}

	// do all the math here
	Calculator::Status process_expressions() {
		// loop over the expressions processing rhs and variable
		// quit when:
		//  a full pass has no changes
		//  or all variables have values
		//  or the same unknowns exist from previous pass

		Calculator::Status rc = OK;
		bool stop_sign = false;
		while (!stop_sign) {
			bool changed = false;
			for (rhs_t var_rhs : this->get_rhs_list()) {
				Calculator::Status rc = this->process_rhs(var_rhs);
				if (rc == CHANGED)
					changed = true;
				else if (rc == ERROR) {
					stop_sign = true;
					break;
				}
			}
			if (!changed)
				stop_sign = true;

			if (this->all_variables_set())
				stop_sign = true;

			if (this->same_unknowns()) {
				rc = ERROR;
				stop_sign = true;
			}
		}
		return rc;
	}
};
