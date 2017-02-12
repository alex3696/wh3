#ifndef __MOVEOBJDATA_H
#define __MOVEOBJDATA_H

#include "_pch.h"
#include "db_rec.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace wh{

//-----------------------------------------------------------------------------

class Type
{
public:
	Type(const wxString& id, const wxString& title)
		:mId(id), mTitle(title)
	{}
	wxString	mId;
	wxString	mTitle;
};
//-----------------------------------------------------------------------------

class Obj 
{
public:
	Obj(const Type* type, const wxString& id, const wxString& title, const wxString& path)
		:mType(type), mId(id), mTitle(title), mPath(path)
	{}
	const Type*	mType;
	wxString	mId;
	wxString	mTitle;
	wxString	mPath;
	
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
struct extr_type_id
{
	typedef const wxString& result_type;
	inline result_type operator()(const Type& r)const
	{
		return r.mId;
	}
};

using TypeStore =
	boost::multi_index_container
	<
		Type,
		indexed_by
		<
		   random_access<> //SQL order
		   , ordered_unique< extr_type_id >
		>
	>;
//-----------------------------------------------------------------------------
struct extr_obj_id
{
	typedef const wxString& result_type;
	inline result_type operator()(const Obj& r)const
	{
		return r.mId;
	}
};



struct extr_string
{
	typedef const wxString& result_type;
	inline result_type operator()(const Obj& r)const
	{
		return r.mType->mId;
	}
};


using ObjStore =
	boost::multi_index_container
	<
		Obj,
		indexed_by
		<
			random_access<> //SQL order
			, ordered_unique< extr_obj_id >
			, ordered_non_unique < extr_string >
		>
	>;

using IdxObjRnd = nth_index< ObjStore, 0>::type;
using IdxObjId = nth_index< ObjStore, 1>::type;
using IdxObjType = nth_index< ObjStore, 2>::type;

//-----------------------------------------------------------------------------

class ObjTree
{
public:

	TypeStore	mType;
	ObjStore	mObj;

	void Clear()
	{
		mObj.clear();
		mType.clear();
	}

	std::pair<IdxObjType::const_iterator, IdxObjType::const_iterator>
		GetTypeRange(const wxString& type_id)const
	{
		const auto& idxObjType = mObj.get<2>();
		return idxObjType.equal_range(type_id);
	}
};


}//namespace wh{
#endif // __****_H