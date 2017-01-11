// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <atlbase.h>
#include <atlstr.h>
#include <assert.h>
#include <fstream>
#include <array>
#include <stdio.h>
#include <vector>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Select()返回数据集合 的使用

#define local static

local bool CreateTable();
local bool InsertOneRow();
local int InsertRows();
local bool DropTable();
local void PrintRowDataFromDataset(unsigned int printrowcout = 10);

int main()
{
	try
	{
		//OracleUtil::OpenDB("数据库服务名", "用户名", "密码");
		OracleUtil::OpenDB("mybk106", "test123", "test123");
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

	DropTable();	
	CreateTable();
	InsertOneRow();
	int insertcount = InsertRows();
	if (insertcount > 0)
	{
		PrintRowDataFromDataset(insertcount);
	}

	OracleUtil ora;

	ora.ExecuteTransaction("update b_kkqp_jgcl set jlbh = '000'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set hphm = '测试'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set jgsj = sysdate");
	cout << endl;
	cout << "更新 jlbh, hphm, jgsj的值后查询结果" << endl;
	PrintRowDataFromDataset(insertcount);

	ora.Rollback();
	cout << endl;
	cout << "回滚之后的查询结果" << endl;
	PrintRowDataFromDataset(insertcount);

	ora.ExecuteTransaction("update b_kkqp_jgcl set jlbh = '000'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set hphm = '测试'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set jgsj = sysdate");
	ora.Commit();
	cout << endl;
	cout << "再次更新并提交后查询结果" << endl;
	PrintRowDataFromDataset(insertcount);

	getchar();

	return 0;
}

bool CreateTable()
{
	ifstream readFile("create_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "文件 create_t_kk_jgcl 打开失败" << endl;
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

	OracleUtil ora;

	bool created = false;
	try
	{
		ora.Execute(sql);
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

	OracleUtil ora;

	bool inserted = false;
	try
	{
		ora.Execute(insert1);
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

	OracleUtil ora;

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
			ora.Execute(sql);
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

void PrintRowDataFromDataset(unsigned int printrowcout /* = 1 */)
{
	OracleUtil ora;

	std::array<char, 256> szSql = {0};
	sprintf_s(szSql.data(), szSql.size(), "select jlbh, hphm ,jgsj from b_kkqp_jgcl where rownum <= %d", printrowcout);

	std::unique_ptr<Dataset> pdt = NULL;

	try
	{
		pdt = ora.Select2(string(szSql.data()));
	}
	catch (std::exception &ex)
	{
		cout << "查询数据出错:" << ex.what() << endl;
		return;
	}

	cout << "序号\t" << "记录编号\t" << "号牌号码\t" << "经过时间" << endl;

	int idx = 0;
	while (pdt->Next())
	{
		try
		{
			string jlbl = pdt->Get<DTStr>(1);
			string hphm = pdt->Get<DTStr>(2);
			ocilib::Date jgsj = pdt->Get<DTDate>(3);

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

	OracleUtil ora;

	bool droped = false;
	try
	{
		ora.Execute(sql);
		droped = true;
		cout << "删除数据库成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "删除表失败:" << ex.what() << endl;
	}

	return droped;
}



