#pragma once
#include <string>
#include<fstream>
#include<iostream>
#include "User.h"
class Seller : public User{
public:
	Seller(std::string ns = "", std::string ps = "", double mon = 0, int idd = 0) {
		state = 0;
		name = ns; password = ps;
		money = mon;
		id = idd;
	}
	int getUserType() { return 0; }
};

