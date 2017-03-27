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
	const string load_message = "$LOAD%*";
	const string unload_message = "$UNLOAD%";
	const string status_check_message = "$STATUS%*";
	string word;
	string message;
	string reply;
	string username = "";
	
	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);

	// // Call server to get results
	sendfifo.openwrite();
	sendfifo.send(load_message);

	/* Get a message from a server */
	//cout << "Content-Type: text/plain\n\n";
	recfifo.openread();
	reply = recfifo.recv();
	username = get_username(reply);
	cout << "\n\nYou've been assigned " << username << endl;
	
	recfifo.fifoclose();
	sendfifo.fifoclose();

	if(username == "FULL")
	{
		cout << "\n\nChat room is full. Try another time.\n";
		return 0;
	}
	
	while (1) 
	{
		int answer;
		do
		{
			cout << "\nMessage or update or leave or check status?   (1 or 2 or 3 or 4)\n";
			cin >> answer;
		}while(answer != 1 && answer != 2 && answer != 3 && answer != 4);
		
		cin.ignore();
		cin.clear();
		if(answer == 1)
		{
			message = "$MESSAGE%" + username + "%";
			cout << "Enter a message:";
			getline(cin, word);
			word += "*";
			message += word;
		}
		else if(answer == 2)
		{
			int size;
			message = "$UPDATE%" + username + "%";
			cout << "\nHow big do you have?    ";
			cin >> size;
			message += to_string(size);
			message += "*";
		}
		else if(answer == 3)
		{
			message = unload_message + username + "*";
			cout << "Send:" << message << endl;
			sendfifo.openwrite();
			sendfifo.send(message);
			sendfifo.fifoclose();
			break;
		}
		else
		{
			message = status_check_message;
		}

		cout << "Send:" << message << endl;
		sendfifo.openwrite();
		sendfifo.send(message);
	
		string all_replies;
		if(answer == 2)
		{
			cout << "\nGoing to open read fifo...\n";
			recfifo.openread();
			reply = recfifo.recv();
			while(reply.find("$END") > 0)
			{
				all_replies += "%" + reply;
				if(reply == "$UPTODATE*")
				{
					break;
				}
				reply = recfifo.recv();
			}
			recfifo.fifoclose();
			all_replies += "*";
			cout << all_replies;
		}
		else if(answer == 4)
		{
			cout << "\nGoing to open read fifo...\n";
			recfifo.openread();
			reply = recfifo.recv();
			recfifo.fifoclose();
			
			cout << reply;
		}
		
		sendfifo.fifoclose();
	}
	
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
	//go past USER%
	while(message[index] != '%')
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