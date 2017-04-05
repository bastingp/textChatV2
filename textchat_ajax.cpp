//comment

#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

//Stuff for pipes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include "fifo.h"

using namespace std;
using namespace cgicc; // Needed for AJAX functions.

// fifo for communication
string receive_fifo = "chatReply";
string send_fifo = "chatRequest";

string get_command(string message);
string get_number_of_messages(string message);

int main() 
{	
	Cgicc cgi;    // Ajax object
	char *cstr;
	const string unload_command = "UNLOAD";
	
	// Create AJAX objects to recieve information from web page.
	form_iterator user_input = cgi.getElement("message");
	
	string message = **user_input;
	
	//Tell javascript how to read output
	cout << "Content-Type: text/plain\n\n";
	
	string reply;
	//if page is unloading, no reason to wait for response from server
	if(get_command(message) == unload_command)
	{
		return 0;
	}
	else if(get_command(message) == "LOAD")
	{
		reply = "|$USER|StrangerBob*";
	}
	else if(get_command(message) == "MESSAGE")
	{
		reply = "$UPDATE|StrangerHelga|Hi! I'm Helga!|StrangerBob|This message should begin with 'You:'";
	}
	else if(get_command(message) == "UPDATE")
	{
		// if(stoi(get_number_of_messages(message)) <= 5)
		// {
			reply = "$UPDATE|StrangerHelga|Hi! I'm Helga!|StrangerBob|This message should begin with 'You:'";
		// }
		// else
		// {
			// reply = "$UPTODATE*";
		// }
	}
	
	cout << reply;
	
	return 0;
}

string get_command(string message)
{	
	int index = 0;
	string command = "";
	
	//go past $
	while(message[index] != '$' && index < message.size())
	{
		index++;
	}
	index++;
	//go past TimeCode|
	while(message[index] != '|' && index < message.size())
	{
		index++;
	}
	index++;
	//get command
	while(message[index] != '|' && index < message.size())
	{
		command += message[index];
		index++;
	}
	
	return command;
}

string get_number_of_messages(string message)
{
	int index = 0;
	
	//go past $
	while(message[index] != '$' && index < message.size())
	{
		index++;
	}
	index++;
	//go past TimeCode|
	while(message[index] != '|' && index < message.size())
	{
		index++;
	}
	index++;
	//go past MESSAGE|
	while(message[index] != '|' && index < message.size())
	{
		index++;
	}
	index++;
	//go past username
	while(message[index] != '|' && index < message.size())
	{
		index++;
	}
	index++;
	//go past message
	while(message[index] != '|' && index < message.size())
	{
		index++;
	}
	index++;
	//get message size
	string message_size = "";
	while(message[index] != '*' && index < message.size())
	{
		message_size += message[index];
		index++;
	}
	
	return message_size;
}
