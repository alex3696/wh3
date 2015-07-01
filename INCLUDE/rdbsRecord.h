#ifndef __RDBSRECORD_H
#define __RDBSRECORD_H

#include "rdbsTrigger.h"

namespace wh{
namespace rdbs{



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
using RecordData = std::vector < wxString > ;
//-------------------------------------------------------------------------
class Record
{
protected:
	std::shared_ptr<RecordData>		mStored;
	std::shared_ptr<RecordData>		mCurrent;

	//TriggerArray					mTriggers;
	Table*							mTable;

	struct error : virtual exception_base {};
public:
	Record(Table* table = nullptr
			, const RecordData* current = nullptr
			, const RecordData* stored = nullptr);
	static std::shared_ptr<Record> Create(Table* table = nullptr
										, const RecordData* current = nullptr
										, const RecordData* stored = nullptr);

	static std::shared_ptr<Record> Create(Table* table 
		, std::shared_ptr<whTable>& dbtable, unsigned int row);

	void SetData(const RecordData* current, const RecordData* stored = nullptr);

	wxString GetData(unsigned int col, bool current = true);
	wxString GetData(const wxString& col, bool current = true);
	const RecordData& GetData()const;

	ModelState GetState()const;

	unsigned int ColumnCount()const;

	bool Load(std::shared_ptr<whTable>& table, unsigned int row);
	//void Save();

	
protected:

};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
namespace record
{
struct extr_state
{
	typedef ModelState result_type;
	inline result_type operator()(const RecordSp& r)const
	{
		return r->GetState();
	}
};
struct extr_voidptr
{
	typedef const void* result_type;
	inline result_type operator()(const RecordSp& r)const { return r.get(); }
};

using Store =
	boost::multi_index_container
	<
		std::shared_ptr<Record>,
		indexed_by
		<
			random_access<>
			, hashed_unique< extr_voidptr >
			, ordered_non_unique< extr_state >
		>
	>;

using IdxRnd = nth_index< Store, 0>::type;
using IdxPtr = nth_index< Store, 1>::type;
using IdxState = nth_index< Store, 2>::type;

class Array
	: public record::Store
{
	Table*				mTable;

	void  DoInsert();
	void  DoUpdate();
	void  DoDelete();
	void  GetRange(std::vector< IdxPtr::iterator> iters, ModelState state);
public:
	Array(Table* table = nullptr);

	RecordSp Create()const;
	RecordSp Create(const RecordData* current, const RecordData* stored)const;

	void Load(std::shared_ptr<whTable>& table);
	void Save();
};
//-------------------------------------------------------------------------
template <class RECORD>
struct Textr_state
{
	typedef ModelState result_type;
	inline result_type operator()(const std::shared_ptr<RECORD>& r)const
	{
		return r->GetState();
	}
};
template <class RECORD>
struct Textr_voidptr
{
	typedef const void* result_type;
	inline result_type operator()(const std::shared_ptr<RECORD>& r)const 
	{ 
		return r.get(); 
	}
};

template <class RECORD>
using TStore =
	boost::multi_index_container
	<
		std::shared_ptr<RECORD>,
		indexed_by
		<
			random_access<>
			, hashed_unique< Textr_voidptr<RECORD> >
			, ordered_non_unique< Textr_state<RECORD> >
		>
	>;

template <typename RECORD> using TIdxRnd = typename nth_index<  TStore<RECORD>, 0>::type;
template <typename RECORD> using TIdxPtr = typename nth_index< TStore<RECORD>, 1>::type;
template <typename RECORD>	using TIdxState = typename nth_index< TStore<RECORD>, 2>::type;
//-------------------------------------------------------------------------
template <class RECORD>
class TArray
	: public record::TStore<RECORD>
{
	Table*				mTable;
	
	void  DoInsert()
	{
		std::vector < TIdxPtr<RECORD>::iterator > oldIters;

		GetRange(oldIters, msCreated);

		for (auto& it : oldIters)
		{
			RecordSp oldRec = *it;
			wxString query;
			if (!mTable->GetInsertQuery(query, oldRec))
				return;

			RecordSp newRec;
			if (mTable->GetUpdateOnInsert())
			{
				auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
				newRec = (table) ? Record::Create(mTable, table, 0) : oldRec;
			}
			else
			{
				if (whDataMgr::GetDB().Exec(query))
					newRec = oldRec;
			}

			mTable->Update(newRec, oldRec);
		}
	}
	void  DoUpdate()
	{
		std::vector < TIdxPtr<RECORD>::iterator > oldIters;

		GetRange(oldIters, msUpdated);

		for (auto& it : oldIters)
		{
			RecordSp oldRec = *it;
			wxString query;
			if (!mTable->GetUpdateQuery(query, oldRec))
				return;

			RecordSp newRec;
			if (whDataMgr::GetDB().Exec(query))
				newRec = oldRec;

			mTable->Update(newRec, oldRec);
		}
	}
	void  DoDelete()
	{
		std::vector < TIdxPtr<RECORD>::iterator > oldIters;

		GetRange(oldIters, msDeleted);

		for (auto& it : oldIters)
		{
			RecordSp oldRec = *it;
			wxString query;
			if (mTable->GetDeleteQuery(query, oldRec))
				if (whDataMgr::GetDB().Exec(query))
					mTable->Delete(it);
		}
	}
	void  GetRange(std::vector< IdxPtr::iterator> iters, ModelState state)
	{
		TIdxState<RECORD>& stateIdx = this->get<2>();
		auto range = stateIdx.equal_range(ModelState::msCreated);
		auto qty = std::distance(range.first, range.second);
		if (!qty)
			return;

		size_t i = 0;
		while (range.first != range.second)
		{
			iters[i] = this->project<1>(range.first);
			++i;
			++range.first;
		}

	}
public:
	using RecordSp = std::shared_ptr < RECORD >;

	TArray(Table* table = nullptr)
		:mTable(table)
	{}

	RecordSp Create()const
	{
		const auto& fields = mTable->GetFieldArray();

		RecordData recData(fields.size());
		for (size_t i = 0; i < fields.size(); i++)
		{
			const auto& field = fields[i];
			recData[i] = field.mDefault;
		}
		return RECORD::Create(mTable, &recData);
	}
	RecordSp Create(const RecordData* current, const RecordData* stored)const
	{
		RECORD::Create(mTable, current, stored);
	}

	void Load(std::shared_ptr<whTable>& table)
	{
		if (!table)
			return;

		unsigned int rowQty = table->GetRowCount();
		for (unsigned int i = 0; i < rowQty; ++i)
		{
			auto child = Record::Create(mTable, table, i);
			mTable->Insert(child);
		}//if(table)

	}
	void Save()
	{
		DoInsert();
		DoUpdate();
		DoDelete();
	}
};





}//namespace record{
}//namespace rdbs{
}//namespace wh{
#endif // __****_H