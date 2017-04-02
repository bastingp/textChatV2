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
	string timeCode = "";
	string command = "";
	string username = "";
	string message = "";
	string userMessageSize = "";
	
	//resets all variables to empty strings
	void Reset()
	{
		timeCode = "";
		command = "";
		username = "";
		message = "";
		userMessageSize = "";
	}
};

vector<User> activeUsers;			//all users signed into the server

void ParseIncomingData(IncomingData incomingData, string message);


int main()
{
	IncomingData incomingData;
	
	string message = "dummymessage";
	
	ParseIncomingData(incomingData, message);
	
	
	return 0;
}


ParseIncomingData(IncomingData incomingData, string message){
	int i = 0 //index; 
	
	//get the timecode
	while (i < message.size())
	{
		if (message[i] == '$')
		{
			break;
		}
		index++
	}
	index++
	
	while(i < message.size())
	{
		if(message[i] == '|')
			{
				break;
			}
			
			incomingData.timecode += message[i];
			index++; 
	}
	index++; 
	
	//figure out what's next; 
	
	if (message[i] == '$'){ //get command
	while(i < message.size()) 
	{
		if(message[i] == '|')
			{
				break;
			}
			
			incomingData.command += message[i];
			index++; 
	}
	index++; 
	}
	
	//get everything else; 
	
	//get Username
	while(i < message.size()) 
	{
		if(message[i] == '|')
			{
				break;
			}
			
			incomingData.username += message[i];
			index++; 
	}
	index++; 


	//get Message
	while(i < message.size()) 
	{
		if(message[i] == '|')
			{
				break;
			}
			
			incomingData.message += message[i];
			index++; 
	}
	index++; 
	
	
	//get Message size
	 
	while(i < message.size()) 
	{
		if(message[i] == '|')
			{
				break;
			}
			
			incomingData.messageSize += message[i];
			index++; 
	}
	index++; 
	}
	
		
