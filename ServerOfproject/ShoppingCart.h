#pragma once
#include <string>
class ShoppingCart{
public:
	int type;//商品种类
	int id;//商品在当前种类里的id
	int num;//数量
	std::string name;
	ShoppingCart(std::string na="",int tt = 0, int ii = 0, int nn = 0) {
		name = na;
		type = tt;
		id = ii;
		num = nn;
	}
};

