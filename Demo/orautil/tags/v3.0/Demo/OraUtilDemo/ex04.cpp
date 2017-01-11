// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Select的使用

// 采用RAII手法实现"自动初始化" 示例: 
static asp::OciEnvInit ociEnvInit;

int main()
{
	asp::AutoConnection con(new ocilib::Connection(), asp::ConnectionDeleter);

	try
	{
		con->Open("mybk106", "bsitsyx2", "bsitsyx2");
		std::cout << "打开数据库成功\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "打开数据库失败:" << ex.what() << '\n';
		return 0;
	}

	std::cout << "序号\t记录编号\t号牌号码\t经过时间\tcwkc\t设备标号\t\n";

	ocilib::Statement st(*con.get());
	std::unique_ptr<ocilib::Resultset> rs = NULL;
	try
	{
		st.Execute("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30");
		rs = std::unique_ptr<ocilib::Resultset>(new ocilib::Resultset(st.GetResultset()));
	}
	catch (std::exception &ex)
	{
		std::cout << "查询出错:" << ex.what() << '\n';
		return 0;
	}

	int idx = 0;
	while (rs->Next())
	{
		try
		{
			std::string jlbh = rs->Get<std::string>(1);
			std::string hphm = rs->Get<std::string>(2);
			ocilib::Date jgsj = rs->Get<ocilib::Date>(3);
			double cwkc = rs->Get<double>(4);
			std::string sbbh = rs->Get<std::string>(5);

			std::cout << "[" << ++idx << "]:\t" << jlbh << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-mm hh24:mi:ss") << ",\t"
				<< cwkc << ",\t" << sbbh << '\n';
		}
		catch (std::exception &ex)
		{
			std::cout << "获取字段出错:" << ex.what() << '\n';
			break;
		}
	}

	return 0;
}

