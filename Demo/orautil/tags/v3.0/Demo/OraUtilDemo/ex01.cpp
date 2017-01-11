// asppro::oracle.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 数据库环境初始化与注销
// 注: 任何操作数据库之前, 必须要"初始化数据库环境", 并且使用完后要"清除数据库环境分配资源"
//     类似于Com的初始化与注销, "数据库环境初始化与注销"是全局的, 一个应用程序中只做一次即可
static asp::OciEnvInit ociEnvInit;

int main()
{
	// 数据库相关操作
	// ...
	// ...

	// 如果ocilib库环境初始化成功, 则可从数据库查询出正确系统时间, 否则返回NullData, 触发异常
	ocilib::Connection con;
	try
	{
		con.Open("mybk106", "test123", "test123");
		std::cout << asp::SelectOne<ocilib::Date>(con, "select sysdate from dual").ToString("yyyy-mm-dd hh24:mi:ss") << '\n';
		std::cout << "ocilib库环境初始化成功\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "异常错误:" << ex.what() << '\n';
	}

	return 0;
}

