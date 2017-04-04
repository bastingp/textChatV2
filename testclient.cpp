#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "fifo.h"

using namespace std;
string receive_fifo = "chatReply";
string send_fifo = "chatRequest";

string get_username(string message);

int main() 
{
	string load_message = "$123456|LOAD|*";
	string unload_message = "$123456|UNLOAD|user1*";
	string status_check_message = "$STATUS%*";
	string word;
	string message;
	string reply;
	string username = "";
	
	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);

	/////ASSIGNMENT TEST/////////
	cout << "\n\nAbout to open send pipe\n\n";
	sendfifo.openwrite();
	cout << "\n\nSuccess opening send pipe\n\n";
	
	sendfifo.send(load_message);


	cout << "About to open receive pipe";
	recfifo.openread();
	reply = recfifo.recv();
	username = get_username(reply);
	cout << "received from server: "<< reply << endl;
	recfifo.fifoclose();
	sendfifo.fifoclose();

	cout << "asigned user:" << username <<endl;

	cout << "Continue? (Type anything):";
	cin.ignore();

	/////////UNASSIGNMENT TEST////////////////


	sendfifo.openwrite();
	sendfifo.send(unload_message);
	cout << "sent to server:" << unload_message << endl;

	sendfifo.fifoclose();
	
	return 0;
}

string get_username(string message)
{
	int index = 0;
	string str = "";
	
	//go past $
	while(message[index] != '$')
	{
		index++;
	}
	index++;
	//go past USER|
	while(message[index] != '|')
	{
		index++;
	}
	index++;

	//get username
	while(message[index] != '*')
	{
		str += message[index];
		index++;
	}
	
	return str;
}
