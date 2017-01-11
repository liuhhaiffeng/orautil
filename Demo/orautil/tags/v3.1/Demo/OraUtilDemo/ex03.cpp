// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// SelectOne()的使用

// 采用RAII手法实现"自动初始化" 示例: 
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

	try
	{
		short short_max = asp::SelectOne<short>(con, "select 32767 from dual");
		assert(short_max == 32767);

		short short_min = asp::SelectOne<short>(con, "select -32767 from dual");
		assert(short_min == -32767);

		unsigned short ushort_max = asp::SelectOne<unsigned short>(con, "select 65535 from dual");
		assert(ushort_max == 65535);

		unsigned short ushort_min = asp::SelectOne<unsigned short>(con, "select 0 from dual");
		assert(ushort_min == 0);

		int int_max = asp::SelectOne<int>(con, "select 2147483647 from dual");
		assert(int_max == 2147483647);

		int int_min = asp::SelectOne<int>(con, "select -2147483647 from dual");
		assert(int_min == -2147483647);

		unsigned int uint_max = asp::SelectOne<unsigned int>(con, "select 4294967295 from dual");
		assert(uint_max == 4294967295);

		unsigned int uint_min = asp::SelectOne<unsigned int>(con, "select 0 from dual");
		assert(uint_min == 0);

		float myfloat = asp::SelectOne<float>(con, "select 3.14 from dual");
		assert(myfloat == 3.14f);

		double mydouble = asp::SelectOne<double>(con, "select 3.1415926 from dual");
		assert(mydouble == 3.1415926);

		std::string str = asp::SelectOne<std::string>(con, "select 'test std::string' from dual");
		assert(str == std::string("test std::string"));

		ocilib::Date curDate = asp::SelectOne<ocilib::Date>(con, "select sysdate from dual");
		std::cout << "DTDate curDate:" << curDate.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		// MaxVal(long long) == 9223372036854775807
		long long llmax = asp::SelectOne<long long>(con, "select 9223372036854775807 from dual");	
		assert(llmax == 9223372036854775807);

		long long llmin = asp::SelectOne<long long>(con, "select -9223372036854775807 from dual");
		assert(llmin == -9223372036854775807);

		// MaxVal(unsigned long long) = 18446744073709551615
		unsigned long long ullmax = asp::SelectOne<unsigned long long>(con, "select 18446744073709551615 from dual");	
		assert(ullmax == 18446744073709551615);

		unsigned long long ullmin = asp::SelectOne<unsigned long long>(con, "select 0 from dual");
		assert(ullmin == 0);

		std::cout << "单个值查询测试完成\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "测试出错:" << ex.what() << '\n';
	}

	con.Close();

	return 0;
}

