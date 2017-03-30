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
#include "fifo.h"

using namespace std;
using namespace cgicc; // Needed for AJAX functions.

// fifo for communication
string receive_fifo = "chatReply";
string send_fifo = "chatRequest";

bool requires_receive(string message, bool& asked_to_update);
string get_command(string message);

int main() 
{	
	Cgicc cgi;    // Ajax object
	char *cstr;
	const string no_reply = "$END*";		//sent if message to server does not get a reply
	
	// Create AJAX objects to recieve information from web page.
	form_iterator user_input = cgi.getElement("message");
	
	  // create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	
	string message = **user_input;
	
	bool asked_to_update;
	bool need_to_listen = requires_receive(message, asked_to_update);
	
	cout << "Content-Type: text/plain\n\n";
	
	sendfifo.openwrite();
	sendfifo.send(message);
	if(need_to_listen)
	{		
		string reply = "";
		recfifo.openread();
		//update sends multiple strings, so deal with it differently
		if(asked_to_update)
		{
			string all_replies = "";
			reply = recfifo.recv();
			while(reply.find("$END") > 0)
			{
				all_replies += "|" + reply;
				if(reply.find("$UPTODATE*") != string::npos)
				{
					break;
				}
				reply = recfifo.recv();
			}
			
			reply = all_replies;
		}
		else
		{
			reply = recfifo.recv();
		}
		
		recfifo.fifoclose();
		cout << reply;
	}
	else
	{
		cout << no_reply;
	}
	
	sendfifo.fifoclose();
	
	return 0;
}

bool requires_receive(string message, bool& asked_to_update)
{
	const string load_command = "LOAD";
	const string update_command = "UPDATE";
	const string status_command = "STATUS";
	
	string command = get_command(message);

	if(command == update_command)
		asked_to_update = true;
	else
		asked_to_update = false;
	
	return (command == load_command || command == update_command || command == status_command);
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
	//go past MESSAGE|
	while(message[index] != '|' && index < message.size())
	{
		command += message[index];
		index++;
	}
	
	return command;
}
