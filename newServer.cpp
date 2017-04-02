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
	while (i < message.size())
	{
		if (message[i] == '$')
		{
			break;
		}
		i++;
	}
	i++;
	
	while(i < message.size())
	{
		if(message[i] == '|')
		{
			break;
		}
		
		incomingData.timecode += message[i];
		i++; 
	}
	i++; 
	
	//figure out what's next; 
	
	//get command
	while(i < message.size()) 
	{
		if(message[i] == '|')
		{
			break;
		}
		
		incomingData.command += message[i];
		i++; 
	}
	i++; 
	
	//get everything else; 
	
	//get Username
	while(i < message.size() && message[i] != '*') 
	{
		if(message[i] == '|')
		{
			break;
		}
			
		incomingData.username += message[i];
		i++; 
	}
	i++; 


	//get Message
	while(i < message.size() && message[i] != '*') 
	{
		if(message[i] == '|')
		{
			break;
		}
		
		//if the data is a message, store it in message
		if(incomingData.command == messageRequest)
		{
			incomingData.message += message[i];
		}
		else if(incomingData.command == updateRequest)	//if the data is messageSize, store it in userMessageSize
		{
			incomingData.userMessageSize += message[i];
		}
		i++; 
	}
	i++; 
	
	return incomingData;
}
	
		
