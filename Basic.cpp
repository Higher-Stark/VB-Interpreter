/*
 * File: Basic.cpp
 * ---------------
 * Name: Higher Stark
 * Section: Stark Teamwork
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
   cout << "Stub implementation of BASIC" << endl;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
		  if (ex.getMessage() == "QUIT") break;
         cerr << "Error: " << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == NUMBER) { 
	   int index = stringToInteger(token);
	   if (!scanner.hasMoreTokens()) {
		   program.removeSourceLine(index);
	   }
	   else {
		   token = scanner.nextToken();
		   type = scanner.getTokenType(token);
		   if  (type == WORD) {
			   if (token == "REM") {
				   Statement* new_state = new Sequential(line);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else if (token == "LET") {
				   token = scanner.nextToken();
				   if (isdigit(token[0])) error("SYNTAX ERROR");
				   string var = token;
				   token = scanner.nextToken();
				   if (token != "=") error("SYNTAX ERROR");
				   Expression* new_exp = readE(scanner, 0);
				   if (scanner.hasMoreTokens()) {
					   delete new_exp;
					   error("SYNTAX ERROR");
				   }
				   Statement* new_state = new Sequential(var, new_exp, Sequential::Assign);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else if (token == "PRINT") {
				   if (!scanner.hasMoreTokens()) error("SYNTAX ERROR");
				   Expression* exp = readE(scanner);
				   if (scanner.hasMoreTokens()) {
					   delete exp;
					   error("SYNTAX ERROR");
				   }
				   Statement* new_state = new Sequential(token, exp, Sequential::Out);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else if (token == "INPUT") {
				   token = scanner.nextToken();
				   if (scanner.getTokenType(token) != WORD) error("SYNTAX ERROR");
				   if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
				   Statement* new_state = new Sequential(token, nullptr, Sequential::In);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else if (token == "END") {
				   if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
				   Statement* new_state = new Sequential(token, nullptr, Sequential::End);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else if (token == "IF") {
				   Expression* new_exp = readE(scanner, 0);
				   token = scanner.nextToken();
				   if (token != "THEN") error("SYNTAX ERROR");
				   token = scanner.nextToken();
				   if (scanner.getTokenType(token) != NUMBER) error("SYNTAX ERROR");
				   Statement* new_state = new ctrlState(new_exp, stringToInteger(token), ctrlState::conditional);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else if (token == "GOTO") {
				   token = scanner.nextToken();
				   if (scanner.getTokenType(token) != NUMBER) error("SYNTAX ERROR");
				   if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
				   Statement* new_state = new ctrlState(nullptr, stringToInteger(token), ctrlState::jump);
				   program.addSourceLine(index, line);
				   program.setParsedStatement(index, new_state);
			   }
			   else {
				   scanner.saveToken(token);
				   error("SYNTAX ERROR");
			   }
			   
		   }
		   else error("SYNTAX ERROR");
	   }

   }
   else if (type == WORD) {
	   if (token == "LET") {
		   token = scanner.nextToken();
		   string tmpvar = token;
		   token = scanner.nextToken();
		   if (token != "=") error("SYNTAX ERROR");
		   Expression* exp = readE(scanner, -1);
		   if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
		   state.setValue(tmpvar, exp->eval(state));
		   delete exp;
	   }
	   else if (token == "INPUT") {
		   token = scanner.nextToken();
		   if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
		   cout << " ? ";
		   int tmp;
		   cin >> tmp;
		   while (cin.fail()) {
			   cin.clear();
			   cin.ignore(1024, '\n');
			   cout << "INVALID NUMBER" << endl;
			   cout << " ? ";
			   cin >> tmp;
		   }
			state.setValue(token, tmp);
	   }
	   else if (token == "PRINT") {
		   if (!scanner.hasMoreTokens()) error("SYNTAX ERROR");
		   Expression* exp = readE(scanner);
		   if (scanner.hasMoreTokens()) {
			   delete exp;
			   error("SYNTAX ERROR");
		   }
		   cout << exp->eval(state) << endl;
		   delete exp;
	   }
	   else if (token == "RUN") {
		   int lineNumber = program.getFirstLineNumber();
		   while (lineNumber != -1) {
			   Statement* stmt = program.getParsedStatement(lineNumber);
			   int cp(lineNumber);
			   stmt->execute(state, lineNumber);
			   if (lineNumber == cp) lineNumber = program.getNextLineNumber(lineNumber);
		   }
	   }
	   else if (token == "LIST") {
		   int lineNumber = program.getFirstLineNumber();
		   while (lineNumber != -1) {
			   cout << program.getSourceLine(lineNumber) << endl;
			   lineNumber = program.getNextLineNumber(lineNumber);
		   }
	   }
	   else if (token == "CLEAR") {
		   int lineNumber = program.getFirstLineNumber();
		   while (lineNumber != -1) {
			   program.removeSourceLine(lineNumber);
			   lineNumber = program.getNextLineNumber(lineNumber);
		   }
	   }
	   else if (token == "QUIT") {
		   error("QUIT");
	   }
	   else if (token == "HELP") {
		   cout << "DOC" << endl;
	   }
   }


}
