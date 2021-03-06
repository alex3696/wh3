#ifndef __DB_REC_H
#define __DB_REC_H

#include "db_data.h"

namespace wh{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
enum class ClsKind
{
	Abstract = 0,
	Single = 1,
	QtyByOne = 2,
	QtyByFloat = 3
};
//-----------------------------------------------------------------------------
static wxString ToString(ClsKind kind)
{
	switch (kind)
	{
	case ClsKind::Abstract: return "0";
	case ClsKind::Single:	return "1";
	case ClsKind::QtyByOne:	return "2";
	case ClsKind::QtyByFloat:return "3";
	default:break;
	}
	return wxEmptyString;
}
//-----------------------------------------------------------------------------
static bool ToClsKind(const wxString& str, ClsKind& kind)
{
	unsigned long ul;
	if (str.ToULong(&ul))
	{
		switch (ul)
		{
		case 0: kind = ClsKind::Abstract; return true;
		case 1: kind = ClsKind::Single; return true;
		case 2: kind = ClsKind::QtyByOne; return true;
		case 3: kind = ClsKind::QtyByFloat; return true;
		default:break;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ObjectKey
{
public:
	ObjectKey(int64_t oid, int64_t poid)
		:mId(oid), mParentId(poid)
	{}
	int64_t mId;
	int64_t mParentId;

	bool operator< (ObjectKey const& rhs)const
	{
		if (mId < rhs.mId)
			return true;
		else if (mId == rhs.mId)
			return mParentId < rhs.mParentId;
		return false;
	}

	wxString GetId_AsString()const
	{
		wxLongLong ll(mId);
		return ll.ToString();
	}

	wxString GetParentId_AsString()const
	{
		wxLongLong ll(mParentId);
		return ll.ToString();
	}

};




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

	ClsKind GetClsType()const
	{
		return  ClsKind(long(mType));
	}
	bool IsAbstract()const
	{
		return ClsKind::Abstract == GetClsType();
	}
	bool IsNumberic()const
	{
		return ClsKind::Single == GetClsType();
	}
	bool IsQuantity()const
	{
		return ClsKind::QtyByOne == GetClsType() || ClsKind::QtyByFloat == GetClsType();
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
	
	SqlString	mPeriod;
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
enum CatType
{
	catCls = 0,
	catObj = 1,
	catCustom = 2,
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
struct PageUser{};
//-----------------------------------------------------------------------------
struct PageGroup{};
//-----------------------------------------------------------------------------
struct PageProp{};
//-----------------------------------------------------------------------------
struct PageAct{};
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
struct PageHistory
{
	size_t	mRowsLimit = 50;
	size_t	mRowsOffset = 0;
	size_t	mStringPerRow = 4;
	bool	mPathInProperties = true;
	bool	mColAutosize = false;
	bool	mShowPropertyList = false;
	bool	mShowFilterList = false;
	bool	mVisibleColumnClsObj = true;

	bool operator==(const PageHistory& rv)const
	{
		return rv.mRowsLimit == mRowsLimit
			&& rv.mRowsOffset == mRowsOffset
			&& rv.mStringPerRow == mStringPerRow
			&& rv.mPathInProperties == mPathInProperties
			&& rv.mColAutosize == mColAutosize
			&& rv.mShowPropertyList == mShowPropertyList
			&& rv.mShowFilterList == mShowFilterList
			&& rv.mVisibleColumnClsObj == mVisibleColumnClsObj;
	}
	bool operator!=(const PageHistory& rv)const
	{
		return !operator==(rv);
	}
};
//-----------------------------------------------------------------------------
struct PageReportList
{
	wxString mSelected_id;
};
//-----------------------------------------------------------------------------
struct PageReport
{
	wxString mReportId;
};
//-----------------------------------------------------------------------------


}//namespace rec

}//namespace wh
#endif // __CLASSMODEL_H

