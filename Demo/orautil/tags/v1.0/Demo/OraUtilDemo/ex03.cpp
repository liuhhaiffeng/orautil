// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Select()的使用

int main()
{
	try
	{
		//OracleUtil::OpenDB("数据库服务名", "用户名", "密码");
		OracleUtil::OpenDB("mybk106", "bsitsyx2", "bsitsyx2");
		cout << "打开数据库成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "打开数据库失败:" << ex.what() << endl;
		return 0;
	}

	if (OracleUtil::IsConnected())
	{
		cout << "数据库已连接" << endl;
	}
	else
	{
		cout << "数据库已断开" << endl;
	}

	cout << "序号\t" << "记录编号\t" << "号牌号码\t" << "经过时间\t" << "cwkc\t" << "设备标号\t" << endl;

	OracleUtil ora;
	try
	{
		ora.Select("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30");
	}
	catch (std::exception &ex)
	{
		cout << "查询出错:" << ex.what() << endl;
		return 0;
	}

	int idx = 0;
	while (ora.Next())
	{
		try
		{
			string jlbh = ora.Get<DTStr>(1);
			string hphm = ora.Get<DTStr>(2);
			ocilib::Date jgsj = ora.Get<DTDate>(3);
			double cwkc = ora.Get<DTDouble>(4);
			string sbbh = ora.Get<DTStr>(5);

			cout << "[" << ++idx << "]:\t" << jlbh << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-mm hh24:mi:ss") << ",\t"
				<< cwkc << ",\t" << sbbh << endl;
		}
		catch (std::exception &ex)
		{
			cout << "获取字段出错:" << ex.what() << endl;
			break;
		}
	}

	return 0;
}

