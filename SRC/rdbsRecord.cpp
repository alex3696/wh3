#include "_pch.h"
#include "rdbs.h"

using namespace wh;
using namespace wh::rdbs;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Record
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
Record::Record(Table* table, const RecordData* current, const RecordData* stored )
	: mTable(table)
	, mCurrent(current ? new RecordData(*current) : nullptr)
	, mStored(stored ? new RecordData(*stored) : nullptr)
{
}
//-------------------------------------------------------------------------
std::shared_ptr<Record> Record::Create(Table* table, const RecordData* current 
													, const RecordData* stored )
{
	return std::make_shared<Record>(table, current, stored);
}
//-------------------------------------------------------------------------

std::shared_ptr<Record> Record::Create(Table* table
						, std::shared_ptr<whTable>& dbtable, unsigned int row)
{
	auto rec = Record::Create(table, nullptr, nullptr);
	rec->Load(dbtable, row);
	return rec;
}

//-------------------------------------------------------------------------

void Record::SetData(const RecordData* current, const RecordData* stored )
{
	const auto& recArray = mTable->GetRecordArray();
	auto& recIdx = recArray.get<1>();

	auto oldIt = recIdx.find(this);
	if (recIdx.end() != oldIt)
	{
		auto newRec = mTable->CreateRecord(current, stored);
		mTable->Update(newRec, oldIt);
	}
}
//-------------------------------------------------------------------------
wxString Record::GetData(unsigned int col, bool current )
{
	return wxEmptyString;
}
//-------------------------------------------------------------------------
wxString Record::GetData(const wxString& col, bool current )
{
	return wxEmptyString;
}
//-------------------------------------------------------------------------
const RecordData& Record::GetData()const
{
	if (mCurrent) // msCreated msUpdated msExist
		return *mCurrent.get();
	if (!mStored)
		BOOST_THROW_EXCEPTION(error() << wxstr("No model data, msNull"));
	return *mStored.get();
}
//-------------------------------------------------------------------------
ModelState Record::GetState()const
{
	return wh::GetState(mStored, mCurrent);
}
//-------------------------------------------------------------------------
unsigned int Record::ColumnCount()const
{
	if (mCurrent)
		return mCurrent->size();
	if (mStored)
		return mStored->size();
	return 0;
}
//-------------------------------------------------------------------------
bool Record::Load(std::shared_ptr<whTable>& table, unsigned int row)
{
	
	auto colQty = mTable->GetFieldArray().size();
	
	mCurrent.reset(new RecordData(colQty));
	
	for (unsigned int col = 0; col < colQty; ++col)
		table->GetAsString(col, row, mCurrent->at(col) );
	
	mStored = mCurrent ;
	return true;
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// RecordArray
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
using namespace wh::rdbs::record;



Array::Array(Table* table )
	:mTable(table)
{}
//-------------------------------------------------------------------------
RecordSp Array::Create()const
{
	const auto& fields = mTable->GetFieldArray() ;

	RecordData recData(fields.size());
	for (size_t i = 0; i < fields.size(); i++)
	{
		const auto& field = fields[i];
		recData[i] = field.mDefault;
	}
	return Record::Create(mTable, &recData);
}
//-------------------------------------------------------------------------
RecordSp Array::Create(const RecordData* current, const RecordData* stored)const
{
	return Record::Create(mTable, current, stored);
}
//-------------------------------------------------------------------------
void Array::Load(std::shared_ptr<whTable>& table)
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
//-------------------------------------------------------------------------
void Array::Save()
{
	DoInsert();
	DoUpdate();
	DoDelete();
}
//-------------------------------------------------------------------------
void  Array::DoInsert()
{
	std::vector < IdxPtr::iterator > oldIters;

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
//-------------------------------------------------------------------------
void  Array::DoUpdate()
{
	std::vector < IdxPtr::iterator > oldIters;

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
//-------------------------------------------------------------------------
void  Array::DoDelete()
{
	std::vector < IdxPtr::iterator > oldIters;

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

//-------------------------------------------------------------------------
void  Array::GetRange(std::vector< IdxPtr::iterator> iters, ModelState state )
{
	record::IdxState& stateIdx = this->get<2>();
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