/*
 * game.cpp
 *
 *  Created on: Mar 8, 2024
 *      Author: alyssaditroia
 */


#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>


using namespace std;

const char *LEADERBOARD_FILE = "leaderboard.txt";

/*----------------------------------------------------
 class for Player
----------------------------------------------------*/

class Player {
public:
	Player() : number_of_guesses(0) {}

	Player(string nm, int ng = 0, bool leader = false) :
			player_name(nm), number_of_guesses(ng), is_leader(leader) {}

	bool getIsLeader() const { return is_leader; }

	void setIsLeader(bool isLeader) { is_leader = isLeader; }

	int getNumberOfGuesses() const { return number_of_guesses; }

	void setNumberOfGuesses(int numberOfGuesses) { number_of_guesses = numberOfGuesses; }

	const string& getPlayerName() const { return player_name; }

	void setPlayerName(const string &playerName) { player_name = playerName; }

	int incrementGuessCount();

	void playerName();

private:
	string player_name;
	int number_of_guesses;
	bool is_leader;

};
/*----------------------------------------------------
 Player Member Functions
----------------------------------------------------*/

/*----------------------------------------------------
 Prompts user for their name and sets their name
----------------------------------------------------*/
void Player::playerName() {
	string name;
	cout << "Please Enter your name: ";
	cin >> name;
	setPlayerName(name);
}
/*----------------------------------------------------
 Increments the user's guess count
----------------------------------------------------*/
int Player::incrementGuessCount() {
	number_of_guesses++;
	return number_of_guesses;
}
/*----------------------------------------------------
 class for the LeaderBoard
----------------------------------------------------*/
class LeaderBoard {
public:
	LeaderBoard() : current_num_leaders(0) {}

	const Player* getLeaders() const { return leaders; }
	void readLeaderBoard();
	void printLeaderBoard(const Player leaders[]);
	void insertPlayer(Player &player);


private:
	static const int NUM_LEADERS = 5;
	Player leaders[NUM_LEADERS];
	int current_num_leaders;

};

const int LeaderBoard::NUM_LEADERS;
/*----------------------------------------------------
 LeaderBoard Member Functions
----------------------------------------------------*/

/*----------------------------------------------------
Prints the leader board if needed (not used)
----------------------------------------------------*/
void LeaderBoard::printLeaderBoard(const Player leaders[]) {
	for (int i = 0; i < current_num_leaders; i++) {
		cout << leaders[i].getPlayerName() << " "
				<< leaders[i].getNumberOfGuesses() << endl;
	}
}

/*-----------------------------------------------------------------
Reads Leader board file and copies objects of type Player to array
-------------------------------------------------------------------*/
void LeaderBoard::readLeaderBoard() {
	ifstream leader_file(LEADERBOARD_FILE);
	if (!leader_file) {
		ofstream new_file(LEADERBOARD_FILE);
		new_file.close();
		return;
	}

	string player;
	int guesses;
	int idx = 0;
	while (leader_file >> player >> guesses && idx < NUM_LEADERS) {
		leaders[idx++] = Player(player, guesses, true);
	}
	leader_file.close();
	current_num_leaders = idx;
}

/*-----------------------------------------------------------------
Inserts the player into the leader board
-------------------------------------------------------------------*/

void LeaderBoard::insertPlayer(Player &player) {
	readLeaderBoard();
	fstream file(LEADERBOARD_FILE, ios::in | ios::out);

	if (!file.is_open()) {
		cerr << "Unable to open file." << endl;
		return;
	}

	// Count the number of leaders
	int leaderCount = 0;
	string playerName;
	int numberOfGuesses;
	while (leaderCount < NUM_LEADERS
			&& file >> playerName >> numberOfGuesses) {
		leaders[leaderCount] = Player(playerName, numberOfGuesses);
		leaderCount++;
	}

	// Check if the user's score is less than someone on the leader board
	int insertIdx = leaderCount;
	for (int i = 0; i < leaderCount; i++) {
		if (player.getNumberOfGuesses() < leaders[i].getNumberOfGuesses()) {
			insertIdx = i;
			break;
		}
	}

	// Insert the new leader into the leader board array
	if (insertIdx < NUM_LEADERS) {
		for (int j = leaderCount - 1; j >= insertIdx; j--) {
			leaders[j + 1] = leaders[j];
		}
		leaders[insertIdx] = player;
		leaderCount = min(leaderCount + 1, NUM_LEADERS);

		if(leaderCount > NUM_LEADERS){
			leaderCount = NUM_LEADERS;
		}
	}

	// Write the leader board back to the file
	file.clear();
	file.seekp(0); // Write pointer is at top of file
	for (int i = 0; i < NUM_LEADERS && i < leaderCount; i++) {
		file << leaders[i].getPlayerName() << " "
				<< leaders[i].getNumberOfGuesses() << endl;
	}

	file.close();
}
/*----------------------------------------------------
 getNumber gets the number to guess for the game
----------------------------------------------------*/
int getNumber() {
	srand(static_cast<unsigned int>(time(NULL)));
	int numberToGuess = rand() % 91 + 10;
	double squareRoot = sqrt(numberToGuess);
	printf("%.8f is the square root of what number?\n", squareRoot);
	return numberToGuess;
}
/*----------------------------------------------------
 guessLogic takes users guesses and checks value
----------------------------------------------------*/
void guessLogic(int number, Player &currentPlayer) {
	int guess;
	cout << "Enter your guess: ";
	while (!(cin >> guess) || cin.get() != '\n') {
		cout << "Invalid input. Please enter an integer: " << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	while (guess != number) {
		currentPlayer.incrementGuessCount();
		if (guess < number) {
			cout << "Too low, guess again:" << endl;
		} else if (guess > number) {
			cout << "Too high, guess again:" << endl;
		}
		while (!(cin >> guess) || cin.get() != '\n') {
			cout << "Invalid input. Please enter an integer: " << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
	currentPlayer.incrementGuessCount();
	cout << "You got it, baby!" << endl;
	cout << "Your guesses: " << currentPlayer.getNumberOfGuesses() << endl;

}
/*--------------------------------------------------------------
PlayGuessingGame takes player input and calls needed functions
----------------------------------------------------------------*/
void playGuessingGame(LeaderBoard &leaderboard) {
	char playerInput;
	cout << "Welcome, press 'q' to quit or any other key to continue:" << endl;
	cin >> playerInput;
	while (playerInput != 'q') {
		leaderboard.readLeaderBoard();

		Player currentUser;

		currentUser.playerName();

		int numberToGuess = getNumber();
		guessLogic(numberToGuess, currentUser);
		leaderboard.insertPlayer(currentUser);

		cout << "Press 'q' to quit or any other key to continue:" << endl;
		cin >> playerInput;
	}
}

/*----------------------------------------------------
 Main
 ----------------------------------------------------*/
int main() {
	LeaderBoard leaderboard;
	playGuessingGame(leaderboard);
	cout << "bye bye!" << endl;

	return 0;
}
