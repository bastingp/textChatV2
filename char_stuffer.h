#ifndef CHARSTUFFER_H
#define CHARSTUFFER_H

using namespace std;

#include <vector>
#include <string>

class CharStuffer
{
public:
	CharStuffer(char specialChar);			//adds specialChar to specialChars
	
	CharStuffer(char charOne, char charTwo);			//adds charOne and Two to specialChars
	
	CharStuffer(char charOne, char charTwo, char charThree);			//adds charOne, Two, and Three to specialChars

	CharStuffer(char _specialChars[]);		//adds all elements in _specialChars to specialChars
	
	CharStuffer(vector<char> _specialChars);		//adds all elements in _specialChars to specialChars
	
	string GetStringStuffed(string line);		//returns string with special chars stuff (i.e. "\hello" becomes "\\hello")
	
	string GetStringUnstuffed(string line);		//returns unstuffed string (i.e. "\\hello" becomes "\hello")
	
	vector<char> GetSpecialChars();				//for testing. returns specialChars
	
private:
	vector<char> specialChars;
	
	const char charNotifier = '\\';
};

#endif