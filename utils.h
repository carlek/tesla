#pragma once
#include <string>
#include <sstream>
using namespace std;

// utility functions

bool is_digits(const string &str) {
	return all_of(str.begin(), str.end(), ::isdigit);
}

string trim_blanks(string s) {
	stringstream stream_trimmer;
	stream_trimmer << s;
	stream_trimmer >> s;
	return s;
}
