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
// ������� ����� ��� ���� ��������
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
/// ����� - �������� ���������� - ������ � �������� ������� 
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
/// ������ - �������� ���������� 
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
/// ���������� � �������� �������� - ������ �� �� 
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
/// �������� 
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
/// �������� ��������
class PropVal
{
public:
	Prop		mProp;
	SqlString	mVal;
};

//-----------------------------------------------------------------------------
/// �������� ������
class ClsProp : public PropVal
{
public:
	//Prop		mProp;
	//wxString	mVal;
	SqlLong	mId;

};

//-----------------------------------------------------------------------------
/// �������� ��������
class ActProp final
{
public:
	Prop	mProp;
	SqlLong	mId;

};

//-----------------------------------------------------------------------------

/// ������� �����
struct Obj : public ObjTitle
{
	std::deque<wxString> mProp;
};

//-----------------------------------------------------------------------------

/// ���������� �������� ������
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
/// ���������� �������� ������
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
/// ������ ������ ������������� 
struct Role
{
	wxString	mID;
	wxString	mLabel;	
	wxString	mComment;

};


//-----------------------------------------------------------------------------
/// ������������ - ������ �� ��
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
/// ������������ - ������ �� ��
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

/** ���� ���������� */
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

/** ������ �� ������  */
class FavFilterCls
{
public:
	//wxString	mFId;		// ����������� � Fav ������ �� ���������
	wxString	mCId;
	wxString	mCTitle;	//������ ��� ����������� � ���� ���
	wxString	mCKind;		//������ ��� ����������� � ���� ���
};
//-------------------------------------------------------------------------

/** ������ �� ��������������  */
class FavFilterPath
{
public:
	unsigned long	mFId;
	unsigned long	mParentOId;
	wxString		mPath;
};
//-------------------------------------------------------------------------

/** ��� ������� ������ */
class FavViewPropCls
{
public:
	unsigned long	mFId;
	unsigned long	mPId;
};
//-------------------------------------------------------------------------

/** ��� ����� ������� ��������*/
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

/** ��� ���������������� �������*/
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

