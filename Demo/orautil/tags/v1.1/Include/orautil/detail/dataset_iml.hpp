/*!
* \date 2016/12/13
* \author ¡ı∫£∑Â
* Contact: liuhaifeng@asppro.com.cn
*
* \brief 
*/

#pragma once

#include <cassert>

namespace asppro {
namespace oracle {

	inline Dataset::Dataset(ocilib::Connection conn, const ocilib::ostring& sql) 
		: m_pRs(NULL)
	{
		m_Conn = conn;
		m_Sql = sql;
		Init();
	}

	inline Dataset::~Dataset()
	{
		if (m_pRs)
		{
			delete m_pRs;
			m_pRs = NULL;
		}

		if (!m_Conn.IsNull())
		{
			m_Conn.Close();
		}
	}

	inline void Dataset::Init()
	{
		m_St = ocilib::Statement(m_Conn);
		m_St.Execute(m_Sql);

		if (m_pRs)
		{
			delete m_pRs;
			m_pRs = NULL;
		}

		m_pRs = new ocilib::Resultset(m_St.GetResultset());
	}

	inline bool Dataset::Next()
	{
		if (!m_pRs || m_pRs->IsNull())
		{
			return false;
		}

		return m_pRs->Next();
	}

	template <typename DbType>
	inline DbType Dataset::Get(int index)
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
			, "type only in DTShort, DTUShort, DTInt, DTUInt, DTStr, DTDate, DTFloat, DTDouble, DTBigInt and DTBigUInt");

		assert(index >= 1);

		if (!m_pRs)
		{
			throw std::exception("Resultset pointer is null");
		}

		if (m_pRs->IsNull())
		{
			throw std::exception("ocilib::Resultset is null");
		}

		return m_pRs->Get<DbType>(index);		
	}

	template <>
	inline DTDate Dataset::Get<DTDate>(int index)
	{
		assert(index >= 1);

		if (!m_pRs)
		{
			throw std::exception("Resultset pointer is null");
		}

		if (m_pRs->IsNull())
		{
			throw std::exception("ocilib::Resultset is null");
		}

		return m_pRs->Get<DTDate>(index).Clone();
	}

} // namesapce oracle
} // namespace asppro
