#ifndef __DB_REC_H
#define __DB_REC_H

#include "globaldata.h"
#include "ResManager.h"

namespace wh{

enum ClsType
{
	ctAbstract = 0,
	ctSingle = 1,
	ctQtyByOne = 2,
	ctQtyByFloat = 3
};




class ValType
{
public:
	enum Type{
		vtNull = -1,
		vtText = 0,
		vtNumber = 1,
		vtData = 2,
		vtLink = 3,
		vtFile = 4
	};
	ValType()		:mType(vtNull)	{}
	ValType(Type t)	:mType(t)		{}
	ValType(const ValType& vt)
	{
		mType = vt.mType;
	}
	ValType(int i)	
	{
		FromInt(i);
	}
	ValType(const wxString& str)
	{
		FromStr(str);
	}

	ValType& operator=(Type rv)
	{
		mType = rv;
		return *this;
	}
	ValType& operator=(const ValType& rv)
	{
		mType = rv.mType;
		return *this;
	}
	bool operator==(const ValType& rv)const
	{
		return mType == rv.mType;
	}


	ValType& operator=(int rv)
	{
		FromInt(rv);
		return *this;
	}
	operator int() const
	{
		return (int)mType;
	}

	ValType& operator=(const wxString& str)
	{
		FromStr(str);
		return *this;
	}
	operator wxString() const
	{
		return wxString::Format("%d", (int)mType);
	}

	wxString ToText()const
	{
		switch (mType)
		{
		default: return L"unknown type"; break;
		case 0: return L"Текст";
		case 1: return L"Число";
		case 2: return L"Дата";
		case 3: return L"Ссылка";
		case 4: return L"Файл";
		}//switch(val)
		return wxEmptyString;
	}
private:
	void FromInt(const int i)
	{
		if (i >= 0 && i < 5)
			mType = Type(i);
		else
			mType = vtNull;
	}
	void FromStr(const wxString& str)
	{
		unsigned long tmp = 0;
		if (str.ToULong(&tmp))
			mType = Type(tmp);
		else
			mType = vtNull;
	}

	Type mType;

};


namespace rec{
//-----------------------------------------------------------------------------
class SqlData
{
public:
	struct error : virtual exception_base {};
	virtual wxString SqlVal()const = 0;
};
//-----------------------------------------------------------------------------
class SqlLong : public SqlData
{
public:
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






//-----------------------------------------------------------------------------
// базовый класс для всех рекордов
struct Base
{
	Base(){}
	Base(const wxString& id, const wxString& label)
		:mId(id), mLabel(label)
	{}
	SqlLong		mId;
	SqlString	mLabel;

	inline bool operator == (const rec::Base& b)const
	{
		return (mId == b.mId && mLabel == b.mLabel);
	}
};
//-----------------------------------------------------------------------------
struct ObjParent
{
	ObjParent(){}
	ObjParent(const wxString& cls_id, const wxString& cls_label
		,const wxString& obj_id, const wxString& obj_label)
	{
		mCls.mId = cls_id;
		mCls.mLabel = cls_label;
		mObj.mId = obj_id;
		mObj.mLabel = obj_label;
	}

	Base mCls;
	Base mObj;
};


//-----------------------------------------------------------------------------
/// Класс - основные метаданные - запись в таблицце классов 
struct Cls: public Base
{
	struct error : virtual exception_base {};
	
	//SqlLong		mID;
	//SqlString	mLabel;
	SqlLong		mType;
	Base		mParent;
	SqlString	mComment;
	SqlString	mMeasure;
	Base		mDefaultObj;

	Cls(){}
	Cls(const wxString& id, const wxString& label)
		:Base(id, label)
	{
	}

	ClsType GetClsType()const
	{
		return  ClsType(long(mType));
	}
	bool IsAbstract()const
	{
		return ctAbstract == GetClsType();
	}
	bool IsNumberic()const
	{
		return ctSingle == GetClsType();
	}
	bool IsQuantity()const
	{
		return ctQtyByOne == GetClsType() || ctQtyByFloat == GetClsType();
	}

};

//-----------------------------------------------------------------------------
/// Объект - основные метаданные 
struct ObjTitle : public Base
{
	//SqlLong		mId;
	//SqlString	mLabel;
	Base		mParent;
	SqlString	mQty;
	SqlLong		mLastMoveLogId;
	SqlLong		mLastActLogId;

	ObjTitle(){}
	ObjTitle(const wxString& id,const wxString& label)
		: Base(id,label)
	{}
	
};
//-----------------------------------------------------------------------------
/// Информация о свойстве действия - запись из БД 
struct Prop: public Base
{
	struct error : virtual exception_base {};

	//wxString	mID;
	//wxString	mLabel;	
	wxString	mType = L"0";

	Prop(){}
	Prop(const wxString& label, const wxString& type = L"0")
		//:mLabel(label), mType(type)
		:Base(wxEmptyString,label), mType(type)
	{}


	static wxString IntToType(unsigned int val)
	{
		switch (val)
		{
		default: BOOST_THROW_EXCEPTION(error() << wxstr("unknown type")); break;
		case 0: return L"Текст";
		case 1: return L"Число";
		case 2: return L"Дата";
		case 3: return L"Ссылка";
		case 4: return L"Файл";
		}//switch(val)
		return wxEmptyString;
	}//static wxString IntToType(unsigned int val)
	wxString GetTypeString()const
	{
		unsigned long tmp = 0;
		mType.ToULong(&tmp);
		return IntToType(tmp);
	}//wxString GetTypeString()const

	unsigned long  GetTypeInt()const
	{
		unsigned long tmp = 0;
		mType.ToULong(&tmp);
		return tmp;
	}

};

//-----------------------------------------------------------------------------
/// действие 
class Act final
{
public:
	wxString	mID;
	wxString	mLabel;
	wxString	mComment;
	wxString	mColor;
	wxString	mScript;
	wxString	mVID;
	
};

//-----------------------------------------------------------------------------
/// Значение свойства
class PropVal
{
public:
	Prop		mProp;
	SqlString	mVal;
};

//-----------------------------------------------------------------------------
/// Свойства класса
class ClsProp : public PropVal
{
public:
	//Prop		mProp;
	//wxString	mVal;
	SqlLong	mId;

};

//-----------------------------------------------------------------------------
/// Свойство действия
class ActProp final
{
public:
	Prop		mProp;
	SqlLong	mId;

};

//-----------------------------------------------------------------------------

/// Объекты общие
struct Obj : public ObjTitle
{
	std::deque<wxString> mProp;
};

//-----------------------------------------------------------------------------

/// Разрешения действий класса
struct ClsActAccess
{
	SqlLong	    mId;
	SqlString	mAcessGroup;
	SqlLong		mAccessDisabled;
	SqlString	mScriptRestrict;

	Base		mCls;
	Base		mObj;
	SqlString	mSrcPath;

	Base		mAct;
	
};

//-----------------------------------------------------------------------------
/// Разрешения действий класса
struct ClsSlotAccess
{
	SqlLong	    mId;
	SqlString	mAcessGroup;
	SqlLong		mAccessDisabled;
	SqlString	mScriptRestrict;

	Base		mSrcCls;
	Base		mSrcObj;
	SqlString	mSrcPath;

	Base		mCls;
	Base		mObj;

	Base		mDstCls;
	Base		mDstObj;
	SqlString	mDstPath;
};

//-----------------------------------------------------------------------------
/// Модель группы пользователей 
struct Role
{
	wxString	mID;
	wxString	mLabel;	
	wxString	mComment;

};


//-----------------------------------------------------------------------------
/// Пользователь - запись из БД
struct User
{
	wxString	mID;
	wxString	mLabel;	
	wxString	mComment;

	wxString	mConnLimit;
	wxString	mValidUntil;
	wxString	mPassword;

};

//-----------------------------------------------------------------------------
/// Пользователь - запись из БД
struct UserRole
{
	wxString	mID;
	wxString	mLabel;



};

//-----------------------------------------------------------------------------
struct PathItem
{
	Cls			mCls;
	ObjTitle	mObj;
};
//-----------------------------------------------------------------------------
struct ObjInfo
{
	Cls			mCls;
	ObjTitle	mObj;
};
//-----------------------------------------------------------------------------
struct FavProp
{

	wxString	mID;
	wxString	mLabel;
	wxString	mType;

	std::shared_ptr<std::set<wxString> > mCls;

	FavProp()
		:mCls(new std::set<wxString>)
	{}
	FavProp(const FavProp& r)
	{
		mID = r.mID;
		mLabel = r.mLabel;
		mType = r.mType;
		mCls.reset(new std::set<wxString>(*r.mCls));
	}
};

using FavProps = boost::multi_index_container	
	<
		FavProp,
		indexed_by	
		<
			random_access<>
			, ordered_unique< BOOST_MULTI_INDEX_MEMBER(FavProp, wxString, mLabel)> 
		>
	>;
//-----------------------------------------------------------------------------
struct FavoriteProp : public Prop
{
	bool mSelected;
};


//-----------------------------------------------------------------------------
struct CatalogCfg
{
	bool mIsOjbTree;
	bool mIsSelectDlg;
	bool mIsFindResult;
	bool mShowDebugColumns;

	CatalogCfg(bool isObjTree = true, bool isSelectDlg = false,
		bool isFindResult = false, bool debugColumns = false)
		: mIsOjbTree(isObjTree)
		, mIsSelectDlg(isSelectDlg)
		, mIsFindResult(isFindResult)
		, mShowDebugColumns(debugColumns)
	{
	}

};
//-----------------------------------------------------------------------------























}//namespace rec

}//namespace wh
#endif // __CLASSMODEL_H

