// Implementation of User class
#include <iostream>
#include <string>
#include <vector>
#include "USER_H.h"

//Constructor

User::User() 
{
    timeCode = "NULL"; 
    color = "#FFC1CC";
    username = "default_username";
	lastUpdateTime = 0;
}

User::User(string t, string c, string n) //initializes user;
{
    timeCode = t; 
    color = c;
    username = n;
	lastUpdateTime = 0;
}

User::User(string t , string c, string n, time_t ut)//initializes user
{
	timeCode = t; 
    color = c;
    username = n;
	lastUpdateTime = ut;
}

string User::GetTime() const //gets the timecode
{
    return (timeCode);
}

string User::GetUsername() const //gets the username
{
    return (username);
}

string User::GetColor() const //gets the color
{
    return (color);
}

time_t User::GetLastUpdateTime() const //gets the last update time (seconds in time_t)
{
	return (lastUpdateTime);
}

void User::SetLastUpdateTime(time_t newUpdateTime) //sets the last update time 
{
	lastUpdateTime = newUpdateTime; 
}
