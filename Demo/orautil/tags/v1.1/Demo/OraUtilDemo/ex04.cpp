// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <array>
#include <stdio.h>
#include <vector>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Execute()的使用



static bool CreateTable();
static bool InsertOneRow();
static int InsertRows();
static bool DropTable();
static void PrintRowData(unsigned int printrowcout = 10);

static OracleUtil con;

int main()
{
	try
	{
		//con.OpenDB("数据库服务名", "用户名", "密码");
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

	DropTable();	
	CreateTable();
	InsertOneRow();
	int insertcount = InsertRows();
	if (insertcount > 0)
	{
		PrintRowData(insertcount);
	}

	getchar();

	return 0;
}

bool CreateTable()
{
	ifstream readFile("create_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "文件 create_b_kkqp_jgcl 打开失败" << endl;
		return false;
	}

	string sql;

	while (!readFile.eof())
	{
		std::array<char , 256> line = {0};
		readFile.getline(line.data(), line.size());
		sql += line.data();
	}

	readFile.close();

	bool created = false;
	try
	{
		con.Execute(sql);
		created = true;
		cout << "创建数据库表成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "创建数据库表失败:" << ex.what() << endl; 
	}

	return created;
}

bool InsertOneRow()
{
	 string insert1 = "insert into b_kkqp_jgcl (JLBH, HPHM, HPZL, JGSJ, CLSD, ZGXS, ZDXS, CWKC, HPYS, CLLX, JLLX, HDSJ, BZWZDM, SBBH, CDBH, FXBH, TPSL, CLBJ, SHBJ, ZFBJ, SJLY, CCLX, BZ, TJBJ, CSYS, JLLB, XSFX, CSBJ, YZYS, TPLX, XRSJ, URL) values ('000000000000000000000000000', '陕CT8283', 0, to_date('10-08-2016 00:03:53', 'dd-mm-yyyy hh24:mi:ss'), 22, 80, 80, 3.00, '0', '2', '0', 610300, '610300001045', '610300001045', 2, 'EW', 1, null, '0', '0', '127.0.0.1', 1, null, null, 0, 0, 1, 1, 1, 999, to_date('10-08-2016 00:44:40', 'dd-mm-yyyy hh24:mi:ss'), '/201605/25/08/12345678901-20160525083919-13-1.jpg;')";
	 bool inserted = false;
	 try
	 {
	 	con.Execute(insert1);
		inserted = true;
		cout << "插入一条数据成功" << endl;
	 }
	 catch (std::exception &ex)
	 {
		 cout << "插入一条数据失败:" << ex.what() << endl;
	 }

	 return inserted;
}

int InsertRows()
{
	ifstream readFile("insert_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "文件 insert_b_kkqp_jgcl 打开失败" << endl;
		return -1;
	}

	string text;
	while (!readFile.eof())
	{
		std::array<char , 1024> line = {0};
		readFile.getline(line.data(), line.size());
		text += line.data();
	}

	vector<string> sqlList;
	
	string::size_type pos = string::npos;
	string::size_type pos2 = string::npos;

	do 
	{
		pos = text.find("insert into");
		if (pos == string::npos)
		{
			break;
		}

		pos2 = text.find("insert into", pos+1);
		if (pos2 != string::npos)
		{
			sqlList.push_back(text.substr(pos, pos2-pos));
			text = text.substr(pos2);
		}
		else
		{
			sqlList.push_back(text);
			break;
		}

	} while (true);

	readFile.close();


	const int count = sqlList.size();
	if (count == 0)
	{
		cout << "读取到0条合法的sql语句, insert_b_kkqp_jgcl 文件为空或内容非法" << endl;
		return -1;
	}
	else
	{
		cout << "从 insert_b_kkqp_jgcl 中读取到 " << count << " 条sql 插入记录" << endl;
	}

	

	string sql;
	pos = string::npos;
	int insertedCount = 0;
	for (int i = 0; i < count; ++i)
	{
		sql = sqlList[i];
		pos = sql.find_last_of(";");
		if (pos != string::npos)
		{
			sql = sql.substr(0, pos);
		}

		try
		{
			con.Execute(sql);
			cout << "插入一条记录成功" << endl;
			++insertedCount;
		}
		catch (std::exception &ex)
		{
			cout << "插入一条记录失败:" << ex.what() << endl; 
			break;
		}
	}

	cout << "成功插入 " << insertedCount << " 条记录" << endl;

	return insertedCount;
}

void PrintRowData(unsigned int printrowcout /* = 1 */)
{
	std::array<char, 256> szSql = {0};
	sprintf_s(szSql.data(), szSql.size(), "select jlbh, hphm ,jgsj from b_kkqp_jgcl where rownum <= %d", printrowcout);

	std::unique_ptr<Dataset> dt = NULL;
	try
	{
		dt = con.Select(string(szSql.data()));
	}
	catch (std::exception &ex)
	{
		cout << "查询数据出错:" << ex.what() << endl;
		return;
	}

	cout << "序号\t" << "记录编号\t" << "号牌号码\t" << "经过时间" << endl;

	int idx = 0;
	while (dt->Next())
	{
		try
		{
			string jlbl = dt->Get<DTStr>(1);
			string hphm = dt->Get<DTStr>(2);
			ocilib::Date jgsj = dt->Get<DTDate>(3);

			cout << ++idx << ",\t" << jlbl << ",\t" << hphm << ",\t" << jgsj.ToString("yyyy-mm-dd hh24:mi:ss") << endl;
		}
		catch (std::exception &ex)
		{
			cout << "获取字段错误:" << ex.what() << endl;
			break;
		}
	}

}

bool DropTable()
{
	string sql = "drop table b_kkqp_jgcl";

	bool droped = false;
	try
	{
		con.Execute(sql);
		droped = true;
		cout << "删除数据库成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "删除表失败:" << ex.what() << endl;
	}

	return droped;
}



