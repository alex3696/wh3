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
// базовый класс для всех рекордов
struct Rec
{
	wxString	mID;
};

//-----------------------------------------------------------------------------
/// Класс - основные метаданные - запись в таблицце классов 
struct Cls
{
	struct error : virtual exception_base {};

	// только читаемые с базы
	wxString	mID;		//id bigserial NOT NULL,
	wxString	mVID;
	wxString	mLabel;		//label name NOT NULL,
	wxString	mType;		//type smallint NOT NULL DEFAULT 1 (class_type=0[abstract] class_type=1[numbered] OR class_type=2[quantity]),
	wxString	mParent;	//parent name NOT NULL DEFAULT 'Object'::name,
	wxString	mComment;	//description text,
	wxString	mMeasure;	//measurename text DEFAULT 'ед.'::text,
	wxString	mDefaultPid;//default_pid BIGINT    NOT NULL DEFAULT 0 --местоположение объектов по умолчанию
		
	
	Cls(){}
	Cls(const wxString& id, const wxString& label)
		:mID(id), mLabel(label)
	{
	}

	bool GetClsType(ClsType& result)const
	{
		long tmp;
		if (mType.ToLong(&tmp) && tmp >= 0 && tmp <= 3)
		{
			result = ClsType(tmp);
			return true;
		}
		return false;
	}
	bool IsAbstract()const
	{
		ClsType ct;
		if (!GetClsType(ct))
			BOOST_THROW_EXCEPTION(error() << wxstr("Wrong cls type"));
		return ctAbstract == ct;
	}
	bool IsNumberic()const
	{
		ClsType ct;
		if (!GetClsType(ct))
			BOOST_THROW_EXCEPTION(error() << wxstr("Wrong cls type"));
		return ctSingle == ct;
	}
	bool IsQuantity()const
	{
		ClsType ct;
		if (!GetClsType(ct))
			BOOST_THROW_EXCEPTION(error() << wxstr("Wrong cls type"));
		return ctQtyByOne == ct || ctQtyByFloat == ct;
	}

};

//-----------------------------------------------------------------------------
/// Объект - основные метаданные 
struct ObjTitle
{
	wxString	mID;
	wxString	mPID;
	wxString	mLabel;
	wxString	mQty;
	wxString	mLastLogId;

	ObjTitle(){}
	ObjTitle(const wxString& id,const wxString& label)
		: mID(id), mLabel(label)
	{}
	
};
//-----------------------------------------------------------------------------
/// Информация о свойстве действия - запись из БД 
struct Prop
{
	struct error : virtual exception_base {};

	wxString	mID;
	wxString	mLabel;	
	wxString	mType = L"0";

	Prop(){}
	Prop(const wxString& label, const wxString& type = L"0")
		:mLabel(label), mType(type)
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
	wxString	mVal;
};

//-----------------------------------------------------------------------------
/// Свойства класса
class ClsProp : public PropVal
{
public:
	//Prop		mProp;
	//wxString	mVal;
	wxString	mID;

};

//-----------------------------------------------------------------------------
/// Свойство действия
class ActProp final
{
public:
	Prop		mProp;
	wxString	mID;

};

//-----------------------------------------------------------------------------

/// Объекты общие
struct Obj : public ObjTitle
{
	std::deque<wxString> mProp;
};


//-----------------------------------------------------------------------------
/// Объекты номерного класса
struct ClsObjNum final
{
	wxString	mLabel;
	wxString	mPID;
	wxString	mID;

	ClsObjNum(){}
	ClsObjNum(const wxString& label, const wxString& id)
		:mLabel(label), mID(id)
	{}



};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// Объекты количественного класса
struct ClsObjQty final
{
	wxString	mLabel;
	wxString	mID;
	wxString	mPID;
	wxString	mQty;

	ClsObjQty(){}
	ClsObjQty(const wxString& label, const wxString& pid = wxEmptyString)
		:mLabel(label), mPID(pid)
	{}


};

//-----------------------------------------------------------------------------
/// Разрешения действий класса
struct ClsActAccess final
{
	wxString	mID;
	wxString	mAcessGroup;
	wxString	mAccessDisabled;
	wxString	mScriptRestrict;

	wxString	mActID;
	wxString	mActLabel;  // имя действия???
	//wxString	mClsLabel;  // тип объекта ???
	
	wxString	mClsID;  // имя класса
	wxString	mClsLabel;  // id класса

	wxString	mObjID;  // имя объекта
	wxString	mObjLabel;  // имя объекта
	wxString	mPath; //текущее положение объекта в который перемещают

	
};

//-----------------------------------------------------------------------------
/// Разрешения действий класса
struct ClsSlotAccess final
{
	wxString	mID;
	wxString	mAcessGroup;
	wxString	mAccessDisabled;
	wxString	mScriptRestrict;
	
	wxString	mDstCls;
	wxString	mDstObj;  
	wxString	mDstPath; 

	wxString	mMovCls;
	wxString	mMovObj;
	wxString	mSrcPath;

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
/// Избранное свойство
struct ClsActProp
{
	wxString	mID;
		
	wxString	mActID;
	wxString	mActLabel;

	wxString	mPropID;
	wxString	mPropLabel;
};
//-----------------------------------------------------------------------------
struct PathItem
{
	Cls			mCls;
	ObjTitle	mObj;
};



//-----------------------------------------------------------------------------
struct CatalogCfg
{
	
	CatalogCfg(bool ocat = true)
		:mObjCatalog(ocat)
	{}
	bool mObjCatalog;

    //#ifdef _DEBUG
	//bool mShowDebugColumns = true;
    //#else
	bool mShowDebugColumns = false;
    //#endif
};
//-----------------------------------------------------------------------------


}//namespace rec

}//namespace wh
#endif // __CLASSMODEL_H

