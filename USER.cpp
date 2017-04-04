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

User::User(string t , string c, string n, unsigned long int ut)
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

unsigned long int User::GetLastUpdateTime() const
{
	return (lastUpdateTime);
}

void User::SetLastUpdateTime(unsigned long int newUpdateTime)
{
	lastUpdateTime = newUpdateTime;
}
