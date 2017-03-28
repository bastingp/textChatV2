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

using namespace std;

/* Fifo names */
string receive_fifo = "chatRequest";
string send_fifo = "chatReply";

const string user_assignment1 = "user1";
const string user_assignment2 = "user2";
const int MAX_UPDATE_DIFFERENCE = 15;
string user1TimeCode = "-1";
string user2TimeCode = "-1";
/*
 * 
 */
 string get_time_code(string message, int& index);
 string get_command(string message, int& index);
 bool is_valid_user(string incoming_time_code);
 void store_message(string message, int starting_index, vector<string>& user1messages, vector<string>& user2messages);
 void output_messages_through_pipes(vector<string> messages, Fifo& sendfifo);
 void handle_update(string message, int starting_index, vector<string> user1, vector<string> user2, int& user1Updates, int& user2Updates, Fifo& sendfifo);
 void check_timeout(bool& user1connected, bool& user2connected, int& user1Updates, int& user2Updates);
 vector<string> get_new_messages(int starting_index, vector<string> stored_messages);
 void assign_user(Fifo& sendfifo, bool& user1connected, bool& user2connected, int& user1Updates, int& user2Updates, string incoming_time_code);
 void unassign_user(string message, int starting_index, bool& user1connected, bool& user2connected, vector<string>& user1messages, vector<string>& user2messages);
 void return_status(Fifo& sendfifo, bool user1connected, bool user2connected);
 
int main() 
{
	bool user1connected = false;
	bool user2connected = false;
	vector<string> user1messages;
	vector<string> user2messages;
	const string message_command = "MESSAGE";
	const string update_command = "UPDATE";
	const string load_command = "LOAD";
	const string unload_command = "UNLOAD";
	const string status_command = "STATUS";
	
	string inMessage;


	int user1Updates = 0;
	int user2Updates = 0;

	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);

	while (1) 
	{
		/* Get a message from a client */
		recfifo.openread();
		inMessage = recfifo.recv();
		
		cout << "Got message: " << inMessage << endl;
		
		int i = 0;
		string incoming_time_code = get_time_code(inMessage, i);
		if (is_valid_user(incoming_time_code) == false)  //checks if user time code is valid
		{
			assign_user(sendfifo, user1connected, user2connected, user1Updates, user2Updates, incoming_time_code);
		}
		else
		{
			string command = get_command(inMessage, i);

			if(command == message_command)
			{
				store_message(inMessage, i, user1messages, user2messages);
			}
			else if(command == update_command)
			{
				handle_update(inMessage, i, user1messages, user2messages, user1Updates, user2Updates, sendfifo);
				check_timeout(user1connected, user2connected, user1Updates, user2Updates);
			}
			else if(command == load_command)
			{
				assign_user(sendfifo, user1connected, user2connected, user1Updates, user2Updates, incoming_time_code);
			}
			else if(command == unload_command)
			{
				unassign_user(inMessage, i, user1connected, user2connected, user1messages, user2messages);
			}
			else if(command == status_command)
			{
				return_status(sendfifo, user1connected, user2connected);
			}
		}
		
		recfifo.fifoclose();
	}

	return 0;
}

string get_time_code(string message, int& index)
{
	index = 0;
	string command = "";
	
	//go past $
	while(index < message.size())
	{
		if(message[index] == '$')
		{
			break;
		}
		index++;
	}
	index++;
	//go past TIMECODE|
	while(index < message.size())
	{
		if(message[index] == '|')
		{
			break;
		}
		command += message[index];
		index++;
	}
	index++;
	
	return command;
}

string get_command(string message, int& index)
{	
	string command = "";
	//go past MESSAGE|
	while(index < message.size())
	{
		if(message[index] == '|')
		{
			break;
		}
		command += message[index];
		index++;
	}
	index++;
	
	return command;
}

bool is_valid_user(string incoming_time_code)
{
	if (incoming_time_code == user1TimeCode || incoming_time_code == user2TimeCode || incoming_time_code == "-1")
	{
		return true;
	}
}

void store_message(string message, int starting_index, vector<string>& user1messages, vector<string>& user2messages)
{
	int i = starting_index;
	string username = "";
	
	//get username
	while(i < message.size())
	{
		if(message[i] == '|')
		{
			break;
		}
		username += message[i];
		i++;
	}
	i++;
	
	//get the message
	string str = "";
	while(i < message.size())
	{
		if(message[i] == '*')
		{
			break;
		}
		str += message[i];
		i++;
	}
	
	if(username == user_assignment1)
	{
		user1messages.push_back(str);
	}
	else
	{
		user2messages.push_back(str);
	}
}

void handle_update(string message, int starting_index, vector<string> user1, vector<string> user2, int& user1Updates, int& user2Updates, Fifo& sendfifo)
{
	int i = starting_index;
	string username = "";
	//get username
	while(i < message.size())
	{
		if(message[i] == '|')
		{
			break;
		}
		username += message[i];
		i++;
	}
	i++;
	
	string size_as_str = "";
	while(i < message.size())
	{
		if(message[i] == '*')
		{
			break;
		}
		size_as_str += message[i];
		i++;
	}
	
	//check size of client's messages
	int size = atoi(size_as_str.c_str());
	vector<string> reply;
	if(username == user_assignment1)
	{
		user1Updates++;
		//return other user's data
		if(size < user2.size())
		{
			reply = get_new_messages(size, user2);
			reply.push_back("$END");
		}
		else
		{
			reply.push_back("$UPTODATE*");
		}
	}
	else if(username == user_assignment2)
	{
		user2Updates++;
		//return other user's data
		if(size < user1.size())
		{
			reply = get_new_messages(size, user1);
			reply.push_back("$END");
		}
		else
		{
			reply.push_back("$UPTODATE*");
		}
	}	
	output_messages_through_pipes(reply, sendfifo);
}

void check_timeout(bool& user1connected, bool& user2connected, int& user1Updates, int& user2Updates)
 {
 	if (user1connected && user2connected)
	{
	 	if ((user1Updates - user2Updates) > MAX_UPDATE_DIFFERENCE)
 		{
 			user2connected = false;
 			cout << "user2 disconnected"<< endl;
 			user1Updates = 0;
 			user2Updates = 0;
 		}
 		if ((user2Updates - user1Updates) > MAX_UPDATE_DIFFERENCE)
 		{
 			user1connected = false;
 			cout << "user1 disconnected" << endl;
 			user1Updates = 0;
 			user2Updates = 0;
 		}
 	}
 }


vector<string> get_new_messages(int starting_index, vector<string> stored_messages)
{
	vector<string> new_messages;
	const string update_message = "$UPDATE";
	
	new_messages.push_back(update_message);
	for(int i = starting_index; i < stored_messages.size(); i++)
	{
		new_messages.push_back(stored_messages[i]);
	}
	
	return new_messages;
}

void output_messages_through_pipes(vector<string> messages, Fifo& sendfifo)
{
	sendfifo.openwrite();
	for(int i = 0; i < messages.size(); i++)
	{
		cout << messages[i] << endl;
		sendfifo.send(messages[i]);
	}
	sendfifo.fifoclose();
}

void assign_user(Fifo& sendfifo, bool& user1connected, bool& user2connected, int& user1Updates, int& user2Updates, string incoming_time_code)
{
	//use a vector because output_messages_through_pipes uses a vector parameter
	vector<string> message;
	string username = "$USER|";
	if(!user1connected)
	{
		username += "user1";
		user1connected = true;
		user2Updates = 0;
		user1TimeCode = incoming_time_code;

	}
	else if(!user2connected)
	{
		username += "user2";
		user2connected = true;
		user1Updates = 0;
		user2TimeCode = incoming_time_code;
	}
	else
	{
		username += "FULL";
	}
	username += "*";
	message.push_back(username);
	
	output_messages_through_pipes(message, sendfifo);
}

void unassign_user(string message, int starting_index, bool& user1connected, bool& user2connected, vector<string>& user1messages, vector<string>& user2messages)
{
	cout << "\n**************unassigning user*************\n";
	
	int i = starting_index;
	string username = "";
	//get username
	while(i < message.size())
	{
		if(message[i] == '*')
		{
			break;
		}
		username += message[i];
		i++;
	}
	
	if(username == user_assignment1)
	{
		user1connected = false;
		user1messages.clear();
		user1TimeCode = "-1";
	}
	else
	{
		user2connected = false;
		user2messages.clear();
		user2TimeCode = "-1";
	}
}

void return_status(Fifo& sendfifo, bool user1connected, bool user2connected)
{
	string message = "$STATUS|";
	if(user1connected && user2connected)
	{
		message += "FULL";
	}
	else
	{
		message += "WAIT";
	}
	message += "*";
	
	//store in vector so output_messages_through_pipes can handle properly
	vector<string> vstr;
	vstr.push_back(message);
	output_messages_through_pipes(vstr, sendfifo);
}
