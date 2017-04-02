#ifndef USER_H
#define USER_H

#include <string>

using namespace std; 

//enum userColor { #800000, #808000, #00FFFF, #008080, #FF00FF, 
		//#800080, #FFFF00, #FFFFFF, #FF0000, #00FF00 };
class User 
{

public: 

	//Constructor
	User(); 
	User(string t , string c, string n); 

	//Get the userTime
	string GetTime() const;
	 

	//Get userColor
	string GetColor() const; 

	//Get userName
	string GetUsername() const; 


private: 

	string timeCode;
	string color;
	string username; 

};

#endif /* USERID_H */
