#include "_pch.h"
#include "rdbs.h"

using namespace wh;
using namespace wh::rdbs;

//-------------------------------------------------------------------------
Table::Table()
	: mIndexes(this)
	, mRecords(this)
{
}
//-------------------------------------------------------------------------
void Table::SetUpdateOnInsert(bool updateonInsert)
{
	mUpdateOnInsert = updateonInsert;
}
//-------------------------------------------------------------------------
bool Table::GetUpdateOnInsert()const
{
	return mUpdateOnInsert;
}
//-------------------------------------------------------------------------
void Table::Clear()
{
	ClearRecords();
	mFields.clear();
}
//-------------------------------------------------------------------------





void Table::SetFieldArray(std::initializer_list<rdbs::Field> il)
{
	Clear();
	mFields = field::Array(il);
}
//-------------------------------------------------------------------------
void Table::SetFieldArray(field::Array& fieldArray)
{
	Clear();
	mFields = fieldArray;
}
//-------------------------------------------------------------------------
const rdbs::Field& Table::GetField(unsigned int pos)const
{
	return mFields[pos];
}
//-------------------------------------------------------------------------
const rdbs::Field& Table::GetField(const wxString& name)const
{
	auto it = mFields.get<1>().find(name);
	return *it;
}
//-------------------------------------------------------------------------
int Table::GetFieldIndex(const wxString& name)const
{
	auto it = mFields.get<1>().find(name);
	if (mFields.get<1>().end() != it)
	{
		mFields.project<0>(it);
		return std::distance(mFields.begin(), mFields.end());
	}
	return -1;
}
//-------------------------------------------------------------------------
const field::Array& Table::GetFieldArray()const
{
	return mFields;
}
//-------------------------------------------------------------------------









const Index& Table::CreateIndex(std::initializer_list<unsigned int> il
	, const wxString& type)
{
	std::vector<unsigned int> idxFields(il);
	return mIndexes.Create(idxFields, type);
}
//-------------------------------------------------------------------------
const Index& Table::CreateIndex(const std::vector<unsigned int>& idxFields
	, const wxString& type)
{
	return mIndexes.Create(idxFields, type);
}
//-------------------------------------------------------------------------
const Index& Table::GetIndex(unsigned int pos)const
{
	return mIndexes.Get(pos);
}
//-------------------------------------------------------------------------
void Table::DeleteIndex(unsigned int pos)
{
	mIndexes.Drop(pos);
}
//-------------------------------------------------------------------------
void Table::ClearRecords()
{
	mIndexes.Clear();
	mRecords.clear();
}
//-------------------------------------------------------------------------
RecordSp Table::CreateRecord()const
{
	return mRecords.Create();
}
//-------------------------------------------------------------------------
RecordSp Table::CreateRecord(const RecordData* current, const RecordData* stored)const
{
	return mRecords.Create(current, stored);
}


































bool Table::GetSelectAllQuery(wxString& query)const
{
	query = "SELECT id, label, type FROM t_prop";
	return true;

}
//-------------------------------------------------------------------------
bool Table::GetSelectQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format("SELECT id, label, type FROM t_prop WHERE id=%s"
		, data[0]);
	return true;
}
//-------------------------------------------------------------------------
bool Table::GetInsertQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format
		("INSERT INTO t_prop(label, type)VALUES('%s', %s)RETURNING id, label, type",
		data[1], data[2]);
	return true;
}
//-------------------------------------------------------------------------
bool Table::GetUpdateQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format(
		"UPDATE t_prop SET label='%s' , type=%s WHERE id=%s"
		, data[1], data[2], data[0]);
	return true;
}
//-------------------------------------------------------------------------
bool Table::GetDeleteQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format(
		"DELETE FROM t_act WHERE id = %s "
		, data[0]);
	return true;
}





//-------------------------------------------------------------------------
const record::Array& Table::GetRecordArray()const
{
	return mRecords;
}

//-------------------------------------------------------------------------
void Table::Insert(RecordSp& newRec)
{
	if (!newRec)
		BOOST_THROW_EXCEPTION(error() << wxstr("newRec is null"));
	if (newRec->ColumnCount() != mFields.size())
		BOOST_THROW_EXCEPTION(error() << wxstr("newRec wrong fields size"));

	mTriggers.Do(this, newRec, RecordSp(nullptr), TgOp::toBeforeInsert);
	mIndexes.Insert(newRec);
	mRecords.emplace_back(newRec);
	mTriggers.Do(this, newRec, RecordSp(nullptr), TgOp::toAfterInsert);
}
//-------------------------------------------------------------------------

void Table::Update(std::shared_ptr<Record> newRec, record::IdxPtr::iterator& oldIt)
{
	if (mRecords.get<1>().end() != oldIt)
	{
		RecordSp oldRec = (*oldIt);

		mTriggers.Do(this, newRec, oldRec, TgOp::toBeforeUpdate);
		mRecords.get<1>().replace(oldIt, newRec);
		mIndexes.Update(newRec, oldRec);
		mTriggers.Do(this, newRec, oldRec, TgOp::toAfterUpdate);
	}
}
//-------------------------------------------------------------------------

void Table::Update(std::shared_ptr<Record> newRec, std::shared_ptr<Record> oldRec)
{
	if (!newRec || !oldRec)
		BOOST_THROW_EXCEPTION(error() << wxstr("oldRec or newRec is null"));
	if (newRec->ColumnCount() != mFields.size())
		BOOST_THROW_EXCEPTION(error() << wxstr("newRec wrong fields size"));


	auto oldIt = mRecords.get<1>().find(oldRec.get());

	Update(newRec, oldIt);


}
//-------------------------------------------------------------------------
void Table::Delete(record::IdxPtr::iterator& oldIt)
{
	RecordSp oldRec = *oldIt;
	mTriggers.Do(this, RecordSp(nullptr), oldRec, TgOp::toBeforeDelete);
	mIndexes.Delete(oldRec);
	mRecords.get<1>().erase(oldIt);
	mTriggers.Do(this, RecordSp(nullptr), oldRec, TgOp::toAfterDelete);

}
//-------------------------------------------------------------------------
void Table::Delete(RecordSp& oldRec)
{
	if (!oldRec)
		BOOST_THROW_EXCEPTION(error() << wxstr("oldRec is null"));
	if (oldRec->ColumnCount() != mFields.size())
		BOOST_THROW_EXCEPTION(error() << wxstr("oldRec wrong fields size"));

	mTriggers.Do(this, RecordSp(nullptr), oldRec, TgOp::toBeforeDelete);
	mIndexes.Delete(oldRec);
	mRecords.get<1>().erase(oldRec.get());
	mTriggers.Do(this, RecordSp(nullptr), oldRec, TgOp::toAfterDelete);
}

//-------------------------------------------------------------------------
sig::connection Table::ConnectTrigger(TgOp tg_op, TriggerFunc& tg_func)
{
	return mTriggers.Connect(tg_op, tg_func);
}
//-------------------------------------------------------------------------
void Table::DisconnectTrigger(TgOp tg_op, TriggerFunc& tg_func)
{
	mTriggers.Disconnect(tg_op, tg_func);
}







//-------------------------------------------------------------------------
void Table::SaveRecords()
{
	mRecords.Save();
}


//-------------------------------------------------------------------------
void Table::LoadRecords()
{
	ClearRecords();
	wxString query;
	if (GetSelectAllQuery(query))
	{
		auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
		mRecords.Load(table);
	}
}


//-------------------------------------------------------------------------
void Table::StructLoad(const wxString& path)
{
	using boost::property_tree::ptree;
	ptree pt;
	std::string str = path.c_str();
	read_xml(str, pt);

	for (const auto& pt_table : pt.get_child("db.tables"))
	{
		//auto table_label = pt_table.second.get_child("label").get_value<std::string>();
		auto table_label = pt_table.second.get<std::string>("label");

		mFields.StructLoad(pt_table.second);
		mIndexes.StructLoad(pt_table.second);

	}


};
