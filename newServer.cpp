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
#include <sstream>
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
	string lastUpdateTime = "";
	
	//resets all variables to empty strings
	void Reset()
	{
		timecode = "";
		command = "";
		username = "";
		message = "";
		userMessageSize = "";
		lastUpdateTime =""; 
		
	}
};

string receive_fifo = "chatRequest";
string send_fifo = "chatReply";

const int MAX_WAIT = 10;	//amount of time that the server will allow a user to be absent before 
									//they're signed out 
const int MAX_USERS = 5;
vector<User> activeUsers;			//all users signed into the server
vector<string> availableUsernames = {"StrangerBob", "StrangerSally", "StrangerPtolemy", "StrangerHelga", "StrangerAlex", 
									"StrangerThings", "StrangerLudwig", "StrangerToadstool", "StrangerJedediah", "StrangerYevgeni"};
vector<string> storedMessages;


IncomingData GetMessageAsIncomingData(string message);		//parses message, stores its data into an IncomingData struct, and returns the data
void AssignUser(IncomingData data);					//adds user to activeUsers
string GetFirstAvailableUsername();					//returns first available username from availableUsernames
bool DataIsCorrupt(IncomingData data);				//returns true if any of the data in 'data' is corrupt
void UnassignUser(IncomingData incomingData);			//removes user with matching data in 'data' from activeUsers
void SendMessageThroughPipes(vector<string> messages, Fifo& sendfifo);			//sends messages through fifo pipes
void SendMessageThroughPipes(string message, Fifo& sendfifo);					//sends single message through fifo pipes
vector<string> GetUpdateMessages(IncomingData data);						//returns vector of messages user hasn't received yet, 
																			//or "$UPTODATE*" if they already have all the messages
void CheckForInactiveUsers(vector<User> users, IncomingData data); 

int main()
{	
	IncomingData incomingData;
	
	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	
	while(1)
	{
		cout << "Current number of users: " << to_string(activeUsers.size()) << endl << endl;
		//Open fifo, get message from client
		recfifo.openread(); 
		string inMessage = recfifo.recv(); 
		
		cout << "Got message: " << inMessage << endl; //test condition 
		
		incomingData = GetMessageAsIncomingData(inMessage);
		
		if(DataIsCorrupt(incomingData) == false)
		{
			cout << "Got command: " << incomingData.command << endl << endl;
			if(incomingData.command == "LOAD")
			{
				//if there's room on the server, add them to the chat room
				if(activeUsers.size() < MAX_USERS)
				{
					AssignUser(incomingData);
					string loadMessage = "$USER|" + activeUsers[activeUsers.size() - 1].GetUsername();
					SendMessageThroughPipes(loadMessage, sendfifo);
				}
				//otherwise, let them know the server is already full
				else
				{
					string chatRoomFullMessage = "$USER|FULL";
					SendMessageThroughPipes(chatRoomFullMessage, sendfifo);
				}
				
			}
			else if(incomingData.command == "UNLOAD")
			{
				UnassignUser(incomingData);
			}
			else if(incomingData.command == "UPDATE")
			{
				vector<string> updateMessages = GetUpdateMessages(incomingData);
				SendMessageThroughPipes(updateMessages, sendfifo);
			}
			else if(incomingData.command == "MESSAGE")
			{
				string messageFromUser = incomingData.username + "|" + incomingData.message;
				storedMessages.push_back(messageFromUser);
				vector<string> updateMessages = GetUpdateMessages(incomingData);
				SendMessageThroughPipes(updateMessages, sendfifo);
			}
			else
			{
				//command not recognized--handle this
				cout << "\n\n******Command not recognized******\n\n";
				SendMessageThroughPipes("$CORRUPT", sendfifo);
			}
		}
		else
		{
			//handle corrupt data
			cout << "\n\n******Data is corrupt******\n\n";
			SendMessageThroughPipes("$CORRUPT", sendfifo);
		}
		
		CheckForInactiveUsers(activeUsers, incomingData);
				
		//Close fifo
		recfifo.fifoclose();	
	}
	
	cout << "\n\n****out of while loop****";
	
	return 0;
}

void SendMessageThroughPipes(vector<string> messages, Fifo& sendfifo)
{
	sendfifo.openwrite();
	cout << "Sending: ";
	for(int i = 0; i < messages.size(); i++)
	{
		cout << messages[i] << endl;
		sendfifo.send(messages[i]);
	}
	sendfifo.send("$END");
	sendfifo.fifoclose();
}

void SendMessageThroughPipes(string message, Fifo& sendfifo)
{
	sendfifo.openwrite();
	
	cout << "Sending: " << message << endl;
	sendfifo.send(message);
	
	sendfifo.send("$END");

	sendfifo.fifoclose();
}


IncomingData GetMessageAsIncomingData(string message)
{
	const string messageRequest = "MESSAGE";
	const string updateRequest = "UPDATE";
	const string corruptMessage = "CORRUPT";
	
	//index; 
	int i = 0;
	//local incomingData
	IncomingData incomingData;
	
	for(int j = 0; j < 6 && i < message.size() && message[i] != '*'; j++)
	{
		while (i < message.size() && message[i] != '$' && message[i] != '|' && message[i] != '*')
		{
			//first time through, read past '$'
			
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
			
			//if message command, also store client's message size
			else if(j == 5)
			{
				//If message size is not a number, data is corrupt
				if(!isdigit(message[i]))
				{
					incomingData.userMessageSize = corruptMessage;
					break;
				}
				incomingData.userMessageSize += message[i];
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

void UnassignUser(IncomingData incomingData)
{
	for(int i = 0; i < activeUsers.size(); i++)
	{
		if(activeUsers[i].GetUsername() == incomingData.username || activeUsers[i].GetTime() == incomingData.timecode)
		{
			activeUsers.erase(activeUsers.begin()+i);
			break;
		}
	}
}

vector<string> GetUpdateMessages(IncomingData data)
{
	vector<string> updateMessages;
	stringstream numMessages;
	numMessages << storedMessages.size();
	
	if(data.userMessageSize == numMessages.str())
	{
		updateMessages.push_back("$UPTODATE");
	}
	else if(data.userMessageSize < numMessages.str())
	{
		updateMessages.push_back("$UPDATE");
		for(int i = atoi(data.userMessageSize.c_str()); i < storedMessages.size(); i++)
		{
			updateMessages.push_back(storedMessages[i]);
		}
	}
	else
	{
		//this should never happen
		cout << "\n\n*****ERROR: User has more messages than server*****\n\n";
		updateMessages.push_back("$UPTODATE");
	}
	
	return updateMessages;
}

void CheckForInactiveUsers(vector<User> users, IncomingData data){
time_t currentTime = time(NULL);
    for (int i = 0; i< users.size(); i++){
        
        if (currentTime - users.lastUpdateTime[i] >= MAX_WAIT)
        {
            unassign(users[i]);
        }
	if (user[i].timeCode = incomingData.timeCode)
	{
		user[i].lastUpdateTime[i] = currentTime; 
    	}
    }
        return; 
}
