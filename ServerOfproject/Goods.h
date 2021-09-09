#pragma once
#include <string>
#include<fstream>
#include<iostream>

extern double BooksOff;
extern double FoodsOff;
extern double ClothesOff;
class Goods {
protected:
	double value;//价格
	int quantity;//存量
	std::string name;//名字
	std::string introduction;//介绍
	int seller;//商户
	bool frozen;//商品是否冻结
public:
	virtual double getPrice() = 0;
	virtual void PrintData() = 0;
	void OutData(std::ofstream& ofs) {

		//val qua sel name intro
		ofs << value <<std::endl;
		ofs << quantity << std::endl;
		ofs << seller << std::endl;
		ofs << name << std::endl;
		ofs << introduction << std::endl;
		ofs << std::endl;
	}
	std::string BackName() {
		return name;
	}

	int BackQua() {
		return quantity;
	}
	double BackVal() {
		return value;
	}
	std::string BackIntro() {
		return introduction;
	}
	int BackFrozen() {
		return frozen;
	}
	void ChangeFrozen(int x) {
		frozen = x;
	}
	void ChangeVal(double val){
		value = val;
	}
	void ChangeQua(int qua) {
		quantity = qua;
	}
	int BackSeller() {
		return seller;
	}
};



