// asppro::oracle.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 打开, 关闭数据库 演示

int main()
{
	OracleUtil con;
	try
	{
		con.OpenDB("mybk106", "test123", "test123");
		cout << "打开数据库成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "打开数据库失败:" << ex.what() << endl;
		return 0;
	}

	if (con.IsConnected())
	{
		cout << "数据库已连接" << endl;
	}
	else
	{
		cout << "数据库已断开" << endl;
	}

	// 可以不用显式调用, OracleUtil析构函数会自动调用
	/*con.CloseDB();
	cout << "关闭数据库" << endl;*/

	return 0;
}

