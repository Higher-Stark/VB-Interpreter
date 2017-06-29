/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
using namespace std;

Program::Program() {
   // map initialize itself
}

Program::~Program() {
	for (map<int, Statement*>::iterator it = rep.begin(); it != rep.end(); ++it) {
		delete it->second;
	}
}

void Program::clear() {
	src.clear();
	for (map<int, Statement*>::iterator it = rep.begin(); it != rep.end(); ++it) {
		delete it->second;
	}
	rep.clear();
}

void Program::addSourceLine(int lineNumber, string line) {
	if (src.find(lineNumber) == src.end()) {
		src.insert(pair<int, string>(lineNumber, line));
    }
	else {
		src[lineNumber] = line;
	}
}

void Program::removeSourceLine(int lineNumber) {
	if (src.find(lineNumber) == src.end()) return;
	src.erase(lineNumber);
	delete rep[lineNumber];
	rep.erase(lineNumber);
}

string Program::getSourceLine(int lineNumber) {
	if (src.find(lineNumber) != src.end()) return src[lineNumber];
	else return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
	if (rep.find(lineNumber) == rep.end()) {
		rep.insert(pair<int, Statement*>(lineNumber, stmt));
	}
	else {
		delete rep[lineNumber];
		rep[lineNumber] = stmt;
	}
}

Statement *Program::getParsedStatement(int lineNumber) {
	if (rep.find(lineNumber) == rep.end()) return nullptr;
	return rep[lineNumber];
}

int Program::getFirstLineNumber() {
	if (src.empty()) return -1;
	int ret(INT32_MAX);
	for (map<int, string>::const_iterator it = src.cbegin(); it != src.cend(); ++it) {
		if (it->first < ret) ret = it->first;
	}
	return ret;
}

int Program::getNextLineNumber(int lineNumber) {
	if (src.empty()) return -1;
	int ret(INT32_MAX);
	for (map<int, string>::const_iterator it = src.cbegin(); it != src.end(); ++it) {
		if (it->first > lineNumber && it->first < ret) ret = it->first;
	}
	if (ret == INT32_MAX) return -1;
	else return ret;
}
