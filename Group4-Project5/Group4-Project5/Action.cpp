#include <iostream>
#include <string>

#include "Action.h"

using namespace std;

Action::Action(string name) : Tile(name, "Action") {}

string Action::GetActionType() {
	return this->GetName();
}

//uses action type to print out the proper description
void Action::PrintDescription() {
	if (this->GetName() == "CHANCE") {
		cout << "You got a chance card" << endl;
		//FIXME: have any idea about this one
	}
	else  if (this->GetName() == "JAIL") {
		cout << "Pay $200 or roll doubles to get out of Jail" << endl;
		//FIXME: add a function to handle this
	}
}