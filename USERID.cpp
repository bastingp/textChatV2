// Implementation of User class
#include <iostream>
#include <string>
#include <vector>
#include "USERID_H"

//Constructor

User::User() {
    timeCode = -1; 
    color = "#FFC1CC"
    username = "default_username"

User::User(string t, string c, string n) {
    timeCode = t; 
    color = c;
    username = n;
}

string User::getUserTime() const {
    
    return (timeCode);
}

string User::getUserName() const {
    return (username);
}

string User::getUserColor() const {
    return (color);
}
