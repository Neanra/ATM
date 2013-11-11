#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <String>

using namespace std;

//ATM (may be rewrite to singleton?)
class ATM
{
public: 
	ATM(
		bool avaliable = false,
		bool card_in = false,
		string strip_info = "",
		string pin_info = "",
		bool finished = false
		);

	//may be rewrite as public virtual?
	~ATM(){}; 

	//selectors - modificators
	bool& avaliable () { return _avaliable;}
	bool& card_in () { return _card_in;}
	string& strip_info () { return _strip_info;}
	string& pin_info () { return _pin_info;}
	bool& finished () { return _finished;}

private:
	bool _avaliable;
	bool _card_in;
	string _strip_info;
	string _pin_info;
	bool _finished;

};

#endif // ATM_H