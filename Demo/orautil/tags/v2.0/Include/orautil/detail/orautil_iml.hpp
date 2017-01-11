namespace asppro {
namespace oracle {

// ****************************************************************************** 
// 封装库环境初始化与注销
// 注: 任何操作数据库之前, 必须要"初始化数据库环境", 并且使用完后要"清除数据库环境分配资源"
//     类似于Com的初始化与注销, "数据库环境初始化与注销"是全局的, 一个应用程序中只做一次即可
// ****************************************************************************** 
inline void  ASPInitialize()
{
	ocilib::Environment::Initialize(ocilib::Environment::Default | ocilib::Environment::Threaded);
	ocilib::Environment::EnableWarnings(true);
}

inline void  ASPCleanup()
{
	ocilib::Environment::Cleanup();
}


// ****************************************************************************** 
// 封装库日期类
// ******************************************************************************
ASPDate::ASPDate(NullDateType dt /*= NullDate*/)
	: m_tt(-1)
{

}

inline ASPDate::ASPDate(const ASPDate& dt)
	: m_tt(dt.m_tt)
{
}

inline ASPDate::ASPDate(const std::string& str, const std::string& format /*= "yyyy-mm-dd hh24:mi:ss"*/)
	: m_tt(ASPDate::Str2Time(str, format))
{
}

inline ASPDate::ASPDate(const time_t& tt)
	: m_tt(tt)
{
}

inline ASPDate::~ASPDate(void)
{
}

inline const std::string& ASPDate::DefFmt()
{
	static const std::string fmt = "yyyy-mm-dd hh24:mi:ss";
	return fmt;
}

inline ASPDate ASPDate::SysDate()
{
	return ASPDate(ASPDate::Time2Str(time(NULL), ASPDate::DefFmt()), ASPDate::DefFmt());
}

inline bool ASPDate::IsNull() const
{
	return (m_tt < 0);
}

inline void ASPDate::FromString(const std::string& str, const std::string& format /*= "yyyy-mm-dd hh24:mi:ss"*/)
{
	m_tt = ASPDate::Str2Time(str, format);
}

inline void ASPDate::FromTimet(const time_t& tt)
{
	m_tt = tt;
}

inline std::string ASPDate::ToString(const std::string& format /*= "yyyy-mm-dd hh24:mi:ss"*/) const
{
	return ASPDate::Time2Str(m_tt, format);
}

inline time_t ASPDate::GetTimet() const
{
	return m_tt;
}

inline ASPDate& ASPDate::operator=(const ASPDate& dt)
{
	m_tt = dt.m_tt;
	return *this;
}

inline ASPDate& ASPDate::operator++()
{
	++m_tt;
	return *this;
}

inline ASPDate ASPDate::operator++(int)
{
	ASPDate tmp(m_tt);
	++m_tt;
	return tmp;
}

inline ASPDate& ASPDate::operator--()
{
	--m_tt;
	return *this;
}

inline ASPDate ASPDate::operator--(int)
{
	ASPDate tmp(m_tt);
	--m_tt;
	return tmp;
}

inline ASPDate ASPDate::operator+(int seconds)
{
	return ASPDate(m_tt + seconds);
}

inline ASPDate ASPDate::operator-(int seconds)
{
	return ASPDate(m_tt - seconds);
}

inline ASPDate& ASPDate::operator+=(int seconds)
{
	m_tt += seconds;
	return *this;
}

inline ASPDate& ASPDate::operator-=(int seconds)
{
	m_tt -= seconds;
	return *this;
}

inline bool ASPDate::operator== (const ASPDate& dt) const 
{
	return (m_tt == dt.m_tt);
}

inline bool ASPDate::operator!= (const ASPDate& dt) const
{
	return (m_tt != dt.m_tt);
}

inline bool ASPDate::operator> (const ASPDate& dt) const
{
	return (m_tt > dt.m_tt);
}

inline bool ASPDate::operator< (const ASPDate& dt) const
{
	return (m_tt < dt.m_tt);
}

inline bool ASPDate::operator>= (const ASPDate& dt) const
{
	return (m_tt >= dt.m_tt);
}

inline bool ASPDate::operator<= (const ASPDate& dt) const
{
	return (m_tt <= dt.m_tt);
}

inline std::string ASPDate::Time2Str(const time_t& time, const std::string& format /*= "yyyy-mm-dd hh24:mi:ss" */)
{
	otext chTmp[100] = {0};
	struct tm mytm;
	localtime_s(&mytm, &time);
	mytm.tm_year = mytm.tm_year + 1900;
	mytm.tm_mon = mytm.tm_mon + 1;

	_snprintf_s(chTmp, _countof(chTmp), "%04d-%02d-%02d %02d:%02d:%02d",
		mytm.tm_year, mytm.tm_mon, mytm.tm_mday, mytm.tm_hour, mytm.tm_min, mytm.tm_sec);

	ocilib::Date date(std::string(chTmp), "yyyy-mm-dd hh24:mi:ss");
	return date.ToString(format);
}

inline time_t ASPDate::Str2Time(const std::string& timeStr, const std::string& format /*= "yyyy-mm-dd hh24:mi:ss"*/)
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


// ****************************************************************************** 
// 封装库Dataset类
// ******************************************************************************
inline ASPDataset::ASPDataset()
	: m_pCon(NULL),
	  m_pSt(NULL),
	  m_pRs(NULL)
{
}

inline ASPDataset::ASPDataset(const ASPDataset& dt)
	: m_pCon(NULL),
	  m_pSt(NULL),
	  m_pRs(NULL)
{
	if (this != &dt)
	{
		ocilib::Connection* conTmp = new ocilib::Connection(*(dt.m_pCon));
		ocilib::Statement* stTmp = new ocilib::Statement(*(dt.m_pSt));
		ocilib::Resultset* rsTmp = new ocilib::Resultset(*(dt.m_pRs));
		Release();
		m_pCon = conTmp;
		m_pSt = stTmp;
		m_pRs = rsTmp;
	}
}

inline ASPDataset::~ASPDataset(void)
{
	Release();
}

inline ASPDataset& ASPDataset::operator=(const ASPDataset& dt)
{
	if (this != &dt)
	{
		ocilib::Connection* conTmp = new ocilib::Connection(*(dt.m_pCon));
		ocilib::Statement* stTmp = new ocilib::Statement(*(dt.m_pSt));
		ocilib::Resultset* rsTmp = new ocilib::Resultset(*(dt.m_pRs));
		Release();
		m_pCon = conTmp;
		m_pSt = stTmp;
		m_pRs = rsTmp;
	}

	return *this;
}

inline bool ASPDataset::Next()
{
	if (m_pRs)
	{
		return m_pRs->Next();
	}
	else
	{
		return false;
	}
}

template <typename TDataType>
inline  TDataType ASPDataset::Get(const std::string& fieldName)
{
	if (!m_pRs)
	{
		throw std::exception("ASPDataSet::Resultset pointer is null");
	}

	if (m_pRs->IsNull())
	{
		throw std::exception("ASPDataSet::Resultset is not init");
	}

	return m_pRs->Get<TDataType>(fieldName);
}

template <>
inline  ASPDate ASPDataset::Get<ASPDate>(const std::string& fieldName)
{
	if (!m_pRs)
	{
		throw std::exception("ASPDataSet::Resultset pointer is null");
	}

	if (m_pRs->IsNull())
	{
		throw std::exception("ASPDataSet::Resultset is not init");
	}

	ocilib::Date dt = m_pRs->Get<ocilib::Date>(fieldName).Clone();
	if (!dt.IsNull())
	{
		return ASPDate(dt.ToString("yyyy-mm-dd hh24:mi:ss"), "yyyy-mm-dd hh24:mi:ss");
	}
	else
	{
		return ASPDate();
	}
}

template <typename TDataType>
inline TDataType ASPDataset::Get(unsigned int index)
{
	if (!m_pRs)
	{
		throw std::exception("ASPDataSet::Resultset pointer is null");
	}

	if (m_pRs->IsNull())
	{
		throw std::exception("ASPDataSet::Resultset is not init");
	}

	return m_pRs->Get<TDataType>(index);
}

template <>
inline  ASPDate ASPDataset::Get<ASPDate>(unsigned int index)
{
	if (!m_pRs)
	{
		throw std::exception("ASPDataSet::Resultset pointer is null");
	}

	if (m_pRs->IsNull())
	{
		throw std::exception("ASPDataSet::Resultset is not init");
	}

	ocilib::Date dt = m_pRs->Get<ocilib::Date>(index).Clone();
	if (!dt.IsNull())
	{
		return ASPDate(dt.ToString("yyyy-mm-dd hh24:mi:ss"));
	}
	else
	{
		return ASPDate();
	}
}

inline ASPDataset::ASPDataset(const ASPConnection& con, const std::string& sql)
	: m_pCon(NULL),
	  m_pSt(NULL),
	  m_pRs(NULL)
{
	m_pCon = new ocilib::Connection(*(con.m_pCon));
	m_pSt = new ocilib::Statement(*m_pCon);
	m_pSt->Execute(sql);
	m_pRs = new ocilib::Resultset(m_pSt->GetResultset());
}

inline void ASPDataset::Release()
{
	if (m_pSt)
	{
		delete m_pSt;
		m_pSt = NULL;
	}

	if (m_pRs)
	{
		delete m_pRs;
		m_pRs = NULL;
	}

	if (m_pCon)
	{
		if (!m_pCon->IsNull())
		{
			(*m_pCon).Close();
		}

		delete m_pCon;
		m_pCon = NULL;
	}
}


// ****************************************************************************** 
// 封装库Connection类
// ******************************************************************************
inline ASPConnection::ASPConnection(void)
	: m_pCon(new ocilib::Connection())
{
}

inline ASPConnection::ASPConnection(const std::string& dbServiceName, 
	const std::string& user, const std::string& pwd)
	: m_pCon(new ocilib::Connection(dbServiceName, user, pwd))
{
}

inline ASPConnection::ASPConnection(const ocilib::Connection& con)
	: m_pCon(new ocilib::Connection())
{
	ocilib::Connection* tmp = new ocilib::Connection(con);
	Release();
	m_pCon = tmp;
}

inline ASPConnection::ASPConnection(const ASPConnection& con)
	: m_pCon(new ocilib::Connection())
{
	if (this != &con)
	{
		ocilib::Connection* tmp = new ocilib::Connection(*(con.m_pCon));
		Release();
		m_pCon = tmp;
	}
}

inline void ASPConnection::Release()
{
	if (m_pCon)
	{
		if (!m_pCon->IsNull())
		{
			m_pCon->Close();
		}

		delete m_pCon;
		m_pCon = NULL;
	}
}

inline ASPConnection::~ASPConnection(void)
{
	Release();
}

inline ASPConnection& ASPConnection::operator=(const ASPConnection& con)
{
	if (this != &con)
	{
		ocilib::Connection* tmp = new ocilib::Connection(*(con.m_pCon));
		Release();
		m_pCon = tmp;
	}

	return *this;
}

inline void ASPConnection::Open(const std::string& dbServiceName, 
	const std::string& user, const std::string& pwd)
{
	Close();
	m_pCon->Open(dbServiceName, user, pwd);
}

inline void ASPConnection::Close()
{
	if (m_pCon && !m_pCon->IsNull())
	{
		m_pCon->Close();
	}
}

inline void ASPConnection::Commit()
{
	m_pCon->Commit();
}

inline void ASPConnection::Rollback()
{
	m_pCon->Rollback();
}

inline void ASPConnection::Break()
{
	m_pCon->Break();
}

inline void ASPConnection::SetAutoCommit(bool enabled)
{
	m_pCon->SetAutoCommit(enabled);
}

inline bool ASPConnection::GetAutoCommit() const
{
	return m_pCon->GetAutoCommit();
}

inline bool ASPConnection::IsConnected() const
{
	return m_pCon->IsServerAlive();
}

template<typename TDataType>
inline TDataType ASPConnection::SelectOne(const std::string& sql) const
{
	ocilib::Statement st(*m_pCon);
	st.Execute(sql);
	ocilib::Resultset rs = st.GetResultset();
	rs.Next();
	return rs.Get<TDataType>(1);
}

template<>
inline  ASPDate ASPConnection::SelectOne<ASPDate>(const std::string& sql) const
{
	ocilib::Statement st(*m_pCon);
	st.Execute(sql);
	ocilib::Resultset rs = st.GetResultset();
	rs.Next();
	ocilib::Date dt = rs.Get<ocilib::Date>(1).Clone();
	if (!dt.IsNull())
	{
		return ASPDate(dt.ToString("yyyy-mm-dd hh24:mi:ss"));
	}
	else
	{
		return ASPDate();
	}
}

inline ASPDataset ASPConnection::Select(const std::string& sql)
{
	return ASPDataset(*this, sql);
}

inline void ASPConnection::Execute(const std::string& sql)
{
	ocilib::Statement st(*m_pCon);
	st.Execute(sql);
}

inline void ASPConnection::Execute(const std::vector<std::string>& sqlList)
{
	ocilib::Statement st(*m_pCon);

	for (std::vector<std::string>::const_iterator iter = sqlList.cbegin(); 
		iter != sqlList.cend(); ++iter)
	{
		st.Execute(*iter);
	}
}


// ****************************************************************************** 
// 封装库连接池Pool类
// ******************************************************************************
inline ASPPool::ASPPool(void)
	: m_pPool(new ocilib::Pool())
{
}

inline ASPPool::ASPPool(const std::string& dbServiceName, const std::string& user, 
	const std::string& pwd, unsigned int minSize, unsigned int maxSize, 
	unsigned int increment /*= 1*/)
	: m_pPool(new ocilib::Pool(dbServiceName, user, pwd, ocilib::Pool::SessionPool, 
	minSize, maxSize, increment))
{
}

inline void ASPPool::Release()
{
	if (m_pPool)
	{
		if (!m_pPool->IsNull())
		{
			m_pPool->Close();
		}

		delete m_pPool;
		m_pPool = NULL;
	}
}

inline ASPPool::~ASPPool(void)
{
	Release();
}

inline void ASPPool::Open(const std::string& dbServiceName, const std::string& user, 
	const std::string& pwd, unsigned int minSize, unsigned int maxSize, 
	unsigned int increment /*= 1*/)
{
	Close();
	m_pPool->Open(dbServiceName, user, pwd, ocilib::Pool::SessionPool,
		minSize, maxSize, increment);
}

inline void ASPPool::Close()
{
	if (m_pPool)
	{
		m_pPool->Close();
	}
}

inline ASPConnection ASPPool::GetConnection()
{
	return ASPConnection(m_pPool->GetConnection());
}

} // oracle
} // asppro
