// asppro::oracle.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 数据库环境初始化与注销

// 采用RAII手法实现"自动初始化与注销" 示例: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

int main()
{
	// 数据库相关操作
	// ...
	// ...

	return 0;
}

