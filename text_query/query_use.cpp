#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

#include "Query.h"
#include "TextQuery.h"

using std::set;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::cerr;
using std::cout;
using std::cin;
using std::endl;

void word_query(int argc, char **argv);
void and_query(int argc, char **argv);
void and_or_query(int argc, char **argv);

int main(int argc, char **argv) {
	word_query(argc, argv);
	and_query(argc, argv);
	and_or_query(argc, argv);

	return 0;
}

void word_query(int argc, char **argv) {
	cout << "\n============word_query use...=============" << endl;
	TextQuery file = get_file(argc, argv);
	do {
		string sought;
		if(!get_word(sought)) break;
		Query name(sought);
		const auto results = name.eval(file);
		cout << "\nExecuting Query for: " << name << endl;
		print(cout, results) << endl;
	}while(true);
}

void and_query(int argc, char **argv) {
	cout << "\n===========and_query use...===============" << endl;
	TextQuery file = get_file(argc, argv);
	do {
		string sought1, sought2;
		if(!get_words(sought1, sought2)) break;
		Query andq = Query(sought1) & Query(sought2);
		cout << "\nExecuting query for: " << andq << endl;
		auto results = andq.eval(file);
		print(cout, results);

		results = Query(sought1).eval(file);
		Query q1(sought1);
		cout << "\nExecuting query: " << q1 << endl;
		print(cout, results);

		results = Query(sought2).eval(file);
		Query q2(sought2);
		cout << "\nExecuting query: " << q2 << endl;
		print(cout, results);
	}while(true);
}

void and_or_query(int argc, char **argv) {
	cout << "\n===============and_or_query...=================" << endl;
	TextQuery file = get_file(argc, argv);
	while(true) {
		string sought1, sought2, sought3;
		if(!get_words(sought1, sought2)) break;
		cout << "\nenter third word: ";
		cin >> sought3;
		Query q = Query(sought1) & Query(sought2) | Query(sought3);
		cout << "\nExecuting query for: " << q << endl;
		const auto results = q.eval(file);
		print(cout, results);
	}
}
