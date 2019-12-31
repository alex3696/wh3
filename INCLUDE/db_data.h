#ifndef __DB_DATA_H
#define __DB_DATA_H

#include "globaldata.h"
#include "ResManager.h"
#include "TModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class Bigint 
{
	//std::unique_ptr<int64_t> mIdPtr;
	int64_t mId;
public:
	Bigint():mId(0){}
	Bigint(const wxString& str){ str.ToLongLong(&mId); };
	Bigint(const int64_t& val):mId(val) { };

	//inline bool IsNull()const	{ return mIdPtr ? false : true; }
	//inline void SetNull()		{ mIdPtr.reset(); }

	wxString AsString()const	{ return wxString::Format("%d", mId); }
	const int64_t&  AsInt64()const		{ return mId; }

	operator wxString() const	{ return wxString::Format("%d", mId); }
	operator int64_t() const	{ return mId; }

	bool Set(const wxString& str){ return str.ToLongLong(&mId); };
	void Set(const int64_t& val) { mId = val; };

	Bigint& operator=(const wxString& str)	{ str.ToLongLong(&mId); return *this; };
	Bigint& operator=(const int64_t& rv)	{ mId = rv; return *this; };
};
//-----------------------------------------------------------------------------
class SqlData
{
public:
	struct error : virtual exception_base {};
	virtual wxString SqlVal()const = 0;

	virtual FieldType GetType()const = 0;
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

	virtual FieldType GetType()const { return ftLong; }

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

	virtual FieldType GetType()const { return ftText; }

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

}//namespace wh
#endif // __*_H