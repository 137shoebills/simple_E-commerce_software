#pragma once
#include "Goods.h"

#include <string>
#include<fstream>
#include<iostream>
class Books : public Goods {
public:
	double getPrice() {
		return value * BooksOff;
	}
	Books(double val = 0, int qua = 0, int id = 0, std::string na = "", std::string intro = "") {
		seller = id; value = val; quantity = qua;
		name = na; introduction = intro;
		frozen = 0;
	}
	void PrintData() {
		std::cout << "商品名：" << name << std::endl;
		std::cout << "介绍：" << introduction << std::endl;
		std::cout << "剩余数量：" << quantity << std::endl;
		std::cout << "价格：" << getPrice() << std::endl;
		std::cout << "类型：书本" << std::endl;
	}
};

