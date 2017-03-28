#ifndef USERID_H
#define USERID_H

using namespace std; 

//enum userColor { #800000, #808000, #00FFFF, #008080, #FF00FF, 
		//#800080, #FFFF00, #FFFFFF, #FF0000, #00FF00 };
class User {

public: 


//Constructor
User(); 
User(userTime, userColor, userName); 

//Get the userTime
unsigned int getUserTime() const;

//set userTime

void setTime(int timeOfLoad); 

//Get userColor
string getUserColor() const; 

//set userColor

void setUserColor(string color);

//Get userName
string getUserName() const; 

//set username

void setUserName(string username);


private: 

userTime = timeOfLoad
userColor = color;
userName = username; 

};

#endif /* USERID_H */
