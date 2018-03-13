#ifndef __MODEL_BROWSER_DATA_H
#define __MODEL_BROWSER_DATA_H

#include "globaldata.h"
#include "db_rec.h"





enum class BrowserMode
{
	Home = 0
	, ByType = 1
	, ByPath = 2
	, FilterByType = 10
	, FilterByPath = 20
	, FindByType = 100
	, FindByPath = 200
};

enum class Operation
{
	  BeforeInsert
	, AfterInsert
	, BeforeUpdate
	, AfterUpdate
	, BeforeDelete
	, AfterDelete
};






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
	operator int64_t() const		{ return mId; }

	bool Set(const wxString& str){ return str.ToLongLong(&mId); };
	void Set(const int64_t& val) { mId = val; };

	Bigint& operator=(const wxString& str)	{ str.ToLongLong(&mId); return *this; };
	Bigint& operator=(const int64_t& rv) { mId = rv; return *this; };
};
//-----------------------------------------------------------------------------
enum class ClassId 
{
	IObject=0
};

class IObject : boost::noncopyable
{
public:
	virtual ~IObject() {}
	virtual ClassId GetClassId()const { return ClassId::IObject; };

};
//-----------------------------------------------------------------------------
class IIdent64: public IObject
{
public:
	wxString GetIdAsString()const
	{
		wxLongLong ll(GetId());
		return ll.ToString();
	}

	virtual const int64_t& GetId()const = 0;
	virtual const wxString& GetTitle()const = 0;

};
struct extr_id_IIdent64
{
	typedef const int64_t& result_type;
	inline result_type operator()(const std::shared_ptr<const IIdent64>& r)const
	{
		return r->GetId();
	}
};
struct extr_void_ptr_IIdent64
{
	typedef const void* result_type;
	inline result_type operator()(const std::shared_ptr<const IIdent64>& r)const
	{
		return r.get();
	}
};

//-----------------------------------------------------------------------------
class IPath64 : public IObject
{
public:
	virtual wxString AsString()const = 0;
	virtual size_t GetQty()const = 0;
	virtual const IIdent64* GetItem(size_t)const = 0;
	virtual const wxString GetItemString(size_t)const = 0;
};

class IClsPath64 : public IPath64
{
public:
};


class IObjPath64 : public IPath64
{
public:
};
//-----------------------------------------------------------------------------
class ICls64;
using SpCls = std::shared_ptr<ICls64>;
using SpClsConst = std::shared_ptr<const ICls64>;

class IObj64;
using SpObj = std::shared_ptr<IObj64>;
using SpObjConst = std::shared_ptr<const IObj64>;

class IProp64;
using SpProp = std::shared_ptr<IProp64>;
using SpPropConst = std::shared_ptr<const IProp64>;

class IPropVal64;
using SpPropVal = std::shared_ptr<IPropVal64>;
using SpPropValConst = std::shared_ptr<const IPropVal64>;

class IAct64;

template <class SP>
class TSpTable : public
	boost::multi_index_container
	<
		SP,
		indexed_by
		<
			random_access<> //SQL order
			, ordered_unique< extr_id_IIdent64 >
			, ordered_unique< extr_void_ptr_IIdent64 >
		>
	>
{
public:
};

using SpClsTable = TSpTable<SpCls>;
using SpClsConstTable = TSpTable<SpClsConst>;
using SpObjTable = TSpTable<SpObj>;
using SpObjConstTable = TSpTable<SpObjConst>;
using SpPropTable = TSpTable<SpProp>;
using SpPropConstTable = TSpTable<SpPropConst>;
using SpPropValTable = TSpTable<SpPropVal>;
using SpPropValConstTable = TSpTable<SpPropValConst>;

//-----------------------------------------------------------------------------
class IAct64 : public IIdent64
{
public:
	virtual const wxString& GetColour()const = 0;

};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
enum class FavAPropInfo
{
	UnnownProp = 0

	,PreviosTimestamp = 01
	,PreviosDate = 02
	,PreviosTime = 03

	,PeriodInterval = 11
	,PeriodSec = 12
	,PeriodDay = 13

	,NextTimestamp = 21
	,NextDate = 22
	,NextTime = 23

	,LeftInterval = 31
	,LeftSec = 32
	,LeftDay = 33
};

static int FavAPropInfo2Int(const FavAPropInfo& val){ return static_cast<int>(val); }
static FavAPropInfo Int2FavAPropInfo(const int& val) { return static_cast<FavAPropInfo>(val); }

static FavAPropInfo StrInt2FavAPropInfo(const wxString& str_int) 
{ 
	unsigned long val = 0;
	str_int.ToULong(&val);
	return Int2FavAPropInfo(val);
}


static wxString FavAPropInfo2Text(FavAPropInfo fapi)
{
	switch (fapi)
	{
	case FavAPropInfo::PreviosTimestamp:return "пред.(дата+время)";
	case FavAPropInfo::PreviosDate:		return "пред.(дата)";
	case FavAPropInfo::PreviosTime:		return "пред.(время)";

	case FavAPropInfo::PeriodInterval:	return "период(interval)";
	case FavAPropInfo::PeriodSec:		return "период(секунды)";
	case FavAPropInfo::PeriodDay:		return "период(сутки)";

	case FavAPropInfo::NextTimestamp:	return "след.(дата+время)";
	case FavAPropInfo::NextDate:		return "след.(дата)";
	case FavAPropInfo::NextTime:		return "след.(время)";

	case FavAPropInfo::LeftInterval:	return "осталось(interval)";
	case FavAPropInfo::LeftSec:			return "осталось(секунды)";
	case FavAPropInfo::LeftDay:			return "осталось(сутки)";
	default:			break;
	}
	return "?? неизвестный параметр ??";
}

static const wxIcon& GetIcon(FavAPropInfo fapi)
{
	auto mgr = ResMgr::GetInstance();
	const wxIcon* ico=&wxNullIcon;
	
		
	switch (fapi)
	{
	case FavAPropInfo::PreviosTimestamp:
	case FavAPropInfo::PreviosDate:		
	case FavAPropInfo::PreviosTime:		ico = &mgr->m_ico_act_previos16; break;

	case FavAPropInfo::PeriodInterval:	
	case FavAPropInfo::PeriodSec:		
	case FavAPropInfo::PeriodDay:		ico = &mgr->m_ico_act_period16; break;

	case FavAPropInfo::NextTimestamp:	
	case FavAPropInfo::NextDate:		
	case FavAPropInfo::NextTime:		ico = &mgr->m_ico_act_next16; break;

	case FavAPropInfo::LeftInterval:	
	case FavAPropInfo::LeftSec:			
	case FavAPropInfo::LeftDay:			ico = &mgr->m_ico_act_left16; break;
	default:			break;
	}
	return *ico;
}


//-----------------------------------------------------------------------------
struct FavAProp : public IObject
{
	FavAProp() {}
	FavAProp(std::shared_ptr<const IAct64> act
		, const FavAPropInfo& val = FavAPropInfo::PreviosTimestamp)
		:mAct(act), mInfo(val)
	{}
	//std::shared_ptr<const ICls64>	mAct; already linked to cls
	std::shared_ptr<const IAct64>	mAct;
	FavAPropInfo					mInfo;

};
struct extr_aid_ClsFavAct
{
	typedef const int64_t result_type;
	inline result_type operator()(const std::shared_ptr<const FavAProp>& r)const
	{
		return r->mAct->GetId();
	}
};
struct extr_info__ClsFavAct
{
	typedef const FavAPropInfo result_type;
	inline result_type operator()(const std::shared_ptr<const FavAProp>& r)const
	{
		return r->mInfo;
	}
};
using ConstClsFavActTable = boost::multi_index_container
<
	std::shared_ptr<const FavAProp>,
	indexed_by
	<
		ordered_unique 
		<
			composite_key 
			<
				std::shared_ptr<const FavAProp>
				, extr_aid_ClsFavAct
				, extr_info__ClsFavAct
			>
		>
	>
>;
//using ConstClsFavActTable = std::vector<std::shared_ptr<const FavAProp>>;
//-----------------------------------------------------------------------------
// FavActInfo
struct FavAPropValue
{
	std::shared_ptr<const FavAProp>	mFavActProp;
	wxString							mValue;

	FavAPropValue(const std::shared_ptr<const FavAProp>& favActProp, const wxString& val)
		:mFavActProp(favActProp), mValue(val)
	{}
};
struct extr_aid_FavAPropValue
{
	typedef const int64_t result_type;
	inline result_type operator()(const std::shared_ptr<const FavAPropValue>& r)const
	{
		return r->mFavActProp->mAct->GetId();
	}
};
struct extr_info__FavAPropValue
{
	typedef const FavAPropInfo result_type;
	inline result_type operator()(const std::shared_ptr<const FavAPropValue>& r)const
	{
		return r->mFavActProp->mInfo;
	}
};

using FavAPropValueTable = boost::multi_index_container
<
	std::shared_ptr<const FavAPropValue>,
	indexed_by
	<
		ordered_unique 
		<
			composite_key 
			<
				std::shared_ptr<const FavAPropValue>
				, extr_aid_FavAPropValue
				, extr_info__FavAPropValue
			>
		>
	>
>;
//-----------------------------------------------------------------------------
class IProp64 : public IIdent64
{
public:
	virtual FieldType GetKind()const = 0;
	virtual const wxArrayString& GetVar()const = 0;
	virtual bool GetVarStrict()const = 0;

};
using ConstPropTable = boost::multi_index_container
<
	std::shared_ptr<const IProp64>,
	indexed_by
	<
	ordered_unique< extr_id_IIdent64 >
	>
>;


class ConstPropLink : public IProp64
{
	std::shared_ptr<const IProp64> mProp;
public:
	ConstPropLink(std::shared_ptr<const IProp64> prop)
		:mProp(prop)
	{}
	virtual const int64_t& GetId()const override { return mProp->GetId(); };
	virtual const wxString& GetTitle()const override { return mProp->GetTitle(); };
	virtual FieldType GetKind()const override { return mProp->GetKind(); };
	virtual const wxArrayString& GetVar()const override { return mProp->GetVar(); };
	virtual bool GetVarStrict()const override { return mProp->GetVarStrict(); };

	std::shared_ptr<const IProp64> GetProp()const { return mProp; };
};

typedef ConstPropLink ObjProp;
typedef ConstPropLink ClsProp;




//-----------------------------------------------------------------------------
struct PropVal
{
	PropVal() {}
	PropVal(std::shared_ptr<const IProp64> prop, const wxString& val=wxEmptyString)
		:mProp(prop), mValue(val)
	{
	}

	std::shared_ptr<const IProp64>	mProp;
	wxString						mValue;
};
struct extr_pid_PropVal
{
	typedef const int64_t result_type;
	inline result_type operator()(const std::shared_ptr<const PropVal>& r)const
	{
		return r->mProp->GetId();
	}
};


using ConstPropValTable = boost::multi_index_container
<
	std::shared_ptr<const PropVal>,
	indexed_by
	<
		ordered_unique< extr_pid_PropVal >
	>
>;


//-----------------------------------------------------------------------------
class ICls64 : public IIdent64
{
public:
	using ChildsTable = std::vector<std::shared_ptr<ICls64>>;
	using ObjTable = std::vector<std::shared_ptr<const IObj64>>;
	using ClsTable = std::vector<std::shared_ptr<const ICls64>>;

	virtual int64_t GetParentId()const = 0;

	virtual		  ClsKind   GetKind()const = 0;
	virtual const wxString& GetMeasure()const = 0;
	virtual const wxString& GetObjectsQty()const = 0;
	virtual const SpPropValConstTable&	GetProperties()const = 0;
	
	virtual void ClearObjTable() = 0;
	virtual const std::shared_ptr<const ObjTable> GetObjTable()const = 0;

	virtual const std::shared_ptr<const ClsTable> GetClsChilds()const = 0;

	virtual std::shared_ptr<const ICls64> GetParent()const = 0;

	// cls favorite attributes + values
	virtual const ConstPropValTable&	GetFavCPropValue()const = 0;
	
	// cls-act favorite attributes 
	virtual const ConstClsFavActTable&	GetFavAProp()const = 0;
	// cls-act favorite values
	virtual const FavAPropValueTable&	GetFavAPropValue()const = 0;
	bool GetActPeriod(int64_t aid, wxString& period)const
	{
		const auto& idxFAV = GetFavAPropValue().get<0>();
		auto fit = idxFAV.find(boost::make_tuple(aid, FavAPropInfo::PeriodDay));
		if (idxFAV.end() == fit)
			return false;
		period = (*fit)->mValue;
		return true;
	}
	
	// cls-obj favorite attributes 
	virtual const ConstPropTable&		GetFavOProp()const = 0;
};

//-----------------------------------------------------------------------------
class IObj64 : public IIdent64
{
public:
	using ChildsTable = std::vector<std::shared_ptr<IObj64>>;

	virtual wxString					GetQty()const = 0;
	virtual SpClsConst					GetCls()const = 0;

	virtual int64_t GetClsId()const = 0;

	virtual std::shared_ptr<const IObjPath64> GetPath()const = 0;
	
	virtual const SpPropValConstTable&	GetProperties()const = 0;

	virtual std::shared_ptr<const IObj64> GetParent()const = 0;
	virtual int64_t GetParentId()const = 0;

	virtual int GetActPrevios(int64_t aid, wxDateTime& dt)const = 0;
	virtual int GetActNext(int64_t aid, wxDateTime& dt)const = 0;
	virtual int GetActLeft(int64_t aid, double& dt)const = 0;
	bool GetActPeriod(int64_t aid, wxString& period)const
	{
		return GetCls()->GetActPeriod(aid, period);
	}
	
	virtual const FavAPropValueTable&	GetFavAPropValue()const = 0;
	virtual const ConstPropValTable&	GetFavOPropValue()const = 0;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H
