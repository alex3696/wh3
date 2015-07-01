#ifndef __TVEC_H
#define __TVEC_H

#include "TModel.h"

namespace wh{

	using namespace boost::signals2;

//-------------------------------------------------------------------------------------------------
template < class T >
struct extractor_state
{
	typedef ModelState result_type;
	inline result_type operator()(const std::shared_ptr<T>& r)const { return r->GetState(); }
};

//-------------------------------------------------------------------------------------------------
template < class T >
struct extractor_voidptr
{
	typedef const void* result_type;
	inline result_type operator()(const std::shared_ptr<T>& r)const { return r.get(); }
};

//-------------------------------------------------------------------------------------------------
template < typename T > using TBaseTable =
boost::multi_index_container
	<
		std::shared_ptr< T >,
		indexed_by
		<
			random_access<>
			, hashed_unique< extractor_voidptr<T> >
			, ordered_non_unique< extractor_state<T> >
		>
	>;

template < typename T > using TRndIdx = typename nth_index< TBaseTable<T>, 0>::type;
template < typename T > using TPtrIdx = typename nth_index< TBaseTable<T>, 1>::type;
template < typename T > using TStateIdx = typename nth_index< TBaseTable<T>, 2>::type;

//-------------------------------------------------------------------------------------------------
template< typename T_Item, bool commitSave=true, bool itemWithId=true >
class TVecDB
	: public boost::noncopyable
{
public:
	

	struct error : virtual exception_base {};

	typedef T_Item					T_Item;
	typedef typename T_Item::T_Vec	T_Vec;
	typedef typename T_Item::T_Data	T_Data;

	typedef TRndIdx<T_Item>			RndIdx;
	typedef TPtrIdx<T_Item>			PtrIdx;
	typedef TStateIdx<T_Item>		StateIdx;

	typedef typename RndIdx::iterator			RndIterator;
	typedef typename RndIdx::const_iterator		CRndIterator;
	typedef typename PtrIdx::iterator			PtrIterator;
	typedef typename PtrIdx::const_iterator		CPtrIterator;
	typedef typename StateIdx::iterator			StateIterator;
	typedef typename StateIdx::const_iterator	CStateIterator;


	const std::function<void(const T_Item&)> mOnItemChangeSlot;
	TVecDB()
		:mOnItemChangeSlot(std::bind(&TVecDB::OnItemChange, this, std::placeholders::_1))
	{}
	~TVecDB()
	{
		Clear();
	}

	// modifiers:
	void	AppendItem(const std::vector<std::shared_ptr<T_Item>>& itemVec)
	{
		unsigned int pos = mVecData.size();
		std::vector<unsigned int> tmpVec(itemVec.size());
		auto it = tmpVec.begin();
		mVecData.reserve(pos + itemVec.size());
		for (const std::shared_ptr<T_Item>& newItem : itemVec)
		{
			*it++ = pos++;
			
			mVecData.emplace_back(newItem);

			if (newItem->mOwner)
				newItem->mOwner->RemoveItem(newItem);
			newItem->mOwner = static_cast<T_Vec*>(this);
			newItem->mChangeConnection
				= newItem->ConnectChangeSlot(mOnItemChangeSlot, false);

		}
		DoSigAppend(tmpVec);
	}

	std::shared_ptr<T_Item >	AppendItem()
	{
		std::shared_ptr<T_Item > newItem = T_Item::Create();
		AppendItem(newItem);
		return newItem;
	}
	int				AppendItem(std::shared_ptr<T_Item >& newItem)
	{
		if (newItem->mOwner)
			newItem->mOwner->RemoveItem(newItem);
		newItem->mOwner = static_cast<T_Vec*>(this);
		newItem->mChangeConnection 
			= newItem->ConnectChangeSlot(mOnItemChangeSlot,false);
		//newItem->mChangeConnection = newItem->ConnectChangeSlot
		//	(std::bind(&TVecDB::OnItemChange, this, std::placeholders::_1));
		std::pair<RndIterator, bool> pairIterBool = mVecData.emplace_back(newItem);
		if (pairIterBool.second)	// if inserts sucess
			DoSigAppend(newItem);	// then signal about insert

		return 0;
	}//AppendItem
	int				RemoveItem(std::shared_ptr<T_Item> item)
	{
		std::shared_ptr<T_Item > removedItem;
		PtrIdx& ptrIdx = mVecData.get<1>();
		CPtrIterator ptrIt = ptrIdx.find(item.get());
		if (ptrIdx.end() != ptrIt) // this own item ?
		{
			unsigned int row;
			bool hasPos = GetItemPosition(item, row);
						
			removedItem = *ptrIt;
			ptrIdx.erase(ptrIt);
			item->mOwner = nullptr;
			item->mChangeConnection.disconnect();
			//item->DisconnectChangeSlot(mOnItemChangeSlot);
			if (hasPos)
			{
				VecChange removedVec;
				removedVec.emplace_back(row);
				DoSigRemove(removedVec);
			}

		}
		return 0;
	}
	std::shared_ptr<T_Item >	RemoveItem(const unsigned int row)
	{
		std::shared_ptr<T_Item > removedItem;
		if (mVecData.size() > row) // this own item ?
		{
			removedItem = mVecData.at(row);
			
			mVecData.erase(mVecData.begin() + row);
			removedItem->mOwner = nullptr;
			removedItem->mChangeConnection.disconnect();
			//removedItem->DisconnectChangeSlot(mOnItemChangeSlot);

			VecChange removedVec;
			removedVec.emplace_back(row);
			DoSigRemove(removedVec);

			
		}
		return removedItem;
	}
	void Clear()
	{
		//DWORD pos0(0), pos1(0), pos2(0), pos3(0);
		//pos0 = GetTickCount();

		unsigned int pos=0;
		std::vector<unsigned int> tmpVec(mVecData.size());
		auto it = tmpVec.begin();
		
		for (const std::shared_ptr<T_Item>& item : mVecData)
		{
			item->mOwner = nullptr;
			item->mChangeConnection.disconnect();
			*it=pos++;
			++it;
		}
		//pos1 = GetTickCount();
		DoSigRemove(tmpVec);
		//pos2 = GetTickCount();
		mVecData.clear();
		//pos3 = GetTickCount();
		//wxMessageBox(wxString::Format("Clear %d \t Load %d", pos1 - pos0, pos3 - pos2));
	}
	// access:
	inline const std::shared_ptr<T_Item>&	GetItem(unsigned int position)const
	{
		return mVecData[position];
	}
	ModelState				GetState()const
	{
		//const typename StateIdx& stateIdx = mVecData.get<2>(); 
		const auto& stateIdx = mVecData.get<2>();
		return (stateIdx.end() != stateIdx.find(msCreated)
			|| stateIdx.end() != stateIdx.find(msUpdated)
			|| stateIdx.end() != stateIdx.find(msDeleted)
			) ? msUpdated : msExist;
	}
	inline unsigned int		GetSize()const
	{
		return mVecData.size();
	}
	bool GetItemPosition(const std::shared_ptr<const T_Item>& item, unsigned int& pos)const
	{
		const PtrIdx& ptrIdx = mVecData.get<1>();
		CPtrIterator ptrIt = ptrIdx.find(item.get());
		if (ptrIdx.end() != ptrIt)
		{
			CRndIterator rndIt = mVecData.project<0>(ptrIt);
			pos = std::distance(mVecData.cbegin(), rndIt);
			return true;
		}
		return false;
	}


	// misc:
	void Load()
	{
		ExecSelect();
	}

	bool GetCanCommitSave()const 
	{
		return commitSave;
	}
	bool ItemWithId()const
	{
		return itemWithId;
	}

	virtual void Save()
	{
		if(commitSave)
			whDataMgr::GetDB().BeginTransaction();

		SaveRange(msCreated, [this](const std::shared_ptr<T_Item>& item)
			{ 
				ExecInsert(item);
				item->MarkSaved(); 
				item->SaveNested();
			}, &TVecDB::DoSigChange);
		SaveRange(msUpdated, [this](const std::shared_ptr<T_Item>& item)
			{ 
				ExecUpdate(item);
				item->MarkSaved(); 
				item->SaveNested();
			}, &TVecDB::DoSigChange);
		SaveRange(msDeleted, [this](const std::shared_ptr<T_Item>& item)
			{ 
				ExecDelete(item);// каскадно удаляется в БД
				item->MarkSaved();
				RemoveItem(item);
			}, nullptr );
		SaveRange(msNull,    [this](const std::shared_ptr<T_Item>& item)
			{
				RemoveItem(item);  
			}, &TVecDB::DoSigRemove );

		for (const std::shared_ptr<T_Item>& item : mVecData)
			item->SaveNested();


		if (commitSave)
			whDataMgr::GetDB().Commit();
	}
	virtual void SaveItem(const std::shared_ptr<T_Item>& item)
	{
		if (commitSave)
			whDataMgr::GetDB().BeginTransaction();
		
		switch (item->GetState())
		{
		default:	BOOST_THROW_EXCEPTION(error() << wxstr("unknown model state"));//break;
		case  msExist:							break;
		case  msCreated:	ExecInsert(item);	break;
		case  msUpdated:	ExecUpdate(item);	break;
		case  msDeleted:	ExecDelete(item);	break;
		}//switch	
		item->SaveNested();

		if (commitSave)
			whDataMgr::GetDB().Commit();

	}

	void SaveItemByPtr(const T_Item* item)
	{
		const PtrIdx& ptrIdx = mVecData.get<1>();
		CPtrIterator ptrIt = ptrIdx.find(item);
		if (ptrIdx.end() != ptrIt)
			SaveItem(*ptrIt);
	}
	void LoadItemByPtr(const T_Item* item)
	{
		const PtrIdx& ptrIdx = mVecData.get<1>();
		CPtrIterator ptrIt = ptrIdx.find(item);
		if (ptrIdx.end() != ptrIt)
			LoadItem(*ptrIt);
	}


	void LoadItem(const std::shared_ptr<T_Item>& item)
	{
		ExecSelectItem(item);
	}
protected:
	void SaveRange(ModelState state,
		const std::function<void(const std::shared_ptr<T_Item>& item)>& saveOperation,
		void(TVecDB::*signalFunc)(const std::vector<unsigned int>&)const)
	{
		StateIdx& stateIdx = mVecData.get<2>();
		auto range = stateIdx.equal_range(state);
		auto qty = std::distance(range.first, range.second);
		if (!qty)
			return;

		std::vector<const std::shared_ptr<T_Item> >     tmpVec(qty);	// времменное хранилище измененных элементов
		std::vector<unsigned int>			 sigVec(qty);	// массив элеметов для оповещения
		std::vector<shared_connection_block> blockVec(qty);	// блокировка сигналов сохранения от элементов
		
		size_t i=0;
		while (range.first != range.second)
		{
			const std::shared_ptr<T_Item>& item = *range.first;
			
			unsigned int		pos;
			GetItemPosition(item, pos);

			tmpVec[i]   = item;
			sigVec[i]	= pos;
			blockVec[i++] = shared_connection_block(item->mChangeConnection);
			
			++range.first;
		}

		const PtrIdx& ptrIdx = mVecData.get<1>();
		std::function<void(ModelState state)> stateChanger = [](ModelState state){};

		for (const std::shared_ptr<T_Item>& item : tmpVec)
		{
			saveOperation(item);
			CPtrIterator ptrIt = ptrIdx.find(item.get());
			if (ptrIdx.end() != ptrIt)
			{
				StateIterator stateIt = mVecData.project<2>(ptrIt);
				bool isStateModified = 
					mVecData.get<2>().modify_key(stateIt, stateChanger);
			}
		}

		if (signalFunc && !sigVec.empty())
			(this->*signalFunc)(sigVec);

	}
	
	
	virtual bool GetSelectQuery(wxString& query)const		
	{ 
		return false; 
	}
	virtual bool GetSelectItemQuery(const std::shared_ptr<T_Item>& , wxString& )const
	{ 
		return false; 
	}
	virtual bool GetInsertItemQuery(const std::shared_ptr<T_Item>& , wxString& )const	
	{ 
		return false; 
	}
	virtual bool GetUpdateItemQuery(const std::shared_ptr<T_Item>& , wxString& )const
	{ 
		return false; 
	}
	virtual bool GetDeleteItemQuery(const std::shared_ptr<T_Item>& , wxString& )const
	{ 
		return false; 
	}
	virtual bool LoadItemFromDb(const std::shared_ptr<T_Item>& item,
		std::shared_ptr<whTable>& table, const unsigned int row)
	{
		return false;
	}

private:
	void ExecSaveWithoutResult(const std::shared_ptr<T_Item>& item, const wxString& query)
	{
		int result = whDataMgr::GetDB().Exec(query);
		if (!result)
			item->MarkSaved();
		else
			BOOST_THROW_EXCEPTION(error() << wxstr("\ncan`t execute:\n" + query));
	}


	void ExecSelect()
	{
		wxString query;
		const bool queryExist = GetSelectQuery(query);
		if (queryExist)
		{
			Clear();
			auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
			if (table)
			{
				size_t			rowQty = table->GetRowCount();
				std::vector< unsigned int> sigVec;
				std::shared_ptr<T_Data> nullData;

				for (unsigned int i = 0; i < rowQty; ++i)
				{
					std::shared_ptr<T_Item> newItem = T_Item::Create(nullData, nullData);
					LoadItemFromDb(newItem, table, i);

					newItem->mOwner = static_cast<T_Vec*>(this);
					newItem->mChangeConnection
						= newItem->ConnectChangeSlot(mOnItemChangeSlot, false);
					std::pair<RndIterator, bool> pairIterBool = mVecData.emplace_back(newItem);
					if (pairIterBool.second)	// if inserts sucess
						sigVec.emplace_back(i);	// then signal about insert

				}

				DoSigAppend(sigVec);

			}//if(table)

		}

	}
	void ExecSelectItem(const std::shared_ptr<T_Item>& item)
	{
		wxString query;
		const bool queryExist = GetSelectItemQuery(item, query);
		if (queryExist)
		{
			auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
			if (table)
			{
				if (LoadItemFromDb(item, table, 0))
					item->MarkSaved();
			}
			else
				BOOST_THROW_EXCEPTION(error() << wxstr("\ncan`t execute:\n" + query));
		}
	}
	void ExecInsert(const std::shared_ptr<T_Item>& item)
	{
		wxString query;
		const bool queryExist = GetInsertItemQuery(item,query);
		if (queryExist)
		{
			if (itemWithId)
			{
				auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
				if (table)
				{
					if (LoadItemFromDb(item, table, 0))
						item->MarkSaved();
				}
				else
					BOOST_THROW_EXCEPTION(error() << wxstr("\ncan`t execute:\n" + query));
			}
			else
				ExecSaveWithoutResult(item, query);

		}// if (queryExist)
	}
	void ExecUpdate(const std::shared_ptr<T_Item>& item)
	{
		wxString query;
		const bool queryExist = GetUpdateItemQuery(item,query);
		if (queryExist)
			ExecSaveWithoutResult(item, query);

	}
	void ExecDelete(const std::shared_ptr<T_Item>& item)
	{
		wxString query;
		const bool queryExist = GetDeleteItemQuery(item, query);
		if (queryExist)
			ExecSaveWithoutResult(item, query);
	}


	using VecChange = std::vector<unsigned int>;
	using SigVecChange = signal<void(const T_Vec&, const VecChange&) >;
	using SlotVecChange = std::function<void(const T_Vec&, const VecChange&)>;

	TBaseTable<T_Item>		mVecData;
	mutable SigVecChange	sigAppend;
	mutable SigVecChange	sigRemove;
	mutable SigVecChange	sigChange;

	void OnItemChange(const T_Item& item)
	{
		PtrIdx& ptrIdx = mVecData.get<1>();
		CPtrIterator ptrIt = ptrIdx.find(&item );
		if (ptrIdx.end() != ptrIt)
		{
			StateIterator stateIt = mVecData.project<2>(ptrIt);
			std::function<void(ModelState state)> stateChanger = [](ModelState state){};
			bool isStateModified = mVecData.get<2>().modify_key(stateIt, stateChanger);
			if (isStateModified)
				DoSigChange(*ptrIt);

		}//if (ptrIdx.end() != ptrIt) 
	}//void OnItemChange(const T_Item& item)

	inline void DoSigAppend(const VecChange& itemVec)const
	{
		sigAppend(*static_cast<const T_Vec*>(this), itemVec);
	}
	inline void DoSigRemove(const VecChange& itemVec)const
	{
		sigRemove(*static_cast<const T_Vec*>(this), itemVec);
	}
	inline void DoSigChange(const VecChange& itemVec)const
	{
		sigChange(*static_cast<const T_Vec*>(this), itemVec);
	}

	void DoSigAppend(std::shared_ptr<const T_Item > item)const
	{
		VecChange vec;
		unsigned int pos;
		if (GetItemPosition(item, pos))
			vec.emplace_back(pos);
		if (!vec.empty() )
			DoSigAppend(vec);
	}
	void DoSigRemove(std::shared_ptr<const T_Item > item)const
	{
		VecChange vec;
		unsigned int pos;
		if (GetItemPosition(item, pos))
			vec.emplace_back(pos);
		if (!vec.empty())
			DoSigRemove(vec);
	}
	void DoSigChange(std::shared_ptr<const T_Item > item)const
	{
		VecChange vec;
		unsigned int pos;
		if (GetItemPosition(item, pos))
			vec.emplace_back(pos);
		if (!vec.empty())
			DoSigChange(vec);
	}
//data

public:
	connection ConnectAppendSlot(const SlotVecChange &subscriber)const
	{
		return sigAppend.connect(subscriber);
	}
	connection ConnectRemoveSlot(const SlotVecChange &subscriber)const
	{
		return sigRemove.connect(subscriber);
	}
	connection ConnectChangeSlot(const SlotVecChange &subscriber)const
	{
		return sigChange.connect(subscriber);
	}

	inline void DisconnectAppendSlot(const SlotVecChange &subscriber)const
	{
		sigAppend.disconnect(&subscriber);
	}
	inline void DisconnectRemoveSlot(const SlotVecChange &subscriber)const
	{
		sigRemove.disconnect(&subscriber);
	}
	inline void DisconnectChangeSlot(const SlotVecChange &subscriber)const
	{
		sigChange.disconnect(&subscriber);
	}

};//class TVecDB








}//namespace wh{
#endif // __****_H
