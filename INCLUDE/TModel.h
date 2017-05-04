#ifndef __TMODEL_H
#define __TMODEL_H

#include "dbFieldType.h"
#include "field_data.h"
#include "db_rec.h"

namespace wh{

//----------------------------------------------------------------------------
enum ModelState
{
	msNull = 0,
	msCreated,	//NEW созданная(новая) модель
	msExist,	//модель, данные которой синхронизированы с хранилищем
	msUpdated,	//измененная модель
	msDeleted,	//удаленная модель

};

//-----------------------------------------------------------------------------
class IModel;
typedef std::shared_ptr<IModel> SptrIModel;

//-----------------------------------------------------------------------------
enum ModelOperation
{
	// Сигналы для элемента
	moBeforeUpdate = 0,
	moAfterUpdate
	// Сигналы для массива
};


template < class DATA >
class SigData
	//: public boost::noncopyable
{
public:
	using Signal = sig::signal<void(const IModel*, const DATA* const)>;
	using Slot = typename Signal::slot_type;

	sig::connection DoConnect(ModelOperation op, const Slot &slot)
	{
		return GetSignal(op)->connect(slot);
	}
	void DoDisconnect(ModelOperation op, const Slot &slot)
	{
		return GetSignal(op)->disconnect(&slot);
	}
protected:
	//void DoSignal(Op op, const IModel* model, DATA* const data)
	void DoSignal(ModelOperation op, const IModel* model, const DATA* data)
	{
		return GetSignal(op)->operator()(model, data);
	}
private:
	struct sig_error : virtual exception_base {};

	using UnqSignal = std::unique_ptr<Signal>;
	using SignalArray = std::array<UnqSignal, 2 >;
	using UnqSignalArray = std::unique_ptr<SignalArray>;

	UnqSignalArray mSignal;

	inline UnqSignal& GetSignal(ModelOperation op)
	{
		unsigned int pos = MAXSIZE_T;
		switch (op)
		{
		case moBeforeUpdate: pos = 0; break;
		case moAfterUpdate: pos = 1; break;
		default:BOOST_THROW_EXCEPTION(sig_error() << wxstr("Out of range SigOp"));break;
		}
		if (!mSignal)
			mSignal.reset(new SignalArray);
		if (!(*mSignal)[pos])
			(*mSignal)[pos].reset(new Signal);
		return (*mSignal)[pos];
	}



	
};
//-------------------------------------------------------------------------

class SigArray
	//: public boost::noncopyable
{
public:
	enum class Op
	{
		BeforeInsert = 0,// $1=nonconst, 2$=NULL
		AfterInsert,		// $1=const, 2$=NULL
		BeforeUpdate,	// $1=new=nonconst, 2$=old=const
		AfterUpdate,		// $1=new=const, 2$=old=nonconst
		BeforeDelete,	// $1=NULL, 2$=old=const
		AfterDelete,		// $1=NULL, 2$=old=nonconst
	};
	using ArrayChange = std::vector <const SptrIModel >;
	// 
	using Signal = sig::signal< void(const IModel&, const ArrayChange&, const ArrayChange&) >;

	using Slot = Signal::slot_type;
	//using Slot = std::function< void(const IModel&, const ArrayChange&, const ArrayChange&) >;

	sig::connection DoConnect(Op op, const Slot &slot)
	{
		CheckSignalOp(op);
		return mSig->at((size_t)op)->connect(slot);
	}
	void DoDisconnect(Op op, const Slot &slot)
	{
		CheckSignalOp(op);
		return mSig->at((size_t)op)->disconnect(&slot);
	}

	void DoSignal(Op op, const IModel& model
		, const ArrayChange& newItem, const ArrayChange& oldItem)
	{
		CheckSignalOp(op);
		mSig->at((size_t)op)->operator()(model, newItem, oldItem);
	}
protected:
private:
	struct sig_error : virtual exception_base {};

	inline void CheckSignalOp(Op op)
	{
		if ((size_t)Op::AfterDelete < (size_t)op)
			BOOST_THROW_EXCEPTION(sig_error() << wxstr("Out of range SigOp"));
		if (!mSig)
			mSig.reset(new SignalArray);
		if (!(*mSig)[(unsigned int)op])
			(*mSig)[(unsigned int)op].reset(new Signal);
	}

	using UnqSignal = std::unique_ptr<Signal>;
	using SignalArray = std::array<UnqSignal, 6>;
	using UnqSignalArray = std::unique_ptr<SignalArray>;

	UnqSignalArray mSig;
};
//-------------------------------------------------------------------------

class IModelNotifier
{
public:
	virtual ~IModelNotifier(){}

	using VecChange = std::vector<unsigned int>;

	using SigVecChange = sig::signal<void(const IModel&, const VecChange&) >;
	using SlotVecChange = SigVecChange::slot_type;

	using SigInsert = sig::signal<void(const IModel&
		,const std::vector<SptrIModel>&, const SptrIModel&) >;
	using SlotInsert = SigInsert::slot_type;
	//using SlotInsert = std::function<void(const IModel&
	//	, const std::vector<SptrIModel>&, const SptrIModel&)>;

	using SigRemove = sig::signal<void(const IModel&, const std::vector<SptrIModel>&)>;
	using SlotRemove = SigRemove::slot_type;


	sig::connection ConnBeforeInsert(const SlotInsert &subscriber)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		if (!mSig->BeforeInsert)
			mSig->BeforeInsert.reset(new SigInsert);
		return mSig->BeforeInsert->connect(subscriber);
	}
	sig::connection ConnAfterInsert(const SlotInsert &subscriber)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		if (!mSig->AfterInsert)
			mSig->AfterInsert.reset(new SigInsert);
		return mSig->AfterInsert->connect(subscriber);
	}
	void DisconnBeforeInsert(const SlotInsert &subscriber)const
	{
		if (mSig && mSig->BeforeInsert)
		{
			mSig->BeforeInsert->disconnect(&subscriber);
			mSig->BeforeInsert.reset(nullptr);
		}
	}
	void DisconnAfterInsert(const SlotInsert &subscriber)const
	{
		if (mSig && mSig->AfterInsert)
		{
			mSig->AfterInsert->disconnect(&subscriber);
			mSig->AfterInsert.reset(nullptr);
		}
	}

	sig::connection ConnectBeforeRemove(const SlotRemove &slot)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		if (!mSig->BeforeRemove)
			mSig->BeforeRemove.reset(new SigRemove);
		return mSig->BeforeRemove->connect(slot);
	}
	sig::connection ConnectAfterRemove(const SlotRemove &slot)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		if (!mSig->AfterRemove)
			mSig->AfterRemove.reset(new SigRemove);
		return mSig->AfterRemove->connect(slot);
	}
	void DisconnectBeforeRemove(const SlotRemove &subscriber)const
	{
		if (mSig && mSig->BeforeRemove)
		{
			mSig->BeforeRemove->disconnect(&subscriber);
			mSig->BeforeRemove.reset(nullptr);
		}
	}
	void DisconnectAfterRemove(const SlotRemove &subscriber)const
	{
		if (mSig && mSig->AfterRemove)
		{
			mSig->AfterRemove->disconnect(&subscriber);
			mSig->AfterRemove.reset(nullptr);
		}
	}
	
	sig::connection ConnectChangeSlot(const SlotVecChange &slot)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		if (!mSig->ChangeChild)
			mSig->ChangeChild.reset(new SigVecChange);
		return mSig->ChangeChild->connect(slot);
	}
	void DisconnectChangeSlot(const SlotVecChange &slot)const
	{
		if (mSig && mSig->AfterRemove)
		{
			mSig->ChangeChild->disconnect(&slot);
			mSig->ChangeChild.reset(nullptr);
		}
	}

protected:
	struct SigImpl
	{
		std::unique_ptr<SigInsert>		BeforeInsert;
		std::unique_ptr<SigInsert>		AfterInsert;

		std::unique_ptr<SigRemove>		BeforeRemove;
		std::unique_ptr<SigRemove>		AfterRemove;

		std::unique_ptr<SigVecChange>	ChangeChild;
	};
	mutable std::unique_ptr<SigImpl>	mSig;
};




//-----------------------------------------------------------------------------




enum ModelOption:char
{
	CommitSave = 0x01,
	CommitLoad = 0x02,
	CascadeLoad = 0x04,
	CascadeSave = 0x08,
	EnableNotifyFromChild = 0x10,
	EnableParentNotify = 0x20
};

//-----------------------------------------------------------------------------

class IModel
	: public boost::noncopyable
	, public FieldsInfo
	, public IModelNotifier
	//, public std::enable_shared_from_this<IModel>
{
protected:
	struct extr_state
	{
		typedef ModelState result_type;
		inline result_type operator()(const std::shared_ptr<IModel>& r)const
		{
			return r->GetState();
		}
	};
	struct extr_voidptr
	{
		typedef const void* result_type;
		inline result_type operator()(const std::shared_ptr<IModel>& r)const 
		{ 
			return r.get(); 
		}
	};
	
	using BaseStore =
		boost::multi_index_container
		<
			std::shared_ptr<IModel>,
			indexed_by
			<
				random_access<>
				, hashed_unique< extr_voidptr >
				, ordered_non_unique< extr_state >
			>
		>;

	using TIdxRnd = nth_index< BaseStore, 0>::type;
	using TIdxPtr = nth_index< BaseStore, 1>::type;
	using TIdxState = nth_index< BaseStore, 2>::type;
	
	IModel* mParent = nullptr;

	std::unique_ptr<BaseStore> mVec;

	char mOption;

	bool InsertWithoutSignal(SptrIModel& newItem, const SptrIModel& itemBefore)
	{
		if (!newItem)
			BOOST_THROW_EXCEPTION(error() << wxstr("Can`t insert nullptr"));

		if (newItem->mParent)
			newItem->mParent->DelChild(newItem);
		newItem->mParent = this;
		if (!mVec)
			mVec.reset(new BaseStore);



		std::pair<RndIterator, bool> pairIterBool;
		if (itemBefore)
		{
			const PtrIdx& ptrIdx = mVec->get<1>();
			CPtrIterator ptrIt = ptrIdx.find(itemBefore.get());
			if (ptrIdx.end() != ptrIt)
			{
				CRndIterator rndIt = mVec->project<0>(ptrIt);
				CRndIterator rndBegin = mVec->cbegin();
				auto pos = std::distance(rndBegin, rndIt);
				pairIterBool = mVec->insert(rndIt, newItem);
			}
			else
				BOOST_THROW_EXCEPTION(error() << wxstr("Can`t find itemBefore"));
		}
		else
			pairIterBool = mVec->emplace_back(newItem);

		if (!pairIterBool.second)
			BOOST_THROW_EXCEPTION(error() << wxstr("Can`t emplace_back "));
		
		return pairIterBool.second;
	}
	bool RemoveWithoutSignal(SptrIModel& remItem)
	{
		if (!mVec || !remItem)
			return false; // BOOST_THROW_EXCEPTION(error() << wxstr("Can`t remove nullptr"));

		PtrIdx& ptrIdx = mVec->get<1>();
		CPtrIterator ptrIt = ptrIdx.find(remItem.get());
		if (ptrIdx.end() != ptrIt) // this own item ?
		{
			ptrIdx.erase(ptrIt);
			return true;
		}
		return false;
	}

public:
	explicit IModel(const char option = ModelOption::EnableParentNotify)
		:mOption(option)
	{}

	struct error : virtual exception_base {};

	typedef TIdxRnd			RndIdx;
	typedef TIdxPtr			PtrIdx;
	typedef TIdxState		StateIdx;


	typedef RndIdx::iterator			RndIterator;
	typedef RndIdx::const_iterator		CRndIterator;
	typedef PtrIdx::iterator			PtrIterator;
	typedef PtrIdx::const_iterator		CPtrIterator;
	typedef StateIdx::iterator			StateIterator;
	typedef StateIdx::const_iterator	CStateIterator;

	virtual ~IModel()
	{
		Clear();
	}
	IModel* GetParent()const
	{
		return mParent;
	}
	size_t  GetChildQty()const
	{
		return mVec ? mVec->size() : 0;
	}
	size_t  size()const
	{
		return mVec ? mVec->size() : 0;
	}
	
	std::shared_ptr<IModel> GetChild(size_t pos)const 
	{
		if (mVec && mVec->size() > pos)
			return (*mVec)[pos]; 
		BOOST_THROW_EXCEPTION(error() << wxstr("Index exceeds"));
		return nullptr;
	}

	template <class CHILD>
	void Insert(const std::vector<std::shared_ptr<CHILD>> & newItems,
		SptrIModel& before = SptrIModel(nullptr))
	{
		DoSigBeforeInsert(newItems, before);
		if (!mVec)
			mVec.reset(new BaseStore);
		mVec->reserve(newItems.size() + mVec->size());
		unsigned int qty_inserted = 0;
		for (const auto& curr : newItems)
		{
			auto newImodel = std::dynamic_pointer_cast<IModel>(curr);
			if (newImodel)
				qty_inserted += InsertWithoutSignal(newImodel, before) ? 1 : 0;
			else
				BOOST_THROW_EXCEPTION(error() << wxstr("Can`t cast to IModel"));
		}
		if (qty_inserted != newItems.size())
			BOOST_THROW_EXCEPTION(error() << wxstr("Not all inserted"));
		DoSigAfterInsert(newItems, before);
	}
	template <class CHILD>
	void Insert(std::shared_ptr<CHILD>& newItem, 
		SptrIModel& before = SptrIModel(nullptr))
	{
		std::vector<SptrIModel> new_vec;
		new_vec.emplace_back(newItem);
		Insert(new_vec, before);
	}

	void DelChild(const std::vector<SptrIModel> & remVec)
	{
		DoSigBefoRemove(remVec);
		unsigned int removedQty = 0;
		for (auto& curr : remVec)
		{
			auto remImodel = std::dynamic_pointer_cast<IModel>(curr);
			if (remImodel)
				removedQty += RemoveWithoutSignal(remImodel) ? 1 : 0;
			else
				BOOST_THROW_EXCEPTION(error() << wxstr("Can`t cast to IModel"));
		}
		if (removedQty != remVec.size())
			BOOST_THROW_EXCEPTION(error() << wxstr("Not all removed"));
		DoSigAfterRemove(remVec);
	}
	
	template <class CHILD>
	void DelChild(std::shared_ptr<CHILD>& remItem)
	{
		std::vector<SptrIModel> remVec;
		remVec.emplace_back(remItem);
		DelChild(remVec);
	}

	std::shared_ptr<IModel> DelChild(size_t pos)
	{
		if (mVec && mVec->size() > pos)
		{
			auto remItem = (*mVec)[pos];
			DelChild(remItem);
			return remItem;
		}
		return std::shared_ptr<IModel>();
	}

	void Clear()
	{
		if (!mVec || mVec->empty())
			return;
		std::vector<SptrIModel> remVec;
		for (auto& remItem : (*mVec))
			remVec.emplace_back(remItem);
			
		DelChild(remVec);
	}


	virtual std::shared_ptr<IModel> CreateChild()
	{
		return std::shared_ptr<IModel>();
	};

	bool GetItemPosition(const IModel* item, size_t& pos)const
	{
		if (!mVec)
			return false;
		const PtrIdx& ptrIdx = mVec->get<1>();
		CPtrIterator ptrIt = ptrIdx.find(item);
		if (ptrIdx.end() != ptrIt)
		{
			CRndIterator rndIt = mVec->project<0>(ptrIt);
			CRndIterator rndBegin = mVec->cbegin();
			pos = std::distance(rndBegin, rndIt);
			return true;
		}
		return false;

	}
	bool GetItemPosition(const std::shared_ptr<IModel>& item_ptr, size_t& pos)const
	{
		return GetItemPosition(item_ptr.get(),pos);
		
	}

	bool GetCanCommitSave()const
	{
		return ModelOption::CommitSave & mOption;
	}
		
	ModelState GetState()const
	{
		auto thisState = GetStateData();
		if (msExist == thisState)
		{
			auto childsState = GetStateChilds();
			switch (childsState)
			{
			default: thisState = msUpdated; break;
			case msExist: case msNull:
				break;
			}
		}
			
			
		return thisState;
	}
	ModelState GetStateChilds()const
	{
		if (!mVec)
			return msExist;
		
		const auto& stateIdx = mVec->get<2>();

		bool haveCreated = stateIdx.end() != stateIdx.find(msCreated);
		bool haveUpdated = stateIdx.end() != stateIdx.find(msUpdated);
		bool haveDeleted = stateIdx.end() != stateIdx.find(msDeleted);

		return (haveCreated || haveUpdated || haveDeleted) ? msUpdated : msExist;
	}
	virtual ModelState GetStateData()const 
	{
		return msExist;
	}

	void Load()
	{
		if (ModelOption::CommitLoad & mOption)
			whDataMgr::GetDB().BeginTransaction();
		LoadData();
		LoadChilds();
		if (ModelOption::CommitLoad & mOption)
			whDataMgr::GetDB().Commit();
	}
	void Save()
	{
		if (ModelOption::CommitSave & mOption)
			whDataMgr::GetDB().BeginTransaction();
		SaveData();
		SaveChilds();
		if (ModelOption::CommitSave & mOption)
			whDataMgr::GetDB().Commit();
	}
	
	void MarkDeleted()
	{
		MarkDeletedData();
		MarkDeletedChilds();
	}	
	virtual void MarkDeletedData(){}
	void MarkDeletedChilds()
	{
		if (!mVec)
			return;
		for (auto& child : *mVec)
			child->MarkDeleted();
	}

	void MarkSaved()
	{
		MarkSavedData();
		MarkSavedChilds();
	}
	virtual void MarkSavedData(){}
	void MarkSavedChilds()
	{
		if (!mVec)
			return;
		for (auto& child : *mVec)
			child->MarkSaved();
	}

protected:
	virtual void LoadData(){}
	virtual void SaveData(){}

	virtual void LoadChilds()
	{
		// отключаем уведомление родителей, чтоб несколько раз не уведомлять
		// на Clear, Insert, а потом после обновления каждого чилда
		auto enableParentNotify = (mOption & ModelOption::EnableParentNotify);
		mOption = mOption & ~ModelOption::EnableParentNotify;

		Clear(); // очищаем, если у детишек есть клиенты, то они отвалятся
		// загружаем детишек
		wxString query;
		const bool queryExist = GetSelectChildsQuery(query);
		if (queryExist)
		{
			auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
			if (table)
			{
				std::vector<SptrIModel> new_vec;
				SptrIModel child;
				unsigned int rowQty = table->GetRowCount();
				
				if (rowQty)
				{
					new_vec.reserve(rowQty);
					for (unsigned int i = 0; i < rowQty; ++i)
					{
						child = CreateChild();
						if (child && LoadChildDataFromDb(child, table, i))
						{
							child->MarkSaved();
							new_vec.emplace_back(child);
						}
					}//for (auto i = 0; i < rowQty; ++i)
					Insert(new_vec);
				}//if (rowQty)

			}//if(table)
		}//if (queryExist)
		
		// загружаем каскадно детишек
		if (mVec && mVec->size() && (mOption & ModelOption::CascadeLoad))
			for (auto item : *mVec)
				item->LoadChilds();
		if (enableParentNotify)// включаем уведомления родителя как было
			mOption = mOption | ModelOption::EnableParentNotify;
		// 3 Каскадно уведомить родителей об изменении
		DoNotifyParent();


	}
	virtual void SaveChilds()
	{
		if (!mVec)
			return;

		SaveRange(msCreated);
		SaveRange(msUpdated);
		SaveRange(msDeleted);

		auto& stateIdx = mVec->get<2>();
		auto range = stateIdx.equal_range(msNull);
		auto qty = std::distance(range.first, range.second);
		size_t i = 0;
		std::vector< std::shared_ptr<IModel>>     tmpVec(qty);
		while (range.first != range.second)
		{
			auto item = *range.first;
			tmpVec[i] = item;
			++i;
			++range.first;
		}
		DelChild(tmpVec);
	}

	
	void OnChildChage(IModel* sender)
	{
		if (mVec && (mOption & ModelOption::EnableNotifyFromChild))
		{
			PtrIdx& ptrIdx = mVec->get<1>();
			CPtrIterator ptrIt = ptrIdx.find(sender);
			if (ptrIdx.end() != ptrIt)
			{
				StateIterator stateIt = mVec->project<2>(ptrIt);
				std::function<void(ModelState state)> stateChanger = [](ModelState state){};
				bool isStateModified = mVec->get<2>().modify_key(stateIt, stateChanger);
				if (isStateModified)
				{
					unsigned int pos;
					if (GetItemPosition(sender, pos))
					{
						std::vector< unsigned int>   sigVec;
						sigVec.emplace_back(pos);
						if (mSig && mSig->ChangeChild)
							(*mSig->ChangeChild)(*this, sigVec);
						DoNotifyParent();
					}// if (GetItemPosition(sender, pos))
				}//if (isStateModified)
			}//if (ptrIdx.end() != ptrIt)
		}//if ((mOption & ModelOption::EnableNotifyFromChild) && mVec)
	}

	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& db, const size_t pos)
	{
		return false;
	};
	virtual bool GetSelectChildsQuery(wxString& query)const
	{
		return false;
	}
		
	void DoNotifyParent()
	{
		if (mParent && (mOption & ModelOption::EnableParentNotify))
			mParent->OnChildChage(this);
	}
	void DoSigAfterRemove(const std::vector<SptrIModel>& sigVec)
	{
		if (mSig && mSig->AfterRemove)
			(*mSig->AfterRemove)(*this, sigVec);
		DoNotifyParent();
	}
	void DoSigBefoRemove(const std::vector<SptrIModel>& sigVec)
	{
		if (mSig && mSig->BeforeRemove)
			(*mSig->BeforeRemove)(*this, sigVec);
		DoNotifyParent();
	}

	void DoSigBeforeInsert(const std::vector<SptrIModel>& new_vec
		, const std::shared_ptr<IModel>& itemBefore)
	{
		if (mSig && mSig->BeforeInsert)
			(*mSig->BeforeInsert)(*this, new_vec, itemBefore);
		DoNotifyParent();
	}
	void DoSigAfterInsert(const std::vector<SptrIModel>& new_vec
		, const std::shared_ptr<IModel>& itemBefore)
	{
		if (mSig && mSig->AfterInsert)
			(*mSig->AfterInsert)(*this, new_vec, itemBefore);
		DoNotifyParent();
	}

	void SaveRange(ModelState state)
	{
		StateIdx& stateIdx = mVec->get<2>();
		auto range = stateIdx.equal_range(state);
		auto qty = std::distance(range.first, range.second);
		if (!qty)
			return;

		size_t i = 0;
		std::vector< std::shared_ptr<IModel>>     tmpVec(qty);
		while (range.first != range.second)
		{
			auto item = *range.first;
			tmpVec[i] = item;
			++i;
			++range.first;
		}
		for (const auto& item : tmpVec)
		{
			item->Save();
		}


	}

};


//-----------------------------------------------------------------------------
template< typename T_Data, bool mItemWithId = true >
class TModelData
	: public IModel
	, public SigData<T_Data>
{
public:
	TModelData(const char option = ModelOption::EnableParentNotify)
		:IModel(option)
	{}

	typedef T_Data	 T_Data;
	using DataType = T_Data;

	bool HaveStored()const
	{
		return mStored.operator bool();
	}

	const T_Data& GetStored() const
	{
		if (mStored)
			return *mStored.get();
		BOOST_THROW_EXCEPTION(error() << wxstr("No model data, msNull"));
	}
	const T_Data& GetData() const
	{
		if (mCurrent) // msCreated msUpdated msExist
			return *mCurrent.get();
		return GetStored();
	}


	void SetData(std::shared_ptr<T_Data> curr_ptr, bool stored = false, bool notify=true)
	{
		switch (GetState())
		{
		default: case msDeleted: break;
		case msNull: case msUpdated: case msCreated: case msExist:
			mCurrent = curr_ptr;
			if (stored)
				mStored = mCurrent;
			if (notify)
			{
				DoSignal(moAfterUpdate, this, curr_ptr.get());
				DoNotifyParent();
			}
			break;
		}
	}
	
	void SetData(const T_Data& current, bool stored=false)
	{
		switch (GetState())
		{
		default: case msDeleted: break;

		case msNull:
		case msExist:
			mCurrent = std::make_shared<T_Data>();
		case msUpdated: case msCreated:
			*mCurrent.get() = current;
			if (stored)
				mStored = mCurrent;
			//DoSigChangeData();
			DoSignal(moAfterUpdate, this, &current);
			DoNotifyParent();
			break;
		}
	}
	//void SetData(std::shared_ptr<T_Data>& stored, std::shared_ptr<T_Data>& current)
	//{
	//	mStored = stored;
	//	mCurrent = current;
	//	DoSignal(moAfterUpdate, this, nullptr);
	//	DoNotifyParent();
	//}
	virtual void MarkDeletedData()override
	{
		mCurrent = nullptr;
		//DoSigChangeData();
		DoSignal(moAfterUpdate, this, nullptr);
		DoNotifyParent();
	}
	virtual void MarkSavedData()override
	{
		if (mStored != mCurrent)
		{
			mStored = mCurrent;
			//DoSigChangeData();
			DoSignal(moAfterUpdate, this, mCurrent.get());
			DoNotifyParent();
		}
	}

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>& db, const size_t pos)
	{
		return false;
	}
protected:
	virtual ModelState GetStateData()const override final
	{
		return wh::GetState(mStored, mCurrent);
	}
	virtual void SaveData()override 
	{
		switch (GetStateData())
		{
		default:	BOOST_THROW_EXCEPTION(error() << wxstr("unknown model state"));//break;
		case  msExist:						break;
		case  msCreated:	ExecInsert();	break;
		case  msUpdated:	ExecUpdate();	break;
		case  msDeleted:	ExecDelete();	break;
		}//switch	
	}
	virtual void LoadData()
	{
		wxString query;
		const bool queryExist = GetSelectQuery(query);
		if (queryExist)
		{
			auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
			if (table && table->GetRowCount() )
			{
				auto stored = std::make_shared<T_Data>();
				if (LoadThisDataFromDb(table, 0))
					MarkSaved();
			}
		}
	}

	virtual bool GetSelectQuery(wxString&)const	
	{
		return false;	
	}
	virtual bool GetInsertQuery(wxString&)const
	{
		return false;
	}
	virtual bool GetUpdateQuery(wxString&)const
	{
		return false;
	}
	virtual bool GetDeleteQuery(wxString&)const
	{
		return false;
	}
	
	//virtual void LoadChilds()override{};
	//virtual void SaveChilds()override{};
private:
	void ExecSaveWithoutResult(const wxString& query)
	{
		int result = whDataMgr::GetDB().Exec(query, false);
		if (!result)
			MarkSavedData();
		else
			BOOST_THROW_EXCEPTION(error() << wxstr("\ncan`t execute:\n" + query));
	}
	void ExecInsert()
	{
		wxString query;
		const bool queryExist = GetInsertQuery(query);
		if (queryExist)
		{
			if (mItemWithId)
			{
				auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
				if (table)
				{
					auto stored = std::make_shared<T_Data>();
					if (LoadThisDataFromDb(table, 0))
						MarkSavedData();
				}
				else
					BOOST_THROW_EXCEPTION(error() << wxstr("\ncan`t execute:\n" + query));
			}
			else
				ExecSaveWithoutResult(query);

		}// if (queryExist)
	}
	void ExecUpdate()
	{
		wxString query;
		const bool queryExist = GetUpdateQuery(query);
		if (queryExist)
			ExecSaveWithoutResult(query);
	}
	void ExecDelete()
	{
		wxString query;
		const bool queryExist = GetDeleteQuery(query);
		if (queryExist)
			ExecSaveWithoutResult(query);
	}
	
	/*
	void DoSigChangeData()
	{
		if (mSig)
			mSig->Change(*this);
		DoNotifyParent();
	}
	*/

	std::shared_ptr<T_Data>		mStored;
	std::shared_ptr<T_Data>		mCurrent;
	
};










































//-----------------------------------------------------------------------------
template< typename T_Data >
static ModelState GetState(const T_Data& stored, const T_Data& current)
{
	//!mStored && !mCurrent								msNull
	//!mStored && mCurrent								msCreated
	//mStored && !mCurrent								msDeleted
	//mStored && mCurrent && mStored == mCurrent		msExist
	//mStored && mCurrent && mStored != mCurrent		msUpdated
	if (stored)
	{
		if (current)
		{
			return current == stored ? msExist : msUpdated;
		}
	}
	else
		return current ? msCreated : msNull;

	return msDeleted;
}
//-----------------------------------------------------------------------------







}//namespace wh{
#endif // __****_H