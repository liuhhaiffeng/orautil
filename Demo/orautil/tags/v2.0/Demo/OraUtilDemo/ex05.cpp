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

// Execute(), Select()的使用

static bool DropTable();
static bool CreateTable();
static int InsertRows_NoCommit();
static void PrintRowData(unsigned int printrowcout = 10);
// 删除字符串尾部可能的分号";"及其后面内容
void TrimEndSemicolon(string& str);

// 采用RAII手法实现"自动初始化" 示例: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

static ASPConnection aspCon;

int main()
{
	try
	{
		aspCon.Open("mybk106", "test123", "test123");
		cout << "打开数据库成功\n";
	}
	catch (std::exception &ex)
	{
		cout << "打开数据库失败:" << ex.what() << '\n';
		return 0;
	}

	DropTable();	
	CreateTable();

	int insertCnt = InsertRows_NoCommit();
	if (insertCnt > 0)
	{
		cout << "\n\n插入 " << insertCnt << " 条数据, 未提交\n\n";
		cout << "打印插入的数据, *** 由于未提交, 打印内容应该为空 ***\n";
		PrintRowData(insertCnt);
	}
	else
	{ assert(false); }

	int insertCnt2 = InsertRows_NoCommit();
	if (insertCnt2 > 0)
	{
		aspCon.Commit();
		cout << "\n\n再次插入" << insertCnt2 << " 条数据, 并提交\n\n";
		cout << "打印提交后数据\n";
		PrintRowData(insertCnt);
	}
	else
	{ assert(false); }

	aspCon.Close();

	getchar();

	return 0;
}

bool DropTable()
{
	string sql = "drop table b_kkqp_jgcl";
	bool droped = false;
	try
	{
		aspCon.Execute(sql);
		droped = true;
		cout << "删除数据库成功\n";
		aspCon.Commit();
	}
	catch (std::exception &ex)
	{
		cout << "删除表失败:" << ex.what() << '\n';
	}

	return droped;
}

bool CreateTable()
{
	ifstream readFile("create_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "文件 create_b_kkqp_jgcl 打开失败\n";
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
		aspCon.Execute(sql);
		created = true;
		cout << "创建数据库表成功\n";
		aspCon.Commit();
	}
	catch (std::exception &ex)
	{
		cout << "创建数据库表失败:" << ex.what() << '\n'; 
	}

	return created;
}

// 删除字符串尾部可能的分号";"及其后面内容
void TrimEndSemicolon(string& str)
{
	string::size_type pos = str.find_last_of(";");
	if (pos != string::npos)
	{
		str = str.substr(0, pos);
	}
}

int InsertRows_NoCommit()
{
	ifstream readFile("insert_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "文件 insert_b_kkqp_jgcl 打开失败\n";
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
			string tmp = text.substr(pos, pos2-pos);
			TrimEndSemicolon(tmp);
			sqlList.push_back(tmp);

			text = text.substr(pos2);
		}
		else
		{
			string tmp = text;
			TrimEndSemicolon(tmp);
			sqlList.push_back(tmp);

			break;
		}

	} while (true);

	readFile.close();

	const int count = sqlList.size();
	if (count == 0)
	{
		return -1;
	}

	try
	{
		aspCon.Execute(sqlList);
		return sqlList.size();
	}
	catch (std::exception&)
	{
		return -1;
	}
}

void PrintRowData(unsigned int printrowcout /* = 1 */)
{
	std::array<char, 256> szSql = {0};
	sprintf_s(szSql.data(), szSql.size(), 
		"select jlbh, hphm ,jgsj from b_kkqp_jgcl where rownum <= %d", printrowcout);

	ASPConnection con;
	ASPDataset dt;
	try
	{
		con.Open("mybk106", "test123", "test123");
		dt = con.Select(string(szSql.data()));
	}
	catch (std::exception &ex)
	{
		cout << "查询数据出错:" << ex.what() << '\n';
		return;
	}

	cout << "序号\t记录编号\t号牌号码\t经过时间\n";

	int idx = 0;
	while (dt.Next())
	{
		try
		{
			string jlbl = dt.Get<string>(1);
			string hphm = dt.Get<string>(2);
			ASPDate jgsj = dt.Get<ASPDate>(3);

			cout << ++idx << ",\t" << jlbl << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';
		}
		catch (std::exception &ex)
		{
			cout << "获取字段错误:" << ex.what() << '\n';
			break;
		}
	}

	con.Close();
}





