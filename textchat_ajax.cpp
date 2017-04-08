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

ofstream logfile;

// fifo for communication
string receive_fifo = "chatReply";
string send_fifo = "chatRequest";

string get_command(string message);

int main() 
{		
	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	//Send message to server
	
	sendfifo.openwrite();
	
	Cgicc cgi;    // Ajax object
	char *cstr;
	const string unload_command = "UNLOAD";
	
	// Create AJAX objects to recieve information from web page.
	form_iterator user_input = cgi.getElement("message");
	
	string message = **user_input;
	sendfifo.send(message);
	
	//Tell javascript how to read output
	cout << "Content-Type: text/plain\n\n";
	
	//if page is unloading, no reason to wait for response from server
	if(get_command(message) == unload_command)
	{		
		sendfifo.fifoclose();		
		return 0;
	}
	
	//Get all messages from the server, and store in string
	string reply = "";
	
	recfifo.openread();
	string temp = recfifo.recv();

	reply += "|" + temp;
	while(reply.find("$END") == string::npos)//searches for the $END message and combines the messages
	{
		temp = recfifo.recv();
		reply += "|" + temp;
	}
	
	recfifo.fifoclose();
	sendfifo.fifoclose(); //closes fifos
	
	reply += "*";
	cout << reply;	
	
	return 0;
}

string get_command(string message)//parser
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