#include "ATM.h"

ATM::ATM (
		bool avaliable,
		bool card_in,
		string strip_info,
		string pin_info,
		bool finished
		):
	_avaliable (avaliable), 
	_card_in(card_in),
	_strip_info(strip_info),
	_pin_info(pin_info),
	_finished(finished)
	{

	}

