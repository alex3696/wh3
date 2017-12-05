#ifndef __MOVEOBJMODEL_H
#define __MOVEOBJMODEL_H

#include "_pch.h"
#include "MoveObjData.h"

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
		auto val = std::make_unique<DataType>(std::forward<DataType>(new_data));
		val.swap(mCurrent);
		DoSignal(moAfterUpdate, *mCurrent, *val);
	}
	void SetData(const DataType& new_data)
	{
		DoSignal(moBeforeUpdate, new_data, *mCurrent);
		auto val = std::make_unique<DataType>(new_data);
		val.swap(mCurrent);
		DoSignal(moAfterUpdate, *mCurrent, *val);
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
	bool						mLock = false;
};// class Moveable




}//namespace wh{
#endif // __****_H