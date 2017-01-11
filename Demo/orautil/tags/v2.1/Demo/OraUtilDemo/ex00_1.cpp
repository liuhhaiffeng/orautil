// asppro::oracle.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 数据库环境初始化与注销
// 注: 任何操作数据库之前, 必须要"初始化数据库环境", 并且使用完后要"清除数据库环境分配资源"
//     类似于Com的初始化与注销, "数据库环境初始化与注销"是全局的, 一个应用程序中只做一次即可

int main()
{
	ASPInitialize();

	// 数据库相关操作
	// ...
	// ...

	ASPCleanup();

	return 0;
}

