#ifndef __TMODEL_H
#define __TMODEL_H

#include "_pch.h"
#include "whDB.h"
#include "globaldata.h"

namespace wh{

namespace sig = boost::signals2;
//-----------------------------------------------------------------------------
enum FieldType
{
	ftText =	0,
	ftNumber=	1,
	ftDate =	2,
	ftLink =	3,
	ftFile =	4,
	
	ftName =	5,
	ftInt =		6,
	ftfloat =	7
};


static FieldType StringToFt(const wxString& str)
{
	unsigned long tmp = 0;
	if( str.ToULong(&tmp) )
		return FieldType(tmp);
	return ftName;
}



//-----------------------------------------------------------------------------
struct Field
{
	Field(const char* name, FieldType ft, bool show)
	:mName(name), mType(ft), mShow(show)
	{}


	Field(const wxString& name, FieldType ft, bool show)
		:mName(name), mType(ft), mShow(show)
	{}
	wxString	mName;
	FieldType	mType;
	bool		mShow;
};
//-----------------------------------------------------------------------------
const static std::vector<Field> gEmptyFieldVec;

class FieldsInfo
{
public:
	virtual ~FieldsInfo()
	{
	}

	virtual const std::vector<Field>& GetFieldVector()const
	{
		return gEmptyFieldVec;
	}
	unsigned int GetFieldQty()const
	{
		return GetFieldVector().size();
	}
	const wxString& GetFieldName(unsigned int col)const
	{
		return GetFieldVector()[col].mName;
	}
	const FieldType& GetFieldType(unsigned int col)const
	{
		return GetFieldVector()[col].mType;
	}
	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)
	{
		return false;
	}

};

//-----------------------------------------------------------------------------
class IModel;


//-----------------------------------------------------------------------------
/// Сигналы для элемента
//enum SigDataOp
//{
//	sdoBeforeUpdate = 0,
//	sdoAfterUpdate
//};


template < class DATA >
class SigData
	//: public boost::noncopyable
{
public:
	enum class Op
	{
		BeforeChange = 0,
		AfterChange
	};

	//using Signal = sig::signal<void(const IModel*, DATA* const)>;
	using Signal = sig::signal<void(const IModel*, const DATA*)>;
	//using Slot = std::function< void(const IModel*, const DATA*) >;
	using Slot = typename Signal::slot_type;

	sig::connection DoConnect(Op op, const Slot &slot)
	{
		return GetSignal(op)->connect(slot);
	}
	void DoDisconnect(Op op, const Slot &slot)
	{
		return GetSignal(op)->disconnect(&slot);
	}
protected:
	//void DoSignal(Op op, const IModel* model, DATA* const data)
	void DoSignal(Op op, const IModel* model, const DATA* data)
	{
		return GetSignal(op)->operator()(model, data);
	}
private:
	struct sig_error : virtual exception_base {};

	using UnqSignal = std::unique_ptr<Signal>;
	using SignalArray = std::array<UnqSignal, 2 >;
	using UnqSignalArray = std::unique_ptr<SignalArray>;

	UnqSignalArray mSignal;

	inline UnqSignal& GetSignal(Op op)
	{
		unsigned int pos = MAXSIZE_T;
		switch (op)
		{
		case Op::BeforeChange: pos = 0; break;
		case Op::AfterChange: pos = 1; break;
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

		BeforeLoad,
		AfterLoad,
		BeforeSave,
		AfterSave,
		
		BeforeClear,
		AfterClear
	};
	using ArrayChange = std::vector <const IModel >;

	using Signal = sig::signal< void(const IModel*, const ArrayChange*, const ArrayChange*) >;

	using Slot = Signal::slot_type;
	//using Slot = std::function< void(const IModel*, const ArrayChange*, const ArrayChange*) >;

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

	void DoSignal(Op op, const IModel* model
		, const ArrayChange* newItem, const ArrayChange* oldItem)
	{
		CheckSignalOp(op);
		mSig->at((size_t)op)->operator()(model, newItem, oldItem);
	}
protected:
private:
	struct sig_error : virtual exception_base {};

	inline void CheckSignalOp(Op op)
	{
		if ((size_t)Op::AfterClear < (size_t)op)
			BOOST_THROW_EXCEPTION(sig_error() << wxstr("Out of range SigOp"));
		if (!mSig)
			mSig.reset(new SignalArray);
		if (!(*mSig)[(unsigned int)op])
			(*mSig)[(unsigned int)op].reset(new Signal);
	}

	using UnqSignal = std::unique_ptr<Signal>;
	using SignalArray = std::array<UnqSignal, 12>;
	using UnqSignalArray = std::unique_ptr<SignalArray>;

	UnqSignalArray mSig;
};
//-------------------------------------------------------------------------

class IModelNotifier
{
public:
	virtual ~IModelNotifier(){}

	using SigChange = sig::signal<void(const IModel&) >;
	using SlotChange = std::function<void(const IModel&)>;

	using VecChange = std::vector<unsigned int>;
	using SigVecChange = sig::signal<void(const IModel&, const VecChange&) >;
	using SlotVecChange = std::function<void(const IModel&, const VecChange&)>;

	/*
	sig::connection ConnectChangeDataSlot(const SlotChange &subscriber)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		return mSig->Change.connect(subscriber);
	}
	*/

	sig::connection ConnectAppendSlot(const SlotVecChange &subscriber)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		return mSig->AppendChild.connect(subscriber);
	}
	sig::connection ConnectBeforeRemove(const SlotVecChange &slot)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		return mSig->BeforeRemove.connect(slot);

	}

	sig::connection ConnectRemoveSlot(const SlotVecChange &subscriber)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		return mSig->RemoveChild.connect(subscriber);

	}
	sig::connection ConnectChangeSlot(const SlotVecChange &subscriber)const
	{
		if (!mSig)
			mSig.reset(new SigImpl);
		return mSig->ChangeChild.connect(subscriber);
	}

	/*
	inline void DisconnectChangeDataSlot(const SlotVecChange &subscriber)const
	{
		if (mSig)
			mSig->Change.disconnect(&subscriber);
	}
	*/

	inline void DisconnectAppendSlot(const SlotVecChange &subscriber)const
	{
		if (mSig)
			mSig->AppendChild.disconnect(&subscriber);
	}
	inline void DisconnectRemoveSlot(const SlotVecChange &subscriber)const
	{
		if (mSig)
			mSig->RemoveChild.disconnect(&subscriber);
	}
	inline void DisconnectBeforeRemove(const SlotVecChange &subscriber)const
	{
		if (mSig)
			mSig->BeforeRemove.disconnect(&subscriber);
	}
	inline void DisconnectChangeSlot(const SlotVecChange &subscriber)const
	{
		if (mSig)
			mSig->ChangeChild.disconnect(&subscriber);
	}

protected:
	struct SigImpl
	{
		SigVecChange	AppendChild;
		SigVecChange	BeforeRemove;
		SigVecChange	RemoveChild;
		SigVecChange	ChangeChild;
		//SigChange		Change;
	};

	mutable std::unique_ptr<SigImpl>	mSig;
};



//-----------------------------------------------------------------------------

static int GetColumnWidthBy(FieldType ft)
{
	switch (ft)
	{
	case wh::ftText:	return	-1;		break;
	case wh::ftNumber:	return	50;		break;
	case wh::ftDate:	return	100;	break;
	case wh::ftLink:	return	80;		break;
	case wh::ftFile:	return	80;		break;
	
	case wh::ftName:	return	150;	break;
	case wh::ftInt:		return	50;		break;
	case wh::ftfloat:	return	80;		break;
	default:break;
	}
	return -1;
}


//-----------------------------------------------------------------------------




namespace ModelOption
{
	const char CommitSave = 0x01;
	const char CommitLoad = 0x02;
	const char CascadeLoad = 0x04;
	const char CascadeSave = 0x08;
	const char EnableNotifyFromChild = 0x10;
	const char EnableParentNotify = 0x20;
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
		inline result_type operator()(const std::shared_ptr<IModel>& r)const { return r.get(); }
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

	bool AppendChildWithoutSignal(std::shared_ptr<IModel>& newItem)
	{
		if (!newItem)
			BOOST_THROW_EXCEPTION(error() << wxstr("Can`t add nullptr"));

		if (newItem->mParent)
			newItem->mParent->DelChild(newItem);

		newItem->mParent = this;
		if (!mVec)
			mVec.reset(new BaseStore);

		
		std::pair<RndIterator, bool> pairIterBool = mVec->emplace_back(newItem);
		return pairIterBool.second;
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
	
	std::shared_ptr<IModel> GetChild(size_t pos)const 
	{
		if (mVec && mVec->size() > pos)
			return (*mVec)[pos]; 
		BOOST_THROW_EXCEPTION(error() << wxstr("Index exceeds"));
		return nullptr;
	}
	void AddChild(std::shared_ptr<IModel>& newItem)
	{
		if (AppendChildWithoutSignal(newItem))
		{
			unsigned int pos = mVec->size()-1;
			VecChange sigVec;
			sigVec.emplace_back(pos);
			DoSigAppendChild(sigVec);
		}
	}
	template <class CHILD>
	void AddChild(std::shared_ptr<CHILD>& newItem)
	{
		AddChild(std::dynamic_pointer_cast<IModel>(newItem));
	}
	std::shared_ptr<IModel> DelChild(std::shared_ptr<IModel>& remItem)
	{
		if (!mVec)
			return nullptr;

		PtrIdx& ptrIdx = mVec->get<1>();
		CPtrIterator ptrIt = ptrIdx.find(remItem.get() );
		if (ptrIdx.end() != ptrIt) // this own item ?
		{
			unsigned int pos;
			bool itemHavePos = GetItemPosition(remItem, pos);

			if (itemHavePos)
			{
				VecChange sigVec;
				sigVec.emplace_back(pos);
				DoSigBefoRemove(sigVec);

				remItem->mParent = nullptr;
				ptrIdx.erase(ptrIt);
						
				DoSigRemoveChild(sigVec);

				return remItem;
			}
		}
		return nullptr;
	}
	std::shared_ptr<IModel> DelChild(size_t pos)
	{
		if (mVec && mVec->size() > pos)
		{
			auto remItem = (*mVec)[pos];
			VecChange sigVec;
			sigVec.emplace_back(pos);
			DoSigBefoRemove(sigVec);

			remItem->mParent = nullptr;
			mVec->erase(mVec->begin() + pos);
			
			DoSigRemoveChild(sigVec);

			return remItem;
		}
		return std::shared_ptr<IModel>();
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
	void Clear()
	{
		if (!mVec || mVec->empty() )
			return;

		auto vecSize = mVec->size();
		std::vector<unsigned int> tmpVec(vecSize);

		for (unsigned int i = 0; i < mVec->size(); ++i)
		{
			auto item = (*mVec)[i];
			item->mParent = nullptr;
			tmpVec[i] = i;
		}
		DoSigBefoRemove(tmpVec);
		mVec->clear();
		mVec.reset(nullptr);
		DoSigRemoveChild(tmpVec);
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
		LoadData();
		LoadChilds();
	}
	void Save()
	{
		SaveData();
		SaveChilds();
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
		// 1 отключаем уведомления родителю, если включено, нет необходимости дёргать это событие более одного раза
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
				VecChange sigVec;
				unsigned int rowQty = table->GetRowCount();
				if (rowQty)
				{
					if (!mVec)
						mVec.reset(new BaseStore);

					unsigned int dbNo = 0;
					unsigned int elemNo = 0;
					while (dbNo < rowQty)
					{
						std::shared_ptr<IModel> child = CreateChild();
						if (child && LoadChildDataFromDb(child, table, dbNo))
						{
							child->MarkSaved();
							if (AppendChildWithoutSignal(child))
								sigVec.emplace_back(elemNo++);
						}
						dbNo++;
					}

				}//if (rowQty)

				// загружаем каскадно детишек
				if (rowQty && (mOption & ModelOption::CascadeLoad))
					for (auto item : *mVec)
						item->LoadChilds();

				DoSigAppendChild(sigVec);
			}//if(table)
		}
		// 3 Каскадно уведомить родителей об изменении
		if (enableParentNotify)
			mOption = mOption | ModelOption::EnableParentNotify;
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
		for (auto& item : tmpVec)
			DelChild(item);


		//stateIdx.erase(range.first, range.second);

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
						if (mSig)
							mSig->ChangeChild(*this, sigVec);
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
	void DoSigAppendChild(VecChange& sigVec)
	{
		if (mSig)
			mSig->AppendChild(*this, sigVec);
		DoNotifyParent();
	}
	void DoSigRemoveChild(VecChange& sigVec)
	{
		if (mSig)
			mSig->RemoveChild(*this, sigVec);
		DoNotifyParent();
	}
	void DoSigBefoRemove(VecChange& sigVec)
	{
		if (mSig)
			mSig->BeforeRemove(*this, sigVec);
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
			DoSignal(Op::AfterChange, this, &current);
			DoNotifyParent();
			break;
		}
	}
	//void SetData(std::shared_ptr<T_Data>& stored, std::shared_ptr<T_Data>& current)
	//{
	//	mStored = stored;
	//	mCurrent = current;
	//	DoSigChangeData();
	//}
	virtual void MarkDeletedData()override
	{
		mCurrent = nullptr;
		//DoSigChangeData();
		DoSignal(Op::AfterChange, this, nullptr);
		DoNotifyParent();
	}
	virtual void MarkSavedData()override
	{
		if (mStored != mCurrent)
		{
			mStored = mCurrent;
			//DoSigChangeData();
			DoSignal(Op::AfterChange, this, mCurrent.get() );
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
//-------------------------------------------------------------------------------------------------
template< typename T_Data  >
class TModelDB
	: public boost::noncopyable
{
public:

	struct error : virtual exception_base {};
	typedef T_Data						T_Data;

	const T_Data& GetStored() const
	{
		if (!mStored)
			BOOST_THROW_EXCEPTION(error() << wxstr("No model data, msNull"));
		return *mStored.get();
	}
	const T_Data& GetData() const
	{
		if (mCurrent) // msCreated msUpdated msExist
			return *mCurrent.get();
		return GetStored();
	}
	ModelState	  GetState()const
	{
		auto state = ::GetState(mStored, mCurrent);
		
		if (msExist == state && msExist != GetStateNested() )
			state = msUpdated;
		return state;
	}
	void SetData(const T_Data& data)
	{
		switch (GetState())
		{
		default: case msNull: case msDeleted: break;
		case msExist:
			mCurrent.reset(new T_Data);
		case msUpdated: case msCreated:
			*mCurrent.get() = data;
			DoSigChange();
			break;
		}
	}
	void SetData(std::shared_ptr<T_Data>& stored, std::shared_ptr<T_Data>& current)
	{
		mStored = stored;
		mCurrent = current;
		DoSigChange();
	}

	void Load()
	{
		LoadThis();
		LoadNested();
	}
	void Save()
	{
		SaveThis();
		SaveNested();
	}


	void MarkDeleted()
	{
		mCurrent = nullptr;
		DoSigChange();
	}
	void MarkSaved()
	{
		mStored = mCurrent;
		DoSigChange();
	}
private:
	
	std::shared_ptr<T_Data>		mStored;
	std::shared_ptr<T_Data>		mCurrent;

protected:
	virtual void SaveThis(){}
	virtual void SaveNested(){}

	virtual void LoadThis(){}
	virtual void LoadNested(){}


	virtual ModelState	  GetStateNested()const
	{
		return msExist;
	}

	TModelDB(const std::shared_ptr<T_Data>& stored = nullptr,
		const std::shared_ptr<T_Data>& current = nullptr)
		:mStored(stored), mCurrent(current)
	{}

	virtual void DoSigChange()const = 0;

};






//-------------------------------------------------------------------------------------------------
#define MODEL_SIGNALIMPL(TYPE)\
protected:\
	using SigChange = signal<void(const TYPE&) >; \
	mutable SigChange	sigChange; \
	virtual void DoSigChange()const override\
{\
	sigChange(*static_cast<const TYPE*>(this)); \
}\
public:\
	using SlotChange = std::function<void(const TYPE&)>; \
	inline connection ConnectChangeSlot(const SlotChange& subscriber, bool signal = true)const\
{\
	connection conn = sigChange.connect(subscriber); \
if (signal)\
	DoSigChange(); \
	return conn; \
}\
	inline void DisconnectChangeSlot(const SlotChange& subscriber)const\
{\
	sigChange.disconnect(&subscriber); \
}


//-------------------------------------------------------------------------------------------------
#define MODEL_CREATEIMPL(TYPE)\
public:\
	static std::shared_ptr<TYPE> Create( \
		std::shared_ptr<T_Data>& current = std::shared_ptr<T_Data>(new T_Data), \
		std::shared_ptr<T_Data>& stored = std::shared_ptr<T_Data>(nullptr) )\
	{\
		TYPE*	newModel = new TYPE(stored, current); \
		return std::shared_ptr<TYPE>(newModel); \
	}



//-------------------------------------------------------------------------------------------------
#define INHERIT_MODEL(BASE,TYPE)\
protected:\
TYPE(const std::shared_ptr<T_Data>& stored = nullptr, const std::shared_ptr<T_Data>& current = nullptr)\
	: BASE (stored, current)\
{} \
MODEL_CREATEIMPL(TYPE)\
MODEL_SIGNALIMPL(TYPE)



}//namespace wh{
#endif // __****_H