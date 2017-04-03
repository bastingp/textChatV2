//Server created on April 2
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include "fifo.h"
#include "USER_H.h"

using namespace std;

//used to store data from cgi
struct IncomingData
{
	string timecode = "";
	string command = "";
	string username = "";
	string message = "";
	string userMessageSize = "";
	
	//resets all variables to empty strings
	void Reset()
	{
		timecode = "";
		command = "";
		username = "";
		message = "";
		userMessageSize = "";
	}
};

const int MAX_USERS = 10;
vector<User> activeUsers;			//all users signed into the server
vector<string> availableUsernames = {"StrangerBob", "StrangerSally", "StrangerPtolemy", "StrangerHelga", "StrangerAlex", 
									"StrangerThings", "StrangerLudwig", "StrangerToadstool", "StrangerJedediah", "StrangerYevgeni"};


void ParseIncomingData(string message);
void AssignUser(IncomingData data);
string GetFirstAvailableUsername();
bool DataIsCorrupt(IncomingData data);


int main()
{
	IncomingData incomingData;
	
	string message = "dummymessage";
	
	ParseIncomingData(incomingData, message);
	
	
	return 0;
}


IncomingData ParseIncomingData(string message)
{
	const string messageRequest = "MESSAGE";
	const string updateRequest = "UPDATE";
	const string corruptMessage = "CORRUPT";
	
	//index; 
	int i = 0;
	//local incomingData
	IncomingData incomingData;
	
	for(int j = 0; j < 5 && i < message.size() && message[i] != '*'; j++)
	{
		while (i < message.size() && message[i] != '$' && message[i] != '|' && message[i] != '*')
		{
			//get timecode
			if(j == 1)		
			{
				//Break if any characters are not numerical
				if(!isdigit(message[i]))
				{
					//if so, message is corrupt
					incomingData.timecode = corruptMessage;
					break;
				}
				incomingData.timecode += message[i];
			}
			
			//get command
			else if(j == 2)		
			{
				//Break if any characters are not uppercase letters
				if(!isalpha(message[i]) || islower(message[i]))
				{
					incomingData.command = corruptMessage;
					break;
				}
				incomingData.command += message[i];
			}
			
			//get username
			else if(j == 3)		
			{
				incomingData.username += message[i];
			}
			
			//get message or messageSize
			else if(j == 4)		
			{
				//if the data is a message, store it in message
				if(incomingData.command == messageRequest)
				{
					incomingData.message += message[i];
				}
				//if the data is messageSize, store it in userMessageSize
				else if(incomingData.command == updateRequest)	
				{
					//If message size is not a number, data is corrupt
					if(!isdigit(message[i]))
					{
						incomingData.userMessageSize = corruptMessage;
						break;
					}
					incomingData.userMessageSize += message[i];
				}
			}
			i++;
		}
		i++;
	}

	return incomingData;
}

void AssignUser(IncomingData data)
{
	if(activeUsers.size() < MAX_USERS)
	{
		string username = GetFirstAvailableUsername();

		//create new user with the data from client, and store them in activeUsers
		User newUser(data.timecode, "#000000", username);
		activeUsers.push_back(newUser);
	}
	else
	{
		cout << "\nCannot add user because chat room is full\n";
	}
}

string GetFirstAvailableUsername()
{
	string username;
	//find first username not yet taken by an active user
	for(int i = 0; i < availableUsernames.size(); i++)
	{
		bool usernameTaken = false;
		for(int j = 0; j < activeUsers.size(); j++)
		{
			//check if username is already taken
			if(activeUsers[j].GetUsername() == availableUsernames[i])
			{
				usernameTaken = true;
				break;
			}
		}
		//if it's not, go ahead and assign username to it, and break out of the loop
		if(!usernameTaken)
		{
			username = availableUsernames[i];
			break;
		}
	}
	
	return username;
}

bool DataIsCorrupt(IncomingData data)
{
	const string corruptMessage = "CORRUPT";
	return(data.timecode == corruptMessage || data.command == corruptMessage || data.userMessageSize == corruptMessage);
}