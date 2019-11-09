#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#include <fstream>

#include "Dice.h"
#include "Property.h"
#include "Player.h"
#include "Railroad.h"
#include "Action.h"
#include "Tile.h"
#include "Utility.h"
#include "GameLogic.h"

using namespace std;

// Use map to show who owns what propert(0 - no one, 1 - player1, 2 - player2, -1 - not ownable)

GameLogic::GameLogic() {
	FillPlayersVect();
	dice1 = Dice();
	dice2 = Dice();
	currentTurn = 0;
	FillGameBoard();
}

void GameLogic::PlayGame() {
	int numDoubles = 0;
	int turnRoll = 0;
	int dummy = 0;
	while (true) {
		cout << endl << "Player: " << (currentTurn + 1) <<  ".\nPress any key to roll -> ";
		cin.get();
		cin.ignore();
		dice1.RollDice();
		dice2.RollDice();
		turnRoll = dice1.GetDiceValue() + dice2.GetDiceValue();
		cout << endl << "Player: " << (currentTurn + 1) << " Roll: " <<  endl;
		cout << "Dice 1: " << dice1.GetDiceValue() << endl;
		cout << "Dice 2: " << dice2.GetDiceValue() << endl;
		cout << "Total Roll:" << turnRoll << endl << endl;
		// this handles the case of 1 or more dice rolls that have the same value
		while (dice1.GetDiceValue() == dice2.GetDiceValue()) {
			numDoubles++;
			cout << endl << "You have rolled " << numDoubles << " double. Three doubles in a row will land you in Jail. Shake those dice carefully!" << endl << endl;
			if (numDoubles == 3) {
				//FIXME: Execute go to jail action for current player
				break;
			}
			else {
				players.at(currentTurn).MovePosition(turnRoll);
				SequenceDecision(players.at(currentTurn).GetPosition(), turnRoll);
			}
			cout << endl << "Player: " << (currentTurn + 1) << ".\nPress any key to roll -> ";
			cin.get();
			cin.ignore();
			dice1.RollDice();
			dice2.RollDice();
			turnRoll = dice1.GetDiceValue() + dice2.GetDiceValue();
		}
		// this handles the case when the dice rolls do not have the same values
		//FIXME: Move this error checking into Player class
		players.at(currentTurn).MovePosition(turnRoll);
		SequenceDecision(players.at(currentTurn).GetPosition(), turnRoll);
		currentTurn++;
		if (currentTurn == players.size()) {
			currentTurn = 0;
		}
	}
}


void GameLogic::SequenceDecision(int position, int roll) {
	if (properties.find(position) != properties.end()) {
		PropertySequence(position);
	}
	else if (railroads.find(position) != railroads.end()) {
		RailroadSequence(position);
	}
	else if (utilities.find(position) != utilities.end()) {
		UtilitySequence(position, roll);
	}
	else {
		ActionSequence(position);
	}
}

void GameLogic::RailroadSequence(int position) {
	int cost = railroads[position].GetCost();
	string userResponse;
	if (railroads[position].PropIsOwned()) {
		players.at(currentTurn).PayRent(cost);
		players.at(railroads[position].GetOwnedBy()).CollectRent(cost);
	}
	else {
		railroads[position].PrintDescription();
		cout << endl << "Would you like to purchase this property? -> ";
		cin >> userResponse;
		if (userResponse == "yes") {
			// FIXME: Check if player has the networth to purchase this property and output that they own the property
			players.at(currentTurn).PurchaseProperty(railroads[position].GetCost());
			railroads[position].SetOwnedBy(currentTurn);
		}
	}
}

void GameLogic::PropertySequence(int position) {
	int cost = properties[position].GetCost();
	string userResponse;
	if (properties[position].PropIsOwned()) {
		cout << "This property is owned by player number: " << properties[position].GetOwnedBy() << " you owe $" << properties[position].GetRent() << " in rent.";
		players.at(currentTurn).PayRent(cost);
		players.at(properties[position].GetOwnedBy()).CollectRent(cost);
	}
	else {
		properties[position].PrintDescription();
		cout << endl << "Would you like to purchase this property? -> ";
		cin >> userResponse;
		if (userResponse == "yes") {
			// FIXME: Check if player has the networth to purchase this property and output that they own the property
			players.at(currentTurn).PurchaseProperty(properties[position].GetCost());
			properties[position].SetOwnedBy(currentTurn);
		}
	}
}

void GameLogic::UtilitySequence(int position, int roll) {
	int cost = utilities[position].GetCost();
	string userResponse;
	if (utilities[position].PropIsOwned()) {
		//FIXME: Print rent cost message
		players.at(currentTurn).PayRent(cost);
		players.at(utilities[position].GetOwnedBy()).CollectRent(cost);
	}
	else {
		utilities[position].PrintDescription();
		cout << endl << "Would you like to purchase this property? -> ";
		cin >> userResponse;
		if (userResponse == "yes") {
			// FIXME: Check if player has the networth to purchase this property and output that they own the property
			players.at(currentTurn).PurchaseProperty(utilities[position].GetCost());
			utilities[position].SetOwnedBy(currentTurn);
		}
	}
}

void GameLogic::ActionSequence(int position) {
	//FIXME: This is for testing. Needs to be removed later.
	if (actions.find(position) == actions.end()) {
		cout << "Failed" << endl;
	}
	actions[position].PrintDescription();
}


void GameLogic::AuctionSequence() {
	//Program this late if time allows
}

void GameLogic::FillGameBoard() {
	ifstream gameProps("game_props.txt");
	string propType;
	string name;
	string color;
	int cost;
	int rentBase;
	int rent1House;
	int rent2House;
	int rent3House;
	int rent4House;
	int rentHotel;
	int mortgage;
	int houseCost;
	int hotelCost;
	int index = 0;
	while (!gameProps.eof()) {
		gameProps >> propType;
		if (propType == "Property") {
			gameProps >> name;
			gameProps >> color;
			gameProps >> cost;
			gameProps >> rentBase;
			gameProps >> rent1House;
			gameProps >> rent2House;
			gameProps >> rent3House;
			gameProps >> rent4House;
			gameProps >> rentHotel;
			gameProps >> mortgage;
			gameProps >> houseCost;
			gameProps >> hotelCost;
			Property newProp = Property(name, color, cost, rentBase, rent1House, rent2House, rent3House, rent4House, rentHotel, mortgage, houseCost, hotelCost);
			properties.insert({ index, newProp });
		}
		else if (propType == "Action") {
			gameProps >> name;
			Action newAction = Action(name);
			actions.insert({ index, newAction });
		}
		else if (propType == "Railroad") {
			gameProps >> name;
			gameProps >> cost;
			gameProps >> rent1House;
			gameProps >> rent2House;
			gameProps >> rent3House;
			gameProps >> rent4House;
			gameProps >> mortgage;
			Railroad newRR = Railroad(name, cost, rent1House, rent2House, rent3House, rent4House, mortgage);
			railroads.insert({ index, newRR });
		}
		else {
			gameProps >> name;
			gameProps >> cost;
			gameProps >> mortgage;
			Utility newUtil = Utility(name, cost, mortgage);
			utilities.insert({ index, newUtil });
		}
		++index;
	}
}

void GameLogic::FillPlayersVect() {
	string userName;
	string keepAdding;
	cout << "Welcome to Monopoly!" << endl;
	cout << "Enter at least two, and up to six, players' names to start playing the game." << endl << endl;
	for (int i = 0; i < 6; ++i) {
		cout << "Enter name of player #" << (i + 1) << " -> ";
		getline(cin, userName);
		players.push_back(Player(userName, (i + 1)));
		if (i > 0) {
			cout << endl << "Would you like to add another player?\nPress y to add another player, any other key to start playing. -> ";
			getline(cin, keepAdding);
			if (keepAdding != "y" && keepAdding != "Y") {
				break;
			}
		}
		cout << endl;
	}
	cout << endl << "Let's get started!\n" << endl;
}

