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

	static OciEnvInit g_ociEnvInit;		// 利用RAII机制进行Oci环境初始化与注销

	// static m_Pool
	ocilib::Pool OracleUtil::m_Pool;

	OracleUtil::OracleUtil()
		: m_pRs(NULL)
	{
	}

	OracleUtil::~OracleUtil()
	{
		if (m_pRs)
		{
			delete m_pRs;
			m_pRs = NULL;
		}

		if (!m_ConSel.IsNull()) 
		{ 
			m_ConSel.Close(); 
		} 

		if (!m_ConTrac.IsNull())
		{
			m_ConTrac.Close();
		}
	}

	// 打开数据库
	void OracleUtil::OpenDB(const string& db, const string& user, 
		const string& pwd, unsigned int minSize /* = 1 */, 
		unsigned int maxSize /* = 2 */)
	{
		assert(minSize > 0 && maxSize > minSize);
		m_Pool.Open(db, user, pwd, ocilib::Pool::SessionPool, minSize, maxSize);
	}

	// 关闭数据库
	void OracleUtil::CloseDB() throw()
	{ 
		if (!m_Pool.IsNull()) 
		{ 
			m_Pool.Close(); 
		} 
	}

	bool OracleUtil::IsConnected() throw ()
	{
		if (!m_Pool.IsNull())
		{
			ocilib::Connection con  = m_Pool.GetConnection();
			bool bConnected = con.IsServerAlive();
			con.Close();
			return bConnected;
		}
		else
		{
			return false;
		}
	}

	void OracleUtil::Select(const string& sql)
	{
		CloseOpenConSel();
		m_ConSel = m_Pool.GetConnection();
		m_St = ocilib::Statement(m_ConSel);
		m_St.Execute(sql);

		if (m_pRs)
		{
			delete m_pRs;
			m_pRs = NULL;
		}
		m_pRs = new ocilib::Resultset(m_St.GetResultset());
	}

	// 数据库查询结果定位到下一行
	inline bool OracleUtil::Next() throw() 
	{ 
		return m_pRs->Next(); 
	}

	template <typename DbType>
	DbType OracleUtil::Get(int index)
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

		assert(index >= 1);

		if (m_pRs->IsNull())
		{
			throw std::exception("ocilib::Resultset is null");
		}

		return m_pRs->Get<DbType>(index);
	}

	template <>
	DTDate OracleUtil::Get<DTDate>(int index)
	{
		assert(index >= 1);

		if (m_pRs->IsNull())
		{
			throw std::exception("ocilib::Resultset is null");
		}

		return m_pRs->Get<DTDate>(index).Clone();
	}

	std::unique_ptr<Dataset> OracleUtil::Select2(const string& sql)
	{
		return std::unique_ptr<Dataset>(new Dataset(m_Pool.GetConnection(), sql));
	}

	template <typename DbType>
	DbType OracleUtil::SelectOne(const string& sql)
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

		ocilib::Connection con = m_Pool.GetConnection();
		ocilib::Statement st(con);
		st.Execute(sql);
		ocilib::Resultset rs = st.GetResultset();
		rs.Next();
		DbType val = rs.Get<DbType>(1);
		con.Close();
		return val;
	}

	template <>
	DTDate OracleUtil::SelectOne<DTDate>(const string& sql)
	{
		ocilib::Connection con = m_Pool.GetConnection();
		ocilib::Statement st(con);
		st.Execute(sql);
		ocilib::Resultset rs = st.GetResultset();
		rs.Next();
		DTDate val = rs.Get<DTDate>(1).Clone();
		con.Close();
		return val;
	}

	int OracleUtil::Execute(const string& sql)
	{
		ocilib::Connection con = m_Pool.GetConnection();
		ocilib::Statement st(con);

		st.Execute(sql);
		int affectRows = st.GetAffectedRows();
		con.Commit();
		con.Close();

		return affectRows;
	}

	int OracleUtil::ExecuteTransaction(const string& sql)
	{
		if (m_ConTrac.IsNull())
		{
			m_ConTrac = m_Pool.GetConnection();
		}
		ocilib::Statement st(m_ConTrac);
		st.Execute(sql);
		int affectRow = st.GetAffectedRows();
		return affectRow;
	}

	inline void OracleUtil::Rollback() 
	{ 
		m_ConTrac.Rollback(); 
	}

	inline void OracleUtil::Commit() 
	{ 
		m_ConTrac.Commit(); 
	}

	inline void OracleUtil::CloseOpenConSel() 
	{ 
		if (!m_ConSel.IsNull()) 
		{ 
			m_ConSel.Close(); 
		} 
	}

	inline void OracleUtil::CloseOpenConTrac()
	{
		if (!m_ConTrac.IsNull())
		{
			m_ConTrac.Close();
		}
	}

	string Time2Str(const time_t& time, const string& format /*= OTEXT("yyyy-mm-dd hh24:mi:ss")*/)
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

	time_t Str2Time(const string& timeStr, const string& format /*= OTEXT("yyyy-mm-dd hh24:mi:ss")*/)
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

	time_t Date2Time(const ocilib::Date& date)
	{
		const string fmt = "yyyy-mm-dd hh24:mi:ss";

		string timeStr = date.ToString(fmt);
		return Str2Time(timeStr, fmt);
	}

	ocilib::Date Time2Date(const time_t& time)
	{
		const string fmt = "yyyy-mm-dd hh24:mi:ss";

		string timeStr = Time2Str(time, fmt);
		return ocilib::Date(timeStr, fmt);
	}

} // namespace oracle
} // namespace asppro
