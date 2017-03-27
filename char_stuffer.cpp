#include "char_stuffer.h"
#include <iostream> //for debugging
#include <sstream>		//for printing ints

using namespace std;

//adds specialChar to specialChars
CharStuffer::CharStuffer(char specialChar)
{
	specialChars.push_back(specialChar);
}			

//adds charOne and Two to specialChars
CharStuffer::CharStuffer(char charOne, char charTwo)
{
	specialChars.push_back(charOne);
	specialChars.push_back(charTwo);
}

//adds charOne, Two, and Three to specialChars
CharStuffer::CharStuffer(char charOne, char charTwo, char charThree)
{
	specialChars.push_back(charOne);
	specialChars.push_back(charTwo);
	specialChars.push_back(charThree);
}

//adds all elements in _specialChars to specialChars
CharStuffer::CharStuffer(char _specialChars[])
{
	for(int i = 0; i < sizeof(&_specialChars); i++)
	{
		specialChars.push_back(specialChars[i]);
	}
}

//adds all elements in _specialChars to specialChars
CharStuffer::CharStuffer(vector<char> _specialChars)
{
	for(int i = 0; i < _specialChars.size(); i++)
	{
		specialChars.push_back(specialChars[i]);
	}
}

//returns string with special chars stuff (i.e. "\hello" becomes "\\hello")
string CharStuffer::GetStringStuffed(string line)
{
	for(int i = 0; i < line.size(); i++)		//go through every character in line
	{
		char c = line[i];
		for(int j = 0; j < specialChars.size(); j++)	//compare each char c to every specialChar
		{
			if(c == specialChars[j])		//if c == a special char...
			{
				line.insert(line.begin() + i, c);			//stuff the char into the line so it's doubled
				i++;				//increment i, so that it doesn't loop endlessly on the same character
				break;
			}
		}
	}
	
	return line;
}	

//returns unstuffed string (i.e. "\\hello" becomes "\hello")
string CharStuffer::GetStringUnstuffed(string line)
{	
	for(int i = 0; i < line.size() - 1; i++)		//go through every character in line, except for the last character (because it can't be doubled afterwards)
	{
		char c = line[i];
		for(int j = 0; j < specialChars.size(); j++)	//compare each char c to every specialChar
		{
			if(c == specialChars[j])		//if c == a special char...
			{
				if(line[i + 1] == c)
				{
					line.erase(line.begin() + i);			//delete the doubled line
				}
				break;
			}
		}
	}

	return line;
}

vector<char> CharStuffer::GetSpecialChars()
{
	return specialChars;
}