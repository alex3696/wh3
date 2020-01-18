#ifndef __MOVEOBJMODEL_H
#define __MOVEOBJMODEL_H

#include "_pch.h"
#include "MoveObjData.h"
#include "ModelBrowser.h"

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

class ModelMoveExecWindow : public IModelWindow
{
	int	 mCurrentPage = 0;
	std::set<ObjectKey> mObjects;
	ObjRec64 mDstObject;

	//ModelActBrowserWindow::FuncActivateCallback mOnActivateDst;
	void UnlockObjectsWithoutTransaction();

	int BuildExecQuery(wxString& query);

	ObjTree		mDst;
	ObjTree		mRecent;


public:
	std::shared_ptr<ModelBrowser>			mModelObjBrowser;

	ModelMoveExecWindow();
	~ModelMoveExecWindow();
	void LockObjects(const std::set<ObjectKey>& obj);
	void UnlockObjects();
	bool GetRecentEnable()const;
	void SetRecentEnable(bool enable);
	//void DoShowDstPage();
	//void DoSelectDst();
	void DoExecute();
	
	
	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Init()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

	sig::signal<void(int)> sigSelectPage;

	sig::signal<void(const ObjTree& tree)>	sigUpdateRecent;
	sig::signal<void(const ObjTree& tree)>	sigUpdateDst;
	sig::signal<void(bool)>					sigEnableRecent;
	sig::signal<void(std::set<int64_t>&)>	sigGetSelection;



};// class ModelMoveExecWindow




}//namespace wh{
#endif // __****_H