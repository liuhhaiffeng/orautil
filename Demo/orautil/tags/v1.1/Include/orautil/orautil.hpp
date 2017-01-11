/*!
 * \date 2016/12/01
 * \author 刘海峰
 * Contact: liuhaifeng@asppro.com.cn
 *
 * \brief 基于ocilib的二次封装库
*/

#pragma once

#include <string>
#include <memory>
#include "detail/ocilib/ocilib.hpp"
#include "detail/dataset.hpp"

namespace asppro {
namespace oracle {

	using std::string;

	typedef short					DTShort;
	typedef unsigned short			DTUShort;
	typedef int						DTInt;		// 对应数据库 Integer Type
	typedef unsigned int			DTUInt;
	typedef string					DTStr;		// 对应数据库 VCHAR, NVCHAR Type
	typedef ocilib::Date			DTDate;		// 对应数据库 Date Type
	typedef float					DTFloat;
	typedef double					DTDouble;	// 对应数据库 Float Type
	typedef long long				DTBigInt;	// 对应数据库 big_int
	typedef unsigned long long		DTBigUInt;	// 对应数据库 big_uint

	//
	// Oralce数据库操作封装类
	//
	// 多线程:
	//   此类支持多线程下的数据库操作
	//   在多线程下使用此类, 注意调用OpenDB(xx, minSize, maxSize)时, 根据多线程数量
	//   确定合理的minSize(至少打开的连接数), maxSize(最大能够打开的连接数), 能够提升
	//   多线程下数据库操作性能
	//
	// 数据库中断重连:
	//   此类支持数据库异常中断后的自动重连(因此, 不需要提供 ReConnect() 方法)
	//   示例:
		/*try
		{
			Select(sql);
		}
		catch (std::exception &ex)
		{
			switch (ex.GetType().GetValue() == "数据库连接中断")
			{
				cout << "数据库连接中断, 等待数据库重连成功..." << endl;
				// 等待数据库重连成功
				while (true)
				{
					Thread::Sleep(sometime);
					if (IsConnected())
					{
						cout << "数据库重连成功" << endl;
						break;
					}
				}
			}
		}*/
	//
	// 异常处理:
	//   示例:
		/*try
		{
			OpenDB();
			Select(sql);
			Execute(sql);
			ExecuteScalar(sql);
			Get<>(idx);
			Execute(sql);
			ExecuteTransaction(sql);
			RollBakc();
			Commit();
		}
		catch (Exception &ex)
		{
			switch (ex.GetType().GetValue())
			{
			case Exception::OracleError:
				cout << text("Oracle Error => ");
				break;
			case Exception::OracleWarning:
				cout << text("Oracle Error => ");
				break;
			case Exception::OcilibError:
				cout << text("OCILIB Error => ");
				break;
			default:
				cout << text("Unknown Error => ");
				break;
			}

			cout << ex.what() << endl;
		}
		catch (std::exception& ex)
		{
			cout << ex.what() << endl;
		}*/

	class OracleUtil
	{
	public:
		OracleUtil();
		~OracleUtil();

		// 打开数据库(并创建指定数量的连接)
		//	 db:		数据库服务名
		//   user:		用户名
		//   pwd:		密码
		//   minSize:	至少打开的连接数     assert(minSize > 0)
		//   maxSize:	最多可以打开的连接数 assert(maxSize > minSize)
		void OpenDB(const string& db, const string& user, const string& pwd);

		// 关闭数据库(及所有打开的连接)
		void CloseDB();

		// 数据库连接是否有效
		bool IsConnected();

		// 数据查询: 返回集合
		// 数据查询
		// 示例:
		/*
		std::unique_ptr<Dataset> dt = Select2("select str, int, date, double from xxx");
		while (dt->Next())
		{
			try
			{
				string str = dt->Get<DTStr>(1);
				int num = dt->Get<DTInt>(2);
				time_t date = dt->Get<DTDate>(3);
				double dbl = dt->Get<DTDouble>(4);
			}
			catch(std::exception& ex)
			{
				cout << "字段数据获取错误或数值类型转换错误:" << ex.what() << endl;
			}
		}*/
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		std::unique_ptr<Dataset> Select(const string& sql);
		// ----------------------------------------------------------------------

		// 查询单个值 (方便从数据库查询单一值)
		// 示例:
		// cout << "sysdate:" << ExecuteScalar<DTDate>("select sysdate from dual");
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		template <typename DbType>
		DbType SelectOne(const string& sql);
		template <>
		DTDate SelectOne<DTDate>(const string& sql);
		// ----------------------------------------------------------------------

		// 插入、删除、更新 (自动提交模式)
		// 返回值: 受影响的行数
		int Execute(const string& sql);

		// 插入、删除、更新 (事务模式)
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		int ExecuteTransaction(const string& sql);
		inline void Rollback();		// 回滚
		inline void Commit();		// 提交
		// -----------------------------------------------------------------------

	private:
		ocilib::Connection		m_Con;
	};

	// 工具函数
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// time_t 转 string
	string Time2Str(const time_t& time, const string& format = "yyyy-mm-dd hh24:mi:ss");

	// string 转 time_t
	time_t Str2Time(const string& timeStr, const string& format = "yyyy-mm-dd hh24:mi:ss");

	// ocilib::Date 转 time_t
	time_t Date2Time(const ocilib::Date& date);

	// time_t 转 ocilib::Date
	ocilib::Date Time2Date(const time_t& time);
	// -----------------------------------------------------------------------------
} // namesapce oracle
} // namespace asppro

#include "detail/orautil_iml.hpp"
