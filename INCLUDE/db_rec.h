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
		case 0: return L"�����";
		case 1: return L"�����";
		case 2: return L"����";
		case 3: return L"������";
		case 4: return L"����";
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
// ������� ����� ��� ���� ��������
struct Base
{
	Base(){}
	Base(const wxString& id, const wxString& label)
		:mId(id), mLabel(label)
	{}
	wxString	mId;
	wxString	mLabel;

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
/// ����� - �������� ���������� - ������ � �������� ������� 
struct Cls
{
	struct error : virtual exception_base {};
	
	wxString	mID;		
	wxString	mVID;
	wxString	mLabel;		
	wxString	mType;		
	Base		mParent;
	wxString	mComment;
	wxString	mMeasure;
	wxString	mDefaultPid;

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
/// ������ - �������� ���������� 
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
/// ���������� � �������� �������� - ������ �� �� 
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
		case 0: return L"�����";
		case 1: return L"�����";
		case 2: return L"����";
		case 3: return L"������";
		case 4: return L"����";
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
	wxString	mVal;
};

//-----------------------------------------------------------------------------
/// �������� ������
class ClsProp : public PropVal
{
public:
	//Prop		mProp;
	//wxString	mVal;
	wxString	mID;

};

//-----------------------------------------------------------------------------
/// �������� ��������
class ActProp final
{
public:
	Prop		mProp;
	wxString	mID;

};

//-----------------------------------------------------------------------------

/// ������� �����
struct Obj : public ObjTitle
{
	std::deque<wxString> mProp;
};


//-----------------------------------------------------------------------------
/// ������� ��������� ������
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
/// ������� ��������������� ������
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
/// ���������� �������� ������
struct ClsActAccess final
{
	wxString	mID;
	wxString	mAcessGroup;
	wxString	mAccessDisabled;
	wxString	mScriptRestrict;

	wxString	mActID;
	wxString	mActLabel;  // ��� ��������???
	//wxString	mClsLabel;  // ��� ������� ???
	
	wxString	mClsID;  // ��� ������
	wxString	mClsLabel;  // id ������

	wxString	mObjID;  // ��� �������
	wxString	mObjLabel;  // ��� �������
	wxString	mPath; //������� ��������� ������� � ������� ����������

	
};

//-----------------------------------------------------------------------------
/// ���������� �������� ������
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
/// ��������� ��������
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

