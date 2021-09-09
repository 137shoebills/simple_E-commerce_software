// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <string>
#include<vector>
#include<WinSock2.h>
#include<Windows.h>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "MyAddition.h"
#include "Seller.h"
#include "Buyer.h"
#include "Clothes.h"
#include "Foods.h"
#include "Books.h"
#include "ShoppingCart.h"
const int PORT = 8000;
#define MaxClient 10
#define MaxBufSize 1024
int idm;
bool mymutex;//互斥锁
typedef struct State{//状态
	int id;
	int idtype;
}State;

typedef struct sendgoods {//用来传送商品数据的结构体
	double value;//价格
	int quantity;//存量
	int seller;
	char name[101];//名字
	char introduction[200];//介绍
}sendgoods;

typedef struct SendCart {//用来传送购物车的结构体
	int type;//商品种类
	int id;//商品在当前种类里的id
	int num;//数量
	char name[101];
}SendCart;

char RecvBuf[MaxBufSize] = {};
char SendBuf[MaxBufSize] = {};
//state是当前状态，-1为未登录，其他数字为登录者的编号
//idtype为登陆者的类型，buyer为1，seller为0
std::vector < Buyer > buyer;//buyer idtype 1
std::vector < Seller > seller;//seller idtype 0
std::vector < Books > bo;
std::vector < Foods > fo;
std::vector < Clothes > clo;

void InsertUsers() {//从文件中读入用户信息
	std::ifstream UFile("user_data.txt");
	int n1, n2;
	UFile >> n1 >> n2 >> idm;
	std::string name, password;
	double money;
	int id;
	char ch; UFile.get(ch);
	for (int i = 0; i < n1; ++i) {
		getline(UFile, name);
		getline(UFile, password);
		UFile >> money >> id;
		Buyer a(name, password, money, id);
		buyer.push_back(a);
		UFile.get(ch); UFile.get(ch);
	}
	for (int i = 0; i < n2; ++i) {
		getline(UFile, name);
		getline(UFile, password);
		UFile >> money >> id;
		Seller a(name, password, money, id);
		seller.push_back(a);
		UFile.get(ch); UFile.get(ch);
	}UFile.close();
}

void InsertGoods() {//从文件中读入商品信息
	std::ifstream GFile("goods_data.txt");
	int n1, n2, n3;
	GFile >> n1;
	int qua, sel;
	double val;
	char ch;
	std::string name, intro;
	for (int i = 0; i < n1; ++i) {
		GFile >> val >> qua >> sel;
		GFile.get(ch);
		getline(GFile, name);
		getline(GFile, intro);
		Books a(val, qua, sel, name, intro);
		bo.push_back(a);
	}
	GFile >> n2;
	for (int i = 0; i < n2; ++i) {
		GFile >> val >> qua >> sel;
		GFile.get(ch);
		getline(GFile, name);
		getline(GFile, intro);
		Clothes a(val, qua, sel, name, intro);
		clo.push_back(a);
	}
	GFile >> n3;
	for (int i = 0; i < n3; ++i) {
		GFile >> val >> qua >> sel;
		GFile.get(ch);
		getline(GFile, name);
		getline(GFile, intro);
		Foods a(val, qua, sel, name, intro);
		fo.push_back(a);
	}GFile.close();
	//std::cout << n1 <<' '<< n2<<' ' << n3 << std::endl;
}
void MyLogin(void* arg,State &state) {//登录
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state.id != -1) {
		std::cout << "已登录，请勿重复登陆" << std::endl;
		return;
	}
	std::string a; a.clear();
	int k = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
	if(k<=0) return;
	else {
		std::cout << RecvBuf << std::endl;
	}
	a = RecvBuf;
	int f = -1;
	for (int i = 0; i < buyer.size(); ++i) {
		if (buyer[i].outname() == a) {
			f = i; state.idtype = 1; break;
		}
	}
	if (f == -1) {
		for (int i = 0; i < seller.size(); ++i) {
			if (seller[i].outname() == a) {
				f = i; state.idtype = 0; break;
			}
		}
	}
	send(*ClientSocket, (char*)&f, sizeof(int), 0);
	if (f != -1) {
		int k = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
		if (k <= 0) { return; }
		a = RecvBuf;
		if (state.idtype == 0) {
			if (seller[f].login(a)) { state.id = f; }
		}
		else {
			if (buyer[f].login(a)) { state.id = f; }
		}
		send(*ClientSocket, (char*)&state, sizeof(int), 0);
	}
}
void MySignUp(void* arg) {//注册
	SOCKET* ClientSocket = (SOCKET*)arg;
	std::string a, b;
	int k;
	for (;;) {
		int fff = 0;
		k = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
		if (k <= 0) return;
		a = RecvBuf;
		for (int i = 0; i < buyer.size(); ++i) {
			if (buyer[i].getname() == a) {
				fff = 1; break;
			}
		}
		for (int i = 0; i < seller.size(); ++i) {
			if (seller[i].getname() == a) {
				fff = 1; break;
			}
		}
		k = send(*ClientSocket, (char*)&fff, sizeof(int), 0);
		if ( k <= 0 ) return;
		if ( fff == 0 ) break;
	}
	k = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
	if (k <= 0) return;
	b = RecvBuf;

	int t=0;
	k = recv(*ClientSocket, (char*)&t, sizeof(int), 0);
	if (k <= 0) return;
	if (t) {
		Buyer mb(a, b, 0, idm++);
		buyer.push_back(mb);
		std::cout << "注册买家成功" << std::endl;
	}
	else {
		Seller mb(a, b, 0, idm++);
		seller.push_back(mb);
		std::cout << "注册商家成功" << std::endl;
	}
}
void OutGoods() {//将商品信息输出到文件中
	//book clothes foods val qua sel name intro
	std::ofstream ofs("goods_data.txt", std::ios::out | std::ios::trunc);
	ofs << bo.size() << std::endl;
	for (int i = 0; i < bo.size(); ++i) {
		//name password money id;
		bo[i].OutData(ofs);
	}ofs << std::endl;
	ofs << clo.size() << std::endl;
	for (int i = 0; i < clo.size(); ++i) {
		//name password money id;
		clo[i].OutData(ofs);
	}ofs << std::endl;
	ofs << fo.size() << std::endl;
	for (int i = 0; i < fo.size(); ++i) {
		//name password money id;
		fo[i].OutData(ofs);
	}ofs << std::endl; 
	std::cout << "更新商品信息到文件中" << std::endl;
	ofs.close();
}
void OutUsers() {//将用户信息输出到文件中
	std::ofstream ofs("user_data.txt", std::ios::out | std::ios::trunc);
	ofs << buyer.size() << ' ' << seller.size() << ' ' << idm << std::endl;
	for (int i = 0; i < buyer.size(); ++i) {
		//name password money id;
		buyer[i].OutData(ofs);
	}
	for (int i = 0; i < seller.size(); ++i) {
		//name password money id;
		seller[i].OutData(ofs);
	}std::cout << "更新用户信息到文件中" << std::endl;
	ofs.close();
}
bool MakeCP(std::string a, std::string b) {//在b字符串中查找子序列a
	int** f;
	f = new int* [100];
	for (int i = 0; i < 100; ++i) {
		f[i] = new int[100];
		for (int j = 0; j < 100; ++j)f[i][j] = 0;
	}
	std::string aa = '0' + a;
	std::string bb = '0' + b;
	for (int i = 0; i <= b.size(); ++i) f[i][0] = 1;
	for (int i = 1; i < bb.size(); ++i) {
		for (int j = 1; j < aa.size(); ++j) {
			if (bb[i] == aa[j] && f[i - 1][j - 1]) {
				f[i][j] = 1;
			}
			if (f[i - 1][j])f[i][j] = 1;
		}
	}
	int xx = f[b.size()][a.size()];
	for (int i = 0; i < 100; ++i) {
		delete[] f[i];
	}
	delete[] f;
	return xx;
}
void ModPass(void* arg,State& state) {//修改密码
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state.id == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k;
	std::string s;
	k = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
	if (k <= 0) return;
	s = RecvBuf;
	int ff = 0;
	if (state.idtype == 0) {
		ff = seller[state.id].ModifyPassword(s);
		OutUsers();
	}
	if (state.idtype == 1) {
		ff = buyer[state.id].ModifyPassword(s);
		OutUsers();
	}
	k = send(*ClientSocket, (char*)&ff, sizeof(int), 0);
	if (k <= 0) return;
	if (ff) {
		std::cout << "修改成功" << std::endl;
	}
}
void LogOut(State& state) {
	if (state.id != -1) {
		if (state.idtype == 0) {
			seller[state.id].logout();
		}
		else {
			buyer[state.id].logout();
		}
	}
	state.id = -1;
	std::cout << "退出登录" << std::endl;
}
void ChangeMoney(void* arg,State& state) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state.id == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k;
	if (state.idtype == 0) {
		double val = seller[state.id].GetMon();
		k = send(*ClientSocket, (char*)&val, sizeof(double), 0);
		if (k <= 0) return;
		double zz = 0;
		k = recv(*ClientSocket, (char*)&zz, sizeof(double), 0);
		if (k <= 0) return;
		seller[state.id].InMon(zz);
		OutUsers();
		std::cout << "充值完成" << std::endl;
	}
	else if (state.idtype == 1) {
		double val = buyer[state.id].GetMon();
		k = send(*ClientSocket, (char*)&val, sizeof(double), 0);
		if (k <= 0) return;
		double zz = 0;
		k = recv(*ClientSocket, (char*)&zz, sizeof(double), 0);
		if (k <= 0) return;
		buyer[state.id].InMon(zz);
		OutUsers();
		std::cout << "充值完成" << std::endl;
	}
}

sendgoods GetSendGoods(double val, int qua,int sel, std::string name, std::string intro) {//将信息转化成sendgoods
	sendgoods aa{};
	aa.value = val; aa.quantity = qua; aa.seller = sel;
	for (int i = 0; i < name.size(); ++i) {
		aa.name[i] = name[i];
	}aa.name[name.size()] = '\0';
	//std::cout << aa.name << std::endl;
	for (int i = 0; i < intro.size(); ++i) {
		aa.introduction[i] = intro[i];
	}aa.introduction[intro.size()] = '\0'; 
	//std::cout << aa.introduction <<' '<<intro.size()<< std::endl;
	return aa;
}
void AddGoods(void* arg,State& state) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state.id < 0) {
		std::cout << "未登录" << std::endl;
		return;
	}
	if (state.idtype != 0) {
		std::cout << "不是商家" << std::endl;
		return;
	}
	int tt = 0, k = 0;
	k = recv(*ClientSocket, (char*)&tt, sizeof(int), 0);
	if (k <= 0) return;
	sendgoods aa;
	k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
	if (k <= 0) return;
	if (tt == 1) {
//double val = 0, int qua = 0, int id = 0, std::string na = "", std::string intro = ""
		Books zz(aa.value,aa.quantity,state.id,aa.name,aa.introduction);
		if (k <= 0) return;
		bo.push_back(zz);
	}
	else if (tt == 2) {
		Clothes zz(aa.value, aa.quantity, state.id, aa.name, aa.introduction);
		if (k <= 0) return;
		clo.push_back(zz);
	}
	else {
		Foods zz(aa.value, aa.quantity, state.id, aa.name, aa.introduction);
		if (k <= 0) return;
		fo.push_back(zz);
	}
	std::cout << "添加完成" << std::endl;
}
void PutInGoods(void* arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	int k, tt = bo.size();
	k = send(*ClientSocket, (char*)&tt, sizeof(int), 0);
	if (k <= 0) return; 
	//std::cout << tt << std::endl;
	for (int i = 0; i < bo.size(); ++i) {
		//std::cout << i << std::endl;
		sendgoods aa = GetSendGoods(bo[i].BackVal(), bo[i].BackQua(), bo[i].BackSeller(), bo[i].BackName(), bo[i].BackIntro());
		//std::cout << 1 << std::endl; 
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		//std::cout << sizeof(aa) << std::endl;
		if (k <= 0) return;
	}//std::cout << 2 << std::endl;
	tt = clo.size();
	k = send(*ClientSocket, (char*)&tt, sizeof(int), 0);
	for (int i = 0; i < clo.size(); ++i) {
		sendgoods aa = GetSendGoods(clo[i].BackVal(), clo[i].BackQua(), clo[i].BackSeller(), clo[i].BackName(), clo[i].BackIntro());
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0)return;
		//std::cout << std::endl;
	}//std::cout << 3 << std::endl;
	tt = fo.size();
	k = send(*ClientSocket, (char*)&tt, sizeof(int), 0);
	for (int i = 0; i < fo.size(); ++i) {
		sendgoods aa = GetSendGoods(fo[i].BackVal(), fo[i].BackQua(), fo[i].BackSeller(), fo[i].BackName(), fo[i].BackIntro());
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0)return;
		//std::cout << std::endl;
	}
	std::cout << "将商品信息传递至用户端" << std::endl;
}
void ShowGoods(void* arg) {
	PutInGoods(arg);
}
void SearchGoods(void* arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(arg);
}
void ChangeGoods(void* arg,State& state) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(arg);
	if (state.id < 0) {
		std::cout << "未登录" << std::endl;
		return;
	}
	if (state.idtype != 0) {
		std::cout << "请求者不是商家" << std::endl;
		return;
	}
	int tt, qua, f = 0;
	double val;
	std::string name, intro, a;
	int k = 0;
	k = recv(*ClientSocket, (char*)&tt, sizeof(tt), 0);
	if (k <= 0) return;
	k = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
	if (k <= 0) return;
	a = RecvBuf;
	k = recv(*ClientSocket, (char*)&val, sizeof(val), 0);
	if (k <= 0) return;
	k = recv(*ClientSocket, (char*)&qua, sizeof(qua), 0);
	if (k <= 0) return;
	if (tt == 1) {
		for (int i = 0; i < bo.size(); ++i) {
			if (a == bo[i].BackName()) {
				f = 1;
				if (bo[i].BackSeller() == state.id) {
					bo[i].ChangeVal(val);
					bo[i].ChangeQua(qua);
					std::cout << "已更新" << std::endl;
				}
				else std::cout << "不具备权限" << std::endl;
			}
		}
	}
	if (tt == 2) {
		for (int i = 0; i < clo.size(); ++i) {
			if (a == clo[i].BackName()) {
				f = 1;
				if (clo[i].BackSeller() == state.id) {
					clo[i].ChangeVal(val);
					clo[i].ChangeQua(qua);
					std::cout << "已更新" << std::endl;
				}
				else std::cout << "不具备权限" << std::endl;
			}
		}
	}
	if (tt == 3) {
		for (int i = 0; i < fo.size(); ++i) {
			if (a == fo[i].BackName()) {
				f = 1;
				if (fo[i].BackSeller() == state.id) {
					fo[i].ChangeVal(val);
					fo[i].ChangeQua(qua);
					std::cout << "已更新" << std::endl;
				}
				else std::cout << "不具备权限" << std::endl;
			}
		}
	}
	if (!f) std::cout << "名字查不到" << std::endl;
	else {
		OutGoods();
		std::cout << "更改完成" << std::endl;
	}
}
void InterFace() {
	std::string ch;
	std::ifstream InterFile("InterFace.txt");
	while (getline(InterFile, ch)) {
		std::cout << ch << std::endl;
	}
	std::cout << " 又鸽了咕 电商交易平台" << std::endl;
	std::cout << std::endl;
	std::cout << "Design by: 杨澜" << std::endl;
	std::cout << std::endl;
	std::cout << "输入 /help 查看命令行操作方式" << std::endl;
	std::cout << std::endl;
}
void AddCart(State &state,void* arg) {//添加入购物车
	if (state.id == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
}
void ShowCart(void *arg) {//展示购物车
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
}
void MakeForm(void* arg,State& state) {//将购物车转化为订单
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
	if (state.id == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k = 0;
	int ff = 0;
	double formval = 0;
	k = recv(*ClientSocket, (char*)&ff, sizeof(ff), 0);
	if (k <= 0)return;
	if (ff!=1){
		while (mymutex == 1) {
			Sleep(100);
		}mymutex = 1;
		std::vector<ShoppingCart> cart; cart.clear();
		int n;
		k = recv(*ClientSocket, (char*)&n, sizeof(n), 0);
		if (k <= 0)return;
		for (int i = 0; i < n; ++i) {
			SendCart aa;
			k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
			if (k <= 0)return;
			ShoppingCart bb(aa.name,aa.type,aa.id,aa.num);
			cart.push_back(bb);
		}
		int ww = 0;
		for (int i = 0; i < cart.size(); ++i) {
			if (cart[i].type == 1) {
				if (bo[cart[i].id].BackFrozen() == 1) {
					ww = i; ff = 1; break;
				}
				bo[cart[i].id].ChangeFrozen(1);
				formval += bo[cart[i].id].getPrice() * cart[i].num;
			}
			else if (cart[i].type == 2) {
				if (clo[cart[i].id].BackFrozen() == 1) {
					ww = i; ff = 1; break;
				}
				clo[cart[i].id].ChangeFrozen(1);
				formval += clo[cart[i].id].getPrice() * cart[i].num;
			}
			else {
				if (fo[cart[i].id].BackFrozen() == 1) {
					ww = i; ff = 1; break;
				}
				fo[cart[i].id].ChangeFrozen(1);
				formval += fo[cart[i].id].getPrice() * cart[i].num;
			}
		}
		if (ff) {
			for (int i = 0; i < ww; ++i) {
				if (cart[i].type == 1) {
					bo[cart[i].id].ChangeFrozen(0);
				}
				else if (cart[i].type == 2) {
					if (clo[cart[i].id].BackFrozen() == 1) {
						ww = i; ff = 1; break;
					}
					clo[cart[i].id].ChangeFrozen(0);
				}
				else {
					fo[cart[i].id].ChangeFrozen(0);
				}
			}
		}
		k = send(*ClientSocket, (char*)&ff, sizeof(ff), 0);
		if (k <= 0)return;
		mymutex = 0;
	}
	if (ff == 1) {
		std::cout << "生成订单失败， 当前有支付未完成 或 购物车中存在商品被冻结 。" << std::endl;
		return;
	}
	std::cout << "生成订单成功" << std::endl;
	std::cout << "订单需支付" << formval << "元" << std::endl;
}
void PayForm(void* arg,State &state) {//支付订单
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
	if (state.id == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k = 0;
	int n;
	k = recv(*ClientSocket, (char*)&n, sizeof(n), 0);
	if (k <= 0)return;
	if (n == 0) {
		std::cout << "该客户端无订单" << std::endl;
		return;
	}
	double formval = 0;
	k = recv(*ClientSocket, (char*)&formval, sizeof(double), 0);
	if (k <= 0)return;
	double mon = 0;
	if (state.idtype == 0) {
		mon = seller[state.id].GetMon();
		k = send(*ClientSocket, (char*)&mon, sizeof(mon), 0);
		if (k <= 0)return;
		if (seller[state.id].GetMon() < formval) {
			std::cout << "余额不足，支付失败" << std::endl;
			return;
		}
		else {
			seller[state.id].InMon(-formval);
		}
	}
	else if (state.idtype == 1) {
		mon = buyer[state.id].GetMon();
		k = send(*ClientSocket, (char*)&mon, sizeof(mon), 0);
		if (k <= 0)return;
		if (buyer[state.id].GetMon() < formval) {
			std::cout << "余额不足，支付失败" << std::endl;
			return;
		}
		else {
			buyer[state.id].InMon(-formval);
		}
	}//std::cout << n <<' '<< formval << std::endl;
	std::vector<ShoppingCart> oform; 
	oform.clear();
	for (int i = 0; i < n; ++i) {
		SendCart aa;
		k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0)return;
		//std::cout << aa.name << std::endl;
		ShoppingCart bb(aa.name, aa.type, aa.id, aa.num);
		oform.push_back(bb);
	}//std::cout << 1 << std::endl;
	for (int i = 0; i < oform.size(); ++i) {
		if (oform[i].type == 1) {
			bo[oform[i].id].ChangeFrozen(0);
			int bb = bo[oform[i].id].BackQua();
			bo[oform[i].id].ChangeQua(bb - oform[i].num);
			mon = bo[oform[i].id].getPrice() * oform[i].num;
			seller[bo[oform[i].id].BackSeller()].InMon(mon);
		}
		else if (oform[i].type == 2) {
			clo[oform[i].id].ChangeFrozen(0);
			int bb = clo[oform[i].id].BackQua();
			clo[oform[i].id].ChangeQua(bb - oform[i].num);
			mon = clo[oform[i].id].getPrice() * oform[i].num;
			seller[clo[oform[i].id].BackSeller()].InMon(mon);
		}
		else {
			fo[oform[i].id].ChangeFrozen(0);
			int bb = fo[oform[i].id].BackQua();
			fo[oform[i].id].ChangeQua(bb - oform[i].num);
			mon = fo[oform[i].id].getPrice() * oform[i].num;
			seller[fo[oform[i].id].BackSeller()].InMon(mon);
		}
	}
	std::cout << "支付完成" << std::endl;
}
void ClearForm(void* arg) {//清除订单
	SOCKET* ClientSocket = (SOCKET*)arg;
	int k = 0;
	int n;
	k = recv(*ClientSocket, (char*)&n, sizeof(n), 0);
	if (k <= 0)return; 
	std::vector<ShoppingCart> oform;
	oform.clear();
	for (int i = 0; i < n; ++i) {
		SendCart aa;
		k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0)return;
		//std::cout << aa.name << std::endl;
		ShoppingCart bb(aa.name, aa.type, aa.id, aa.num);
		oform.push_back(bb);
	}
	for (int i = 0; i < oform.size(); ++i) {
		if (oform[i].type == 1) {
			bo[oform[i].id].ChangeFrozen(0);
		}
		else if (oform[i].type == 2) {
			clo[oform[i].id].ChangeFrozen(0);
		}
		else {
			fo[oform[i].id].ChangeFrozen(0);
		}
	}oform.clear();
	std::cout << "清除订单" << std::endl;
}
DWORD WINAPI ServerThread(const LPVOID arg) {
	SOCKET* ClientSocket = (SOCKET*) arg;
	int receByt = 0;
	State state{ -1,0 };
	for (;;) {
		std::string cmd;
		receByt = recv(*ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
		if (receByt > 0) {
			std::cout << "从" << *ClientSocket << "接收到cmd: " << RecvBuf << std::endl;
			cmd = RecvBuf;
			memset(RecvBuf, 0, sizeof(RecvBuf));
		}
		else {
			std::cout << "与" << *ClientSocket << "接收消息结束，连接线程关闭！" << std::endl;
			break;
		}
		//std::cout << "请输入要发送到客户端" << *ClientSocket << "的信息(q退出):" << std::endl;
		//int k = 0;
		//k = send(*ClientSocket, s, strlen(s)*sizeof(char)+1, 0);
		if (cmd == "/login") {//登录
			MyLogin(ClientSocket, state);
			std::cout << std::endl;
		}
		else if (cmd == "/modpass") {//修改密码
			ModPass(ClientSocket, state);
			OutUsers();
			std::cout << std::endl;
		}
		else if (cmd == "/signup") {//注册
			MySignUp(ClientSocket);
			OutUsers();
			std::cout << std::endl;
		}
		else if (cmd == "/logout") {//登出
			ClearForm(ClientSocket);
			LogOut(state);
			std::cout << std::endl;
		}
		else if (cmd == "/money") {//查看当前钱数和充值
			ChangeMoney(ClientSocket, state);
			std::cout << std::endl;
		}
		else if (cmd == "/addgoods") {//添加商品
			AddGoods(ClientSocket, state);
			OutGoods();
			std::cout << std::endl;
		}
		else if (cmd == "/show") {//展示所有商品
			ShowGoods(ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/search") {//查找商品
			SearchGoods(ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/changegoods") {//改变商品数据
			ChangeGoods(ClientSocket, state);
			OutGoods();
			std::cout << std::endl;
		}
		else if (cmd == "/addincart") {
			AddCart(state, ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/showcart") {
			ShowCart(ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/makeform") {
			MakeForm(ClientSocket, state);
			std::cout << std::endl;
		}
		else if (cmd == "/payform") {
			PayForm(ClientSocket, state);
			OutGoods();
			OutUsers();
			std::cout << std::endl;
		}
		else if (cmd == "/clearform") {
			ClearForm(ClientSocket);
			std::cout << std::endl;
		}
	}
	std::cout << "与" << *ClientSocket << "之间线程连接关闭" << std::endl;
	closesocket(*ClientSocket);
	delete ClientSocket;
	return 0;
}
int main() {
	InsertUsers();
	InsertGoods();
	//InterFace();
	std::cout << "-------------服务器--------------" << std::endl;
	//初始化
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata); 
	//创建服务器监听套接字
	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == SOCKET_ERROR) {
		std::cout << "监听套接字创建失败！" << std::endl;
		return 0;
	}
	else { std::cout << "监听套接字创建成功！" << std::endl; }
	//绑定套接字
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1", &socketAddr.sin_addr.S_un.S_addr);
	socketAddr.sin_port = htons(PORT);
	int bRes = bind(ListenSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));
	if (bRes == SOCKET_ERROR) {
		std::cout << "绑定失败！" << std::endl;
		return 0;
	}
	else {
		std::cout << "绑定成功！" << std::endl;
	}
	//服务器监听
	int lLen = listen(ListenSocket, 20);//能存放20个客户端请求
	if (lLen == SOCKET_ERROR) {
		std::cout << "监听失败！" << std::endl;
		return 0;
	}
	else {
		std::cout << "监听成功！" << std::endl;
	}
	//接受请求
	for (;;) {
		SOCKET* ClientSocket=new SOCKET;
		*ClientSocket= accept(ListenSocket, 0, 0);
		if (*ClientSocket == INVALID_SOCKET) {
			std::cout << "服务端接受请求失败！" << std::endl;
			continue;
		}
		else {
			std::cout << "服务端接受请求成功" << std::endl;
		}
		CreateThread(NULL, 0, &ServerThread, ClientSocket, 0, NULL);
	}
	closesocket(ListenSocket);
	WSACleanup();
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
