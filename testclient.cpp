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
#include <time.h>
#include <istream>

using namespace std;
string receive_fifo = "chatReply";
string send_fifo = "chatRequest";

string get_username(string message);

int main() 
{
	clock_t t = clock();
	cout << t << endl << endl;
	string load_message = "$" + to_string(t) + "|LOAD|*";
	string unload_message = "$" + to_string(t) + "|UNLOAD|user1*";
	string status_check_message = "$STATUS%*";

	string end_messages = "$END";
	string uptodate = "$UPTODATE*";
	string word;
	string message;
	string reply;
	string username = "";
	string received_messages="0";
	
	
	
	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);

	while(1)
	{

		int testChoice;
		cout << "(1)Load, (2)Message, (3)Update, (4)Unload" << endl;
		cout <<"Action:";
		cin >> testChoice;

		switch(testChoice)
		{
			case 1: //Load test 
			{
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
			break;
			}

			case 2: //Message test
			{
				cout << "Message:"<< endl;
				//getline(cin, message);
				cin >> message;
				cout << "Number of messages received:";
				cin >> received_messages;


				cout << "\n\nAbout to open send pipe\n\n";
				sendfifo.openwrite();
				cout << "\n\nSuccess opening send pipe\n\n";
				
				string message_to_send = "$"+to_string(t)+"|MESSAGE|"+username+"|"+message+"|"+received_messages+"*";
				cout << "sending to server:"<<message_to_send<<endl;
				sendfifo.send(message_to_send);
				cout << "\n\nOpening receive pipe...\n\n";
				recfifo.openread();
				reply = recfifo.recv();
				cout << endl<< "Updating......." << endl;
				while(reply.find(end_messages) > 0)
				{
					cout << reply << endl;
					if(reply.find(uptodate) != string::npos)
					{
						break;
					}
					reply = recfifo.recv();
				}

				recfifo.fifoclose();
				sendfifo.fifoclose();
				break;
			}

			case 3: //Update test
			{
				sendfifo.openwrite();
				cout << "Number of messages received:";
				cin >> received_messages;
				string update_message = "$" + to_string(t) + "|UPDATE|"+username+"|"+received_messages+"*";
				cout << "sending:" << update_message;
				sendfifo.send(update_message);
				recfifo.openread();
				cout << "receiving update..." << endl;
				reply = recfifo.recv();
				while(reply.find(end_messages) > 0)
				{
					cout << reply << endl;
					if(reply.find(uptodate) != string::npos)
					{
						break;
					}
					reply = recfifo.recv();
				}

				recfifo.fifoclose();
				sendfifo.fifoclose();
				break;
			}

			case 4: //unload test
			{
				sendfifo.openwrite();
				unload_message = "$" + to_string(t) + "|UNLOAD|" + username +"*";
				sendfifo.send(unload_message);
				cout <<"sent to server:" << unload_message << endl;
				sendfifo.fifoclose();
				username="";
				break;
			}

		}

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
