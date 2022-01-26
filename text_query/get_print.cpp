#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Query.h"
#include "TextQuery.h"

using std::string;
using std::cout;
using std::cin;
using std::ifstream;
using std::runtime_error;

TextQuery get_file(int argc, char **argv) {
	ifstream infile;
	if(argc == 2)
		infile.open(argv[1]);
	else
		infile.open("data/storyDataFile");
	if(!infile)
		throw runtime_error("No input file!");
	
	return TextQuery(infile);
}

bool get_word(string &s) {
	cout << "enter a word to search for, or q to quit: ";
	cin >> s;
	if(!cin || s == "q")
		return false;
	else
		return true;
}

bool get_words(string &s1, string &s2) {
	cout << "enter to words to search for, or q to quit: ";
	cin >> s1;
	if(!cin || s1 == "q")
		return false;
	cin >> s2;
	return true;
}
