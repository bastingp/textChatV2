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

User::User(string t, string c, string n) 
{
    timeCode = t; 
    color = c;
    username = n;
	lastUpdateTime = 0;
}

User::User(string t , string c, string n, time_t ut)
{
	timeCode = t; 
    color = c;
    username = n;
	lastUpdateTime = ut;
}

string User::GetTime() const 
{
    return (timeCode);
}

string User::GetUsername() const 
{
    return (username);
}

string User::GetColor() const 
{
    return (color);
}

time_t User::GetLastUpdateTime() const
{
	return (lastUpdateTime);
}

void User::SetLastUpdateTime(time_t newUpdateTime)
{
	lastUpdateTime = newUpdateTime; 
}
