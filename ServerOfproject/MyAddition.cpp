#include "MyAddition.h"
#include<iostream>
double BooksOff = 1;
double FoodsOff = 1;
double ClothesOff = 1;
std::string GetS() {
	std::string a;
	getline(std::cin, a);
	while (a.size() > 100) {
		std::cout<<"字符串大小需小于100，请重新输入"<<std::endl;
		getline(std::cin, a);
	}
	return a;
}
int GetInt() {
	std::string a;
	getline(std::cin, a);
	int x,f,ff;
	int maxn = 99999999;
	while (1) {
		f = 1; x = 0; ff = 0;
		for (int i = 0; i < a.size(); ++i) {
			if (i == 0 && a[i] == '-') {
				f = -1; continue;
			}
			if (a[i] < '0' || a[i]>'9') {
				ff = 1; break;
			}
			x *= 10 ;
			x += a[i] - '0';
			if (x>maxn){
				std::cout << "请输入-99999999到99999999之间的整数" << std::endl;
				ff = 1; break;
			}
		}
		if (!ff)return x * f;
		std::cout << "输入不合法，请重新输入" << std::endl;
		getline(std::cin, a);
	}
}

double GetDouble() {
	std::string a;
	getline(std::cin, a);
	int f, ff,z;
	double x = 0,now=1;
	int maxn = 99999999;
	while (1) {
		f = 1; x = 0; now = 1;
		ff = 0; z = 0;
		for (int i = 0; i < a.size(); ++i) {
			if (i == 0 && a[i] == '-') {
				f = -1; continue;
			}
			if (a[i] == '.') {
				if (z == 1){
					ff = 1; break;
				}
				z = 1; continue;
			}
			if (a[i] < '0' || a[i]>'9') {
				ff = 1; break;
			}
			if (!z) {
				x *= 10;
				x += a[i] - '0';
			}
			else{
				now /= 10.0;
				x += now * double(a[i]-'0');
			}
			if (x > maxn) {
				std::cout << "请输入-99999999到99999999之间的数" << std::endl;
				ff = 1; break;
			}
		}
		if (!ff)return x * f;
		std::cout << "输入不合法，请重新输入" << std::endl;
		getline(std::cin, a);
	}
}