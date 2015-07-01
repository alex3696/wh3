#include "_pch.h"
#include "rdbs.h"

using namespace wh;
using namespace wh::rdbs;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Index
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
Index::Index(Table* table, const boost::property_tree::ptree& pt)
	:mTable(table)
{
	mType = pt.get<std::string>("type");

	for (const auto& index_field : pt.get_child("fields"))
	{
		//auto label = index_field.first;
		auto field_no = index_field.second.get_value<unsigned int>();
				
		mFields.emplace_back(field_no);
	}
		
}
//-------------------------------------------------------------------------
void Index::MakeKey(IndexKey& key, RecordSp& rec)const
{
	key.clear();
	for (const auto& i : mFields)
		key.emplace_back(rec->GetData(i));

}
//-------------------------------------------------------------------------
void Index::Clear()
{
	mIdx.clear();
}
//-------------------------------------------------------------------------
RecordSp Index::Find(const IndexKey& key)
{
	auto it = mIdx.find(key);
	if (mIdx.end() == it)
		return RecordSp();
	return it->second;
}
//-------------------------------------------------------------------------
void Index::Insert(std::shared_ptr<Record>& newRecord)
{
	IndexKey newKey;
	MakeKey(newKey, newRecord);
	mIdx.emplace(std::make_pair(newKey, newRecord));
}
//-------------------------------------------------------------------------
void Index::Update(RecordSp& newRecord, RecordSp& oldRecord)
{
	Delete(oldRecord);
	Insert(newRecord);
}
//-------------------------------------------------------------------------
void Index::Delete(RecordSp& oldRecord)
{
	IndexKey oldKey;
	MakeKey(oldKey, oldRecord);
	mIdx.erase(oldKey);
}
//-------------------------------------------------------------------------
const std::vector<unsigned int>& Index::GetFieldList()const
{
	return mFields;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Array
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
using namespace wh::rdbs::index;

//-------------------------------------------------------------------------
void Array::Clear()const
{
	for (auto idx : mIndexStore)
		idx.Clear();
		
}
//-------------------------------------------------------------------------
const Index& Array::Get(unsigned int pos)const
{
	if (mIndexStore.size() > pos)
		BOOST_THROW_EXCEPTION(error() << wxstr("there are no index"));
	return mIndexStore[pos];
}

//-------------------------------------------------------------------------
void Array::Insert(std::shared_ptr<Record>& record)const
{
	for (auto idx : mIndexStore)
		idx.Insert(record);
}

//-------------------------------------------------------------------------
void Array::Update(RecordSp& newRecord, RecordSp& oldRecord)const
{
	for (auto idx : mIndexStore)
		idx.Update(newRecord, oldRecord);
}
//-------------------------------------------------------------------------
void Array::Delete(RecordSp& oldRecord)const
{
	for (auto idx : mIndexStore)
		idx.Delete(oldRecord);
}

//-------------------------------------------------------------------------
const Index& Array::Create(const std::vector<unsigned int>& idxFields, const wxString& idxType)
{
	if (!mTable)
		BOOST_THROW_EXCEPTION(error() << wxstr("there are no table ptr"));

	mIndexStore.emplace_back(mTable, idxFields, idxType);
	return mIndexStore.back();
}
//-------------------------------------------------------------------------
void Array::Drop(std::vector<unsigned int>&	idxFields)
{
	mIndexStore.get<1>().erase(idxFields);
}
//-------------------------------------------------------------------------
void Array::Drop(unsigned int pos)
{
	mIndexStore.erase(mIndexStore.begin() + pos);
}

//-------------------------------------------------------------------------
void Array::StructLoad(const boost::property_tree::ptree& pt)
{
	for (const auto& index_pair : pt.get_child("indexes"))
		mIndexStore.emplace_back(mTable, index_pair.second);////Field field(pt_field.second);

};