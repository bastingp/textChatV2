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

vector<User> activeUsers;			//all users signed into the server

void ParseIncomingData(string message);


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
	
	//index; 
	int i = 0;
	//local incomingData
	IncomingData incomingData;
	
	//get the timecode
	//move past '$'
	
	for(int j = 0; j < 5 && i < message.size() && message[i] != '*'; j++)
	{
		while (i < message.size() && message[i] != '$' && message[i] != '|' && message[i] != '*')
		{
			if(j == 1)		//get timecode
			{
				incomingData.timecode += message[i];
			}
			else if(j == 2)		//get command
			{
				incomingData.command += message[i];
			}
			else if(j == 3)		//get username
			{
				incomingData.username += message[i];
			}
			else if(j == 4)		//get message or messageSize
			{
				//if the data is a message, store it in message
				if(incomingData.command == messageRequest)
				{
					incomingData.message += message[i];
				}
				else if(incomingData.command == updateRequest)	//if the data is messageSize, store it in userMessageSize
				{
					incomingData.userMessageSize += message[i];
				}
			}
			i++;
		}
		i++;
	}

	return incomingData;
}