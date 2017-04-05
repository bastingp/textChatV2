#ifndef USER_H
#define USER_H

#include <string>

using namespace std; 

class User 
{

public: 

	//Constructor
	User(); 
	User(string t , string c, string n); 
	User(string t , string c, string n, time_t ut);

	//Get the userTime
	string GetTime() const;
	 

	//Get userColor
	string GetColor() const; 

	//Get userName
	string GetUsername() const; 
	
	//get lastUpdateTime
time_t GetLastUpdateTime() const; 

	void SetLastUpdateTime(time_t lastUpdateTime);
	//sets update time

private: 

	string timeCode;
	string color;
	string username; 
	time_t lastUpdateTime;

};

#endif /* USERID_H */
