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
string get_timecode(string message);

int main() 
{	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << "\n\nstarting CGI\n";
	logfile.close();
	
	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	//Send message to server
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << "Open sendfifo\n";
	logfile.close();
	
	sendfifo.openwrite();
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << "sendfifo opened\n";
	logfile.close();
	
	Cgicc cgi;    // Ajax object
	char *cstr;
	const string unload_command = "UNLOAD";
	
	// Create AJAX objects to recieve information from web page.
	form_iterator user_input = cgi.getElement("message");
	
	string message = **user_input;
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " sending message...\n";
	logfile.close();
	
	sendfifo.send(message);
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " message sent through sendfifo\n";
	logfile.close();
	
	//Tell javascript how to read output
	cout << "Content-Type: text/plain\n\n";
	
	//if page is unloading, no reason to wait for response from server
	if(get_command(message) == unload_command)
	{
		logfile.open("/tmp/g3log.log",ios::out|ios::app);
		logfile << get_timecode(message) << " Found command to be unload\n";
		logfile.close();
		
		sendfifo.fifoclose();
		
		logfile.open("/tmp/g3log.log",ios::out|ios::app);
		logfile << get_timecode(message) << " Exiting program\n";
		logfile.close();
		
		return 0;
	}
	
	//Get all messages from the server, and store in string
	string reply = "";
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " Opening read fifo...\n";
	logfile.close();
	
	recfifo.openread();
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " Readfifo opened\n";
	logfile.close();
	
	string temp = recfifo.recv();
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " Got message from server: " << temp << endl;
	logfile.close();

	reply += "|" + temp;
	while(reply.find("$END") == string::npos)//searches for the $END message and combines the messages
	{
		temp = recfifo.recv();
		reply += "|" + temp;
		
		logfile.open("/tmp/g3log.log",ios::out|ios::app);
		logfile << get_timecode(message) << " Got message from server: " << temp << endl;
		logfile.close();
	}
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " Out of loop. Closing fifos\n";
	logfile.close();
	
	recfifo.fifoclose();
	sendfifo.fifoclose(); //closes fifos
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " Fifos closed. About to cout\n";
	logfile.close();
	
	reply += "*";
	cout << reply;	
	
	logfile.open("/tmp/g3log.log",ios::out|ios::app);
	logfile << get_timecode(message) << " End program\n";
	logfile.close();
	
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

string get_timecode(string message)
{
	int index = 0;
	string timecode = "";
	
	//go past $
	while(message[index] != '$' && index < message.size())
	{
		index++;
	}
	index++;
	//go past TimeCode|
	while(message[index] != '|' && index < message.size() && message[index] != '*')
	{
		timecode += message[index];
		index++;
	}
	
	return timecode;
}
