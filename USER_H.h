#ifndef USERID_H
#define USERID_H

using namespace std; 

class userID {

public: 

//Constructor
userID(); 
userID(userTime, userColor, userName); 

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
