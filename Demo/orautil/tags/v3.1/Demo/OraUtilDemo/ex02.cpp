// asppro::oracle.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 打开, 关闭数据库 演示

static asp::OciEnvInit ociEnvInit;

int main()
{
	ocilib::Connection con;
	try
	{
		con.Open("mybk106", "test123", "test123");
		std::cout << "打开数据库成功\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "打开数据库失败:" << ex.what() << '\n';
		return 0;
	}

	if (asp::IsConnected(con))
	{
		std::cout << "数据库已连接\n";
	}
	else
	{
		std::cout << "数据库已断开\n";
	}

	std::cout << "数据库系统当前时间:" << asp::SelectOne<ocilib::Date>(con, "select sysdate from dual").ToString() << '\n';

	std::cout << "关闭数据库\n";

	con.Close();

	return 0;
}

