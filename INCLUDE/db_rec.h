#ifndef __DB_REC_H
#define __DB_REC_H

#include "db_data.h"

namespace wh{
//-----------------------------------------------------------------------------
enum ClsType
{
	ctAbstract = 0,
	ctSingle = 1,
	ctQtyByOne = 2,
	ctQtyByFloat = 3
};
//-----------------------------------------------------------------------------







namespace rec{


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
typedef ObjParent PathNode;

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
	FieldType	mType;
	wxString	mVarArray;
	wxString	mVarStrict;

	Prop(){}
	Prop(const wxString& label, const FieldType type = ftText)
		//:mLabel(label), mType(type)
		:Base(wxEmptyString,label), mType(type)
	{}
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
	Prop	mProp;
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

	SqlString	mArrId;
	SqlString	mArrTitle;

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

	SqlString	mSrcArrId;		//SqlString	mSrcPath;
	SqlString	mSrcArrTitle;	//SqlString	mSrcPath;

	Base		mCls;
	Base		mObj;

	Base		mDstCls;
	Base		mDstObj;

	SqlString	mDstArrId;		//SqlString	mDstPath;
	SqlString	mDstArrTitle;	//SqlString	mDstPath;
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
typedef PathItem ObjInfo;

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
enum CatType
{
	catCls = 0,
	catObj = 1,
	catCustom = 2,
	catFav = 3
};
//-----------------------------------------------------------------------------

struct CatCfg
{
	CatType mCatType;
	bool mEnableProp;
	bool mEnableObj;
	bool mHideSystemRoot;
	bool mShowDebugColumns;

	CatCfg(CatType catType = catObj
				, bool enableProp = true
				, bool enableObj = true
				, bool hideSysRoot = false
				, bool debugColumns = false)
		: mCatType(catType)
		, mEnableProp(enableProp)
		, mEnableObj(enableObj)
		, mHideSystemRoot(hideSysRoot)
		, mShowDebugColumns(debugColumns)
	{
	}

};
//-----------------------------------------------------------------------------

/** Узел избранного */
class Fav
{
public:
	wxString		mId;
	wxString		mTitle;
	wxString		mNote;
	wxString		mUsr;
	unsigned int 	mViewGroup;
};
//-------------------------------------------------------------------------

/** Фильтр по Классу  */
class FavFilterCls
{
public:
	//wxString	mFId;		// подвязываем к Fav потому не загружаем
	wxString	mCId;
	wxString	mCTitle;	//только для отображения в базе нет
	wxString	mCKind;		//только для отображения в базе нет
};
//-------------------------------------------------------------------------

/** Фильтр по местоположению  */
class FavFilterPath
{
public:
	unsigned long	mFId;
	unsigned long	mParentOId;
	wxString		mPath;
};
//-------------------------------------------------------------------------

/** Вид свойств класса */
class FavViewPropCls
{
public:
	unsigned long	mFId;
	unsigned long	mPId;
};
//-------------------------------------------------------------------------

/** Вид общих свойств действия*/
class FavViewPropActCommon
{
public:
	unsigned long	mFId;
	unsigned long	mAId;

	bool			mShowDateTime;
	bool			mShowUser;
	bool			mShowSrcPath;
	bool			mShowQty;
};
//-------------------------------------------------------------------------

/** Вид пользовательских свойств*/
class FavViewPropActUsr
{
public:
	unsigned long	mFId;
	unsigned long	mAId;
	unsigned long	mPId;
};

struct PageUser{};
struct PageGroup{};
struct PageProp{};
struct PageAct{};

struct PageObjByPath
{
	wxString mParent_Oid;
};
//-----------------------------------------------------------------------------

struct PageObjByType
{
	wxString mParent_Cid;
};
//-----------------------------------------------------------------------------

typedef PathItem PageDetail;

struct PageHistory
{
	size_t	mRowsLimit = 50;
	size_t	mRowsOffset = 0;
	size_t	mStringPerRow = 4;
	bool	mPathInProperties = true;
	bool	mColAutosize = false;
	bool	mShowPropertyList = false;
	bool	mShowFilterList = false;

	bool operator==(const PageHistory& rv)const
	{
		return rv.mRowsLimit == mRowsLimit
			&& rv.mRowsOffset == mRowsOffset
			&& rv.mStringPerRow == mStringPerRow
			&& rv.mPathInProperties == mPathInProperties
			&& rv.mColAutosize == mColAutosize
			&& rv.mShowPropertyList == mShowPropertyList
			&& rv.mShowFilterList == mShowFilterList;
	}
	bool operator!=(const PageHistory& rv)const
	{
		return !operator==(rv);
	}
};

struct PageReportList
{
	wxString mSelected_id;
};

struct PageReport
{
	wxString mReportId;
};



}//namespace rec

}//namespace wh
#endif // __CLASSMODEL_H

