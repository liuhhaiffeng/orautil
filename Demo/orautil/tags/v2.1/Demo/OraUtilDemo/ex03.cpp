// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Select()的使用

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
		con.Open("mybk106", "bsitsyx2", "bsitsyx2");
		cout << "打开数据库成功" << '\n';
	}
	catch (std::exception &ex)
	{
		cout << "打开数据库失败:" << ex.what() << '\n';
		return 0;
	}

	cout << "序号\t记录编号\t号牌号码\t经过时间\tcwkc\t设备标号\t\n";

	ASPDataset dt;
	try
	{
		dt = con.Select("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30");
	}
	catch (std::exception &ex)
	{
		cout << "查询出错:" << ex.what() << '\n';
		return 0;
	}

	int idx = 0;
	while (dt.Next())
	{
		try
		{
			string jlbh = dt.Get<string>(1);
			string hphm = dt.Get<string>(2);
			ASPDate jgsj = dt.Get<ASPDate>(3);
			double cwkc = dt.Get<double>(4);
			string sbbh = dt.Get<string>(5);

			cout << "[" << ++idx << "]:\t" << jlbh << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-mm hh24:mi:ss") << ",\t"
				<< cwkc << ",\t" << sbbh << '\n';
		}
		catch (std::exception &ex)
		{
			cout << "获取字段出错:" << ex.what() << '\n';
			break;
		}
	}

	con.Close();

	return 0;
}

