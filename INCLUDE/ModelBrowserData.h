#ifndef __MODEL_BROWSER_DATA_H
#define __MODEL_BROWSER_DATA_H

#include "globaldata.h"
#include "db_rec.h"


enum class ClsKind
{
	Abstract = 0,
	Single = 1,
	QtyByOne = 2,
	QtyByFloat = 3
};

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




namespace wh{
//-----------------------------------------------------------------------------
class Bigint 
{
	//std::unique_ptr<int64_t> mIdPtr;
	int64_t mId;
public:
	Bigint(){}
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
	Bigint& operator=(const int64_t& rv)	{ mId = rv; };
};
//-----------------------------------------------------------------------------
class IIdent64
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
class IPath64
{
public:
	virtual wxString AsString()const = 0;
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



class ICls64 : public IIdent64
{
public:
	virtual		  ClsKind   GetKind()const = 0;
	virtual const wxString& GetMeasure()const = 0;

	virtual const SpObjConstTable&		GetObjects()const = 0;
	virtual const SpPropValConstTable&	GetProperties()const = 0;

};

//-----------------------------------------------------------------------------
class IObj64 : public IIdent64
{
public:
	virtual SpClsConst					GetCls()const = 0;

	virtual const SpPropValConstTable&	GetProperties()const = 0;
	virtual const IObjPath64&			GetPath()const = 0;


};

class IProp64 : public IIdent64
{
public:
	virtual const wxString& GetKind()const = 0;

	FieldType GetType()const	{ return ToFieldType(GetKind()); }
};

class IPropVal64
{
public:
	virtual const IProp64&	GetProp()const = 0;
	virtual const wxString& GetValue()const = 0;
};





class ClsNode
{
public:
	using ChildsTable = std::vector<std::shared_ptr<ClsNode>>;
private:
	/*
	struct extr_void_ptr_ClsNode
	{
		typedef const void* result_type;
		inline result_type operator()(const std::shared_ptr<const ClsNode>& r)const
		{
			return r.get();
		}
	};

	using ChildsTable = boost::multi_index_container
		<
			std::shared_ptr<ClsNode>,
			indexed_by
			<
				random_access<> //SQL order
				, ordered_unique< extr_void_ptr_ClsNode >
			>
		>;
	*/
	

	std::shared_ptr<ICls64>			mValue;
	std::weak_ptr<const ClsNode>	mParent;
	std::shared_ptr<ChildsTable>	mChild;

	using sigUpdate = sig::signal < void(const ClsNode&
		, std::shared_ptr<const ICls64>
		, std::shared_ptr<const ICls64>)>;

public:

	sigUpdate sigBeforeValChange;
	sigUpdate sigAfterValChange;

	ClsNode();
	ClsNode(const std::shared_ptr<const ClsNode>& parent);
	ClsNode(const std::shared_ptr<const ClsNode>& parent
		, const std::shared_ptr<ICls64>& value);

	void SetValue(const std::shared_ptr<ICls64>& new_value);
	std::shared_ptr<const ICls64> GetValue()const;
	void ClearChilds();

	void AddChild(const std::shared_ptr<ClsNode>& child);

	std::shared_ptr<const ClsNode> GetParent()const;
	void SetParent(const std::shared_ptr<const ClsNode>& parent);

	const std::shared_ptr<ChildsTable> GetChilds()const
	{
		return mChild;
	}

};



using NotyfyTable = std::vector<const ClsNode*>;

//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H
