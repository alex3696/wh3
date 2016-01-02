#ifndef __DB_DATA_H
#define __DB_DATA_H

#include "globaldata.h"
#include "ResManager.h"

namespace wh{
//-----------------------------------------------------------------------------

enum DbType{
	dbText = 0,
	
	dbLong = 10,
	dbFloat = 11,
	
	dbDate = 20,

	dbLink = 30,
	
	dbFile = 40,

	dbJSON = 50
};
//-----------------------------------------------------------------------------
struct DbTypeItem
{
	DbTypeItem(DbType dt, const wxString& str)
		:mType(dt), mTitle(str)
	{}

	DbType		mType;
	wxString	mTitle;

};
//-----------------------------------------------------------------------------
using DbTypeArray =
	boost::multi_index_container
	<
	DbTypeItem,
		indexed_by
		<
			random_access<>
			, ordered_unique< BOOST_MULTI_INDEX_MEMBER(DbTypeItem, DbType, mType)>
			, ordered_unique< BOOST_MULTI_INDEX_MEMBER(DbTypeItem, wxString, mTitle)>
		>
	>;

static DbTypeArray dbTypeArray = { 
								{ dbText,"Текст" }, 
								{ dbLong, "Число(целое)" },
								{ dbFloat, "Число(дробное)" },
								{ dbDate, "Дата" },
								{ dbLink, "Ссылка" },
								{ dbFile, "Файл" },
								{ dbJSON, "JSON" },
								};

//-----------------------------------------------------------------------------
static wxString ToText(DbType type)
{
	const auto& dbTypeIdx = dbTypeArray.get<1>();
	auto it = dbTypeIdx.find(type);
	if (dbTypeIdx.end() == it)
	{
		struct dbtype_error : virtual exception_base {};
		BOOST_THROW_EXCEPTION(dbtype_error() << wxstr("Unknown DbType"));
	}
	return it->mTitle;
}
//-----------------------------------------------------------------------------
static DbType ToType(const wxString str)
{
	unsigned long val;
	if (str.ToCULong(&val))
		return (DbType)val;
	const auto& dbTitleIdx = dbTypeArray.get<2>();
	auto it = dbTitleIdx.find(str);
	if (dbTitleIdx.end() == it)
	{
		struct dbtype_error : virtual exception_base {};
		BOOST_THROW_EXCEPTION(dbtype_error() << wxstr("Unknown Type String"));
	}
	return it->mType;
	
}
//-----------------------------------------------------------------------------
class SqlData
{
public:
	struct error : virtual exception_base {};
	virtual wxString SqlVal()const = 0;

	virtual DbType GetType()const = 0;
};
//-----------------------------------------------------------------------------
class SqlLong : public SqlData
{
public:
	enum FilterOp
	{
		foEq = 0,
		foNotEq,
		foLess,
		foMore,
	};

	virtual DbType GetType()const { return dbLong; }

	SqlLong() :mVal(nullptr){}
	~SqlLong(){ delete mVal; }
	SqlLong(const long&     rv)
		: mVal(new long(rv))
	{
	}
	SqlLong(const wxString& rv)
	{
		long val;
		if (rv.ToLong(&val))
			mVal = new long(val);
		else
			mVal = nullptr;
	}
	SqlLong(const SqlLong&  tocopy)
	{
		//SetNull();
		mVal = (tocopy.mVal) ? new long(*tocopy.mVal) : nullptr;
	}
	operator wxString() const
	{
		if (mVal)
			return wxString::Format("%d", *mVal);
		//wxLog BOOST_THROW_EXCEPTION(error() << wxstr("data is null"));
		return wxEmptyString2;
	}
	operator long() const
	{
		if (!mVal)
			BOOST_THROW_EXCEPTION(error() << wxstr("data is null"));
		return *mVal;
	}
	virtual wxString SqlVal()const override
	{
		return (mVal) ? wxString::Format("%d", *mVal) : wxString("NULL");
	}
	inline wxString toStr()const
	{
		return operator wxString();
	}
	SqlLong& operator=(const long&     rv)
	{
		if (mVal)
			*mVal = rv;
		else
			mVal = new long(rv);
		return *this;
	}
	SqlLong& operator=(const wxString& rv)
	{
		if (!rv.IsEmpty())
		{
			long val;
			if (!rv.ToLong(&val))
				BOOST_THROW_EXCEPTION(error() << wxstr("Can`t convert to long"));
			return operator=(val);
		}
		SetNull();
		return *this;
	}
	SqlLong& operator=(const SqlLong&  rv)
	{
		if (rv.mVal)
			return operator=(*rv.mVal);
		else
			SetNull();
		return *this;
	}
	bool operator==(const SqlLong& rv)const
	{
		return (mVal && rv.mVal) ? *mVal == *rv.mVal : false;
	}
	bool operator==(const wxString& rv)const
	{
		return (*this == SqlLong(rv));
	}

	bool operator==(const long& rv)const
	{
		return mVal ? operator==(rv) : false;
	}

	inline bool IsNull()const	{ return mVal ? false : true; }
	inline void SetNull()		{ delete mVal; mVal = nullptr; }
	//inline bool IsEmpty()const	{ return mVal ? true : false; }
	//inline void Clear()			{ delete mVal; mVal = nullptr; }
private:
	long* mVal;
};
//-----------------------------------------------------------------------------
class SqlString : public SqlData
{
public:
	enum FilterOp
	{
		foEq = 0,
		foNotEq,
		foLike,
		foNotLike,
	};

	virtual DbType GetType()const { return dbText; }

	SqlString() :mVal(nullptr){}
	~SqlString(){ delete mVal; }
	SqlString(const wxString& rv)
		:mVal(new wxString(rv))
	{}
	SqlString(const SqlString&  tocopy)
	{
		mVal = (tocopy.mVal) ? new wxString(*tocopy.mVal) : nullptr;
	}
	operator wxString() const
	{
		if (mVal)
			return *mVal;
		//wxLog BOOST_THROW_EXCEPTION(error() << wxstr("data is null"));
		return wxEmptyString2;
	}
	virtual wxString SqlVal()const override
	{
		return (mVal) ? wxString::Format("'%s'", *mVal) : wxString("NULL");
	}

	inline const wxString& toStr()const
	{
		if (mVal)
			return *mVal;
		//wxLog BOOST_THROW_EXCEPTION(error() << wxstr("data is null"));
		return wxEmptyString2;
	}

	SqlString& operator=(const wxString& rv)
	{
		if (!rv.IsEmpty())
		{
			if (mVal)
				*mVal = rv;
			else
				mVal = new wxString(rv);
		}
		else
			SetNull();
		return *this;
	}
	SqlString& operator=(const SqlString&  rv)
	{
		if (rv.mVal)
			return operator=(*rv.mVal);
		else
			SetNull();
		return *this;
	}
	bool operator==(const SqlString& rv)const
	{
		return (mVal && rv.mVal) ? *mVal == *rv.mVal : false;
	}

	inline bool IsNull()const	{ return mVal ? false : true; }
	inline void SetNull()		{ delete mVal; mVal = nullptr; }
	//inline bool IsEmpty()const	{ return mVal ? true : false; }
	//inline void Clear()			{ delete mVal; mVal = nullptr; }
private:
	wxString* mVal;
};


//template <class SQLDATA>
//struct SqlFilter
//{
//	SQLDATA::FilterOp	mOp;
//	SQLDATA				mVal;
//};
//
//
//typedef SqlFilter<SqlLong> FLong;
//typedef SqlFilter<SqlString> FString;
//










}//namespace wh
#endif // __*_H