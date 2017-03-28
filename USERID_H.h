#ifndef USERID_H
#define USERID_H

using namespace std; 

//enum userColor { #800000, #808000, #00FFFF, #008080, #FF00FF, 
		//#800080, #FFFF00, #FFFFFF, #FF0000, #00FF00 };
class User {

public: 


//Constructor
User(); 
User(string t , string c, string n); 

//Get the userTime
string getUserTime() const;
 

//Get userColor
string getUserColor() const; 

//Get userName
string getUserName() const; 

//set username

void setUserName(string username);


private: 

string timeCode;
string color;
string username; 

};

#endif /* USERID_H */
