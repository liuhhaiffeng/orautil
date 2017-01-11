// asppro::oracle.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 打开, 关闭数据库 演示

// 采用RAII手法实现"自动初始化" 示例: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

int main()
{
	ASPConnection con;
	try
	{
		con.Open("mybk106", "test123", "test123");
		cout << "打开数据库成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "打开数据库失败:" << ex.what() << endl;
		return 0;
	}

	if (con.IsConnected())
	{
		cout << "数据库已连接\n";
	}
	else
	{
		cout << "数据库已断开\n";
	}

	cout << "数据库系统当前时间:" << con.SelectOne<ASPDate>("select sysdate from dual").ToString() << '\n';

	con.Close();		// 可以不用显式调用, ASPConnection析构函数会自动调用
	cout << "关闭数据库" << endl;

	return 0;
}

