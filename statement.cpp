/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
using namespace std;

/* Implementation of the Statement class */

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}

Sequential::Sequential(string variable, Expression* ex, seqType ty) :var(variable), exp(ex), type(ty)
{}

Sequential::~Sequential()
{
	delete exp;
}

void Sequential::execute(EvalState & state, int & next)
{
	switch (type) {
	case In: {
		int tmp;
		cin >> tmp;
		if (cin.fail()) error("INVALID NUMBER");
		state.setValue(var, tmp); break;
	}
	case Out: {
		cout << state.getValue(var);
		break;
	}
	case Assign:
		state.setValue(var, exp->eval(state));
		break;
	case End: case REM:
		break;
	}
}

ctrlState::ctrlState(Expression* exp_str, int next, cmdType tp) 
	:cmd(tp), parsedState(exp_str), nextLine(next)
{}

ctrlState::~ctrlState()
{
	delete parsedState;
}

void ctrlState::execute(EvalState & state, int& next)
{
	switch (cmd) {
	case conditional:
		if (parsedState->eval(state)) {
			next = nextLine;
		}
		// temporary remain the same
		break;
	case jump:
		next = nextLine;
	default:
		error("LINE NUMBER ERROR");
	}
}