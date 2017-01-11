/*!
 * \date 2016/12/01
 * \author 刘海峰
 * Contact: liuhaifeng@asppro.com.cn
 *
 * \brief asppro封装库实现hpp文件
*/
#pragma once

#include <assert.h>

namespace asppro {
namespace oracle {

	class OciEnvInit
	{
	public:
		OciEnvInit()
		{
			ocilib::Environment::Initialize(ocilib::Environment::Default | ocilib::Environment::Threaded);
			ocilib::Environment::EnableWarnings(true);
		}

		~OciEnvInit() { ocilib::Environment::Cleanup(); }
	};

	// 程序中示例用法: 
	// static OciEnvInit g_ociEnvInit;		// 利用RAII机制进行Oci环境初始化与注销

	inline OracleUtil::OracleUtil()
	{
	}

	inline OracleUtil::~OracleUtil()
	{
		CloseDB();
	}

	// 打开数据库
	inline void OracleUtil::OpenDB(const string& db, const string& user, const string& pwd)
	{
		m_Con.Open(db, user, pwd);
	}

	// 关闭数据库
	inline void OracleUtil::CloseDB() throw()
	{ 
		if (!m_Con.IsNull()) 
		{ 
			m_Con.Close(); 
		} 
	}

	inline bool OracleUtil::IsConnected()
	{
		if (!m_Con.IsNull())
		{
			bool bConnected = m_Con.IsServerAlive();
			return bConnected;
		}
		else
		{
			return false;
		}
	}

	inline std::unique_ptr<Dataset> OracleUtil::Select(const string& sql)
	{
		return std::unique_ptr<Dataset>(new Dataset(m_Con, sql));
	}

	template <typename DbType>
	inline DbType OracleUtil::SelectOne(const string& sql)
	{
		static_assert((is_same<DbType, DTShort>::value)
			|| (is_same<DbType, DTUShort>::value)
			|| (is_same<DbType, DTInt>::value)
			|| (is_same<DbType, DTUInt>::value)
			|| (is_same<DbType, DTStr>::value)
			|| (is_same<DbType, DTDate>::value)
			|| (is_same<DbType, DTFloat>::value)
			|| (is_same<DbType, DTDouble>::value
			|| (is_same<DbType, DTBigInt>::value))
			|| (is_same<DbType, DTBigUInt>::value)
			, "type only in DTShort, DTUShort, DTInt, DTUInt, "
			  "DTStr, DTDate, DTFloat, DTDouble, DTBigInt and DTBigUInt");

		ocilib::Statement st(m_Con);
		st.Execute(sql);
		ocilib::Resultset rs = st.GetResultset();
		rs.Next();
		DbType val = rs.Get<DbType>(1);
		return val;
	}

	template <>
	inline DTDate OracleUtil::SelectOne<DTDate>(const string& sql)
	{
		ocilib::Statement st(m_Con);
		st.Execute(sql);
		ocilib::Resultset rs = st.GetResultset();
		rs.Next();
		DTDate val = rs.Get<DTDate>(1).Clone();
		return val;
	}

	inline int OracleUtil::Execute(const string& sql)
	{
		ocilib::Statement st(m_Con);
		st.Execute(sql);
		int affectRows = st.GetAffectedRows();
		m_Con.Commit();
		return affectRows;
	}

	inline int OracleUtil::ExecuteTransaction(const string& sql)
	{
		ocilib::Statement st(m_Con);
		st.Execute(sql);
		int affectRow = st.GetAffectedRows();
		return affectRow;
	}

	inline void OracleUtil::Rollback() 
	{ 
		m_Con.Rollback(); 
	}

	inline void OracleUtil::Commit() 
	{ 
		m_Con.Commit(); 
	}

	inline string Time2Str(const time_t& time, const string& format /*= OTEXT("yyyy-mm-dd hh24:mi:ss")*/)
	{
		otext chTmp[100] = {0};
		struct tm mytm;
		localtime_s(&mytm, &time);
		mytm.tm_year = mytm.tm_year + 1900;
		mytm.tm_mon = mytm.tm_mon + 1;

		_snprintf_s(chTmp, _countof(chTmp), OTEXT("%04d-%02d-%02d %02d:%02d:%02d"),
			mytm.tm_year, mytm.tm_mon, mytm.tm_mday, mytm.tm_hour, mytm.tm_min, mytm.tm_sec);

		ocilib::Date date(string(chTmp), OTEXT("yyyy-mm-dd hh24:mi:ss"));
		return date.ToString(format);
	}

	inline time_t Str2Time(const string& timeStr, const string& format /*= OTEXT("yyyy-mm-dd hh24:mi:ss")*/)
	{
		ocilib::Date date(timeStr, format);

		struct tm mytm;
		time_t tt;
		mytm.tm_year = date.GetYear();
		mytm.tm_mon = date.GetMonth();
		mytm.tm_mday = date.GetDay();
		mytm.tm_hour = date.GetHours();
		mytm.tm_min = date.GetMinutes();
		mytm.tm_sec = date.GetSeconds();

		mytm.tm_year -= 1900;
		mytm.tm_mon--;
		mytm.tm_isdst = -1;

		tt = mktime(&mytm);

		return tt;
	}

	inline time_t Date2Time(const ocilib::Date& date)
	{
		const string fmt = "yyyy-mm-dd hh24:mi:ss";

		string timeStr = date.ToString(fmt);
		return Str2Time(timeStr, fmt);
	}

	inline ocilib::Date Time2Date(const time_t& time)
	{
		const string fmt = "yyyy-mm-dd hh24:mi:ss";

		string timeStr = Time2Str(time, fmt);
		return ocilib::Date(timeStr, fmt);
	}

} // namespace oracle
} // namespace asppro
