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
}

User::User(string t, string c, string n) 
{
    timeCode = t; 
    color = c;
    username = n;
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