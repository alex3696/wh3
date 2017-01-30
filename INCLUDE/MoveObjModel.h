#ifndef __MOVEOBJMODEL_H
#define __MOVEOBJMODEL_H

#include "_pch.h"
#include "db_rec.h"

namespace wh{
//-----------------------------------------------------------------------------
template< class DataType >
class TDataModel
	: public boost::noncopyable
	//, public std::enable_shared_from_this<TDataModel<DataType>>
{
public:
	typedef DataType	DataType;
	using Signal = sig::signal<void(const DataType&, const DataType&)>;
	using Slot = typename Signal::slot_type;


	TDataModel(){};
	TDataModel(DataType&& new_data)
	{
		SetData(std::forward<DataType>(new_data));
	};
	TDataModel(const DataType& new_data)
	{
		SetData(new_data);
	};

	const DataType& GetData() const
	{
		return *mCurrent;
	}
	const DataType* operator->() const
	{
		return mCurrent.get();
	}
	const DataType& operator*() const
	{
		return *mCurrent;
	}

	/*
	const DataType& operator=(DataType& new_data)
	{
	SetData(new_data);
	}
	*/

	void SetData(DataType&& new_data)
	{
		DoSignal(moBeforeUpdate, new_data, *mCurrent);
		mCurrent = std::make_unique<DataType>(std::forward<DataType>(new_data));
		DoSignal(moAfterUpdate, new_data, *mCurrent);
	}
	void SetData(const DataType& new_data)
	{
		DoSignal(moBeforeUpdate, new_data, *mCurrent);
		mCurrent = std::make_unique<DataType>(new_data);
		DoSignal(moAfterUpdate, new_data, *mCurrent);
	}

	sig::connection ConnBU(const Slot &slot)
	{
		return MakeSignal(moBeforeUpdate)->connect(moBeforeUpdate);
	}
	sig::connection ConnAU(const Slot &slot)
	{
		return MakeSignal(moAfterUpdate)->connect(moAfterUpdate);
	}

private:
	using UnqSignal = std::unique_ptr<Signal>;
	using SignalArray = std::array<UnqSignal, 2 >;
	using UnqSignalArray = std::unique_ptr<SignalArray>;

	enum ModelOperation
	{
		moBeforeUpdate = 0,
		moAfterUpdate
	};
	inline UnqSignal& MakeSignal(ModelOperation op)
	{
		size_t	pos = (size_t)op;
		if (!mSignal)
			mSignal.reset(new SignalArray);
		if (!(*mSignal)[pos])
			(*mSignal)[pos].reset(new Signal);
		return (*mSignal)[pos];
	}
	inline void DoSignal(ModelOperation op, const DataType& new_data, const DataType& old_data)
	{
		if (mSignal && (*mSignal)[(size_t)op])
			(*mSignal)[(size_t)op]->operator()(new_data, old_data);
	}

	std::unique_ptr<DataType>	mCurrent;
	UnqSignalArray				mSignal;

};

template<class _Ty, class... _Types>
inline std::shared_ptr<TDataModel<_Ty>> make_shared_model(_Types&&... _Args)
{
	return std::make_shared<TDataModel<_Ty>>(std::forward<_Types>(_Args)...);
}
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







//-----------------------------------------------------------------------------

class Moveable
{
public:
	Moveable();

	bool GetRecentEnable()const;
	void SetRecentEnable(bool enable);
	
	inline const ObjTree& GetRecent()const			
	{ 
		return mRecent;
	}
	inline const ObjTree& GetDst()const
	{
		return mDst;
	}

	inline void SetMoveable(const rec::PathItem& moveable)
	{ 
		mMoveble.SetData(moveable);
	}
	inline const rec::PathItem& GetMoveable()const	{ return mMoveble.GetData(); }


	
	

	
	

	const ObjStore::iterator FindObj(const wxString& str)const;

	
	void Load();
	void Move(const wxString& oid, const wxString& qty);
	void Unlock();

private:
	ObjTree						mDst;
	ObjTree						mRecent;
	TDataModel<rec::PathItem>	mMoveble = rec::PathItem();

};// class Moveable




}//namespace wh{
#endif // __****_H