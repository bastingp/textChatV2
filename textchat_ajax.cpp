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

string get_command(string message);

int main() 
{	
	Cgicc cgi;    // Ajax object
	char *cstr;
	const string unload_command = "UNLOAD";
	
	// Create AJAX objects to recieve information from web page.
	form_iterator user_input = cgi.getElement("message");
	
	  // create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	
	string message = **user_input;
	
	//Tell javascript how to read output
	cout << "Content-Type: text/plain\n\n";
	
	//Send message to server
	sendfifo.openwrite();
	sendfifo.send(message);
	
	//if page is unloading, no reason to wait for response from server
	if(get_command(message) == unload_command)
	{
		return 0;
	}
	
	//Get all messages from the server, and store in string
	recfifo.openread();
	string reply = "";
	string temp = recfifo.recv();
	while(temp.find("$END") > 0)
	{
		reply += "|" + temp;
		if(temp.find("$UPTODATE*") != string::npos)
		{
			break;
		}
		temp = recfifo.recv();
	}
	
	recfifo.fifoclose();
	cout << reply;
	
	sendfifo.fifoclose();
	
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
	//go past MESSAGE|
	while(message[index] != '|' && index < message.size())
	{
		command += message[index];
		index++;
	}
	
	return command;
}
