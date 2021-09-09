#pragma once

#include <string>
#include<fstream>
#include<iostream>
#include "MyAddition.h"
class User {
protected:
	std::string name;//长度小于等于20且由字母数字组成
	std::string password;//长度大于等于8小于等于20
	int state;//1代表登录，0代表未登录
	double money;
	int id;
public:
	virtual int getUserType()=0;
	bool ModifyPassword(std::string s) {
		if (s.size() >= 8 && s.size() <= 20) {
			password = s;
			std::cout << "修改成功,密码共" << s.size() << "位" << std::endl;
			return 1;
		}else if (s.size() < 8) {
			std::cout << "密码过短,修改失败" << std::endl;
			return 0;
		}else {
			std::cout << "密码过长,修改失败" << std::endl;
			return 0;
		}
	}
	std::string getname() {
		return name;
	}
	bool login(std::string s) {
		if (s == password) {
			state = 1;
			std::cout << "登陆成功" << std::endl;
			return 1;
		}
		else {
			std::cout << "密码错误，请重操作" << std::endl;
			return 0;
		}
	}
	void logout() {
		state = 0;
	}
	std::string outname() {
		return name;
	}
	void OutData(std::ofstream &ofs) {
		ofs << name<< std::endl;
		ofs << password << std::endl;
		ofs << money << std::endl;
		ofs << id << std::endl;
		ofs << std::endl;
	}
	double GetMon() { return money; }
	void InMon(double mon) { money += mon; }
};

