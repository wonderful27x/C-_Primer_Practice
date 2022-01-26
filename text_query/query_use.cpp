#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

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

	return;
}

void word_query(int argc, char **argv) {
	cout << "word_query use..." << endl;
	TextQuery file = get_file(argc, argv);
	do {
		string sought;
		if(!get_word(sought)) break;
		Query name(sought);
		const auto results = name.eval(file);
		cout << "\nExecuting Query for: " << name << endl;
		print(cout, result) << endl;
	}while(true);
}

void and_query(int argc, char **argv) {
	cout << "and_query use..." << endl;
	TextQuery file = get_file(argc, argv);
	do {
		string sought1, sought2;
		if(!get_words(sought1, sought2)) break;
		Query andq = Query(sought1) & Query(sought2);
		cout << "\nExecuting query for: " << andq << endl;
		auto results = andq.eval(file);
		print(cout, results);

		results = Query(sought1).eval(file);
		cout << "\nExecuting query: " << Query(sought1) << endl;
		print(cout, results);

		results = Query(southt2).eval(file);
		cout << "\nExecuting query: " << Query(sought2) << endl;
		print(cout, results);
	}while(true);

	return 0;
}

void and_or_query(int argc, char **argv) {
	cout << "and_or_query..." << endl;
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

	return 0;
}
