#pragma once
#include "User.h"

#include <string>
#include<fstream>
#include<iostream>
class Buyer : public User {
public:
	Buyer(std::string ns = "", std::string ps = "", double mon = 0, int idd=0) {
		state = 0;
		name = ns; password = ps;
		money = mon;
		id = idd;
	}
	int getUserType() { return 1; }
};

