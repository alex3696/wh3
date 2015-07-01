#ifndef __RDBSINDEX_H
#define __RDBSINDEX_H

#include "rdbsRecord.h"

namespace wh{
namespace rdbs{

using IndexFields = std::vector < unsigned int >;

using IndexKey = std::vector < wxString > ;

//-------------------------------------------------------------------------
class Index
{
private:
	Table*			mTable;
	IndexFields		mFields;
	wxString		mType;
	
	std::map<IndexKey, RecordSp>	mIdx;

	void MakeKey(IndexKey& key, RecordSp& rec)const;
public:
	Index(Table* table,const IndexFields& idxFields, const wxString& idxType)
		:mTable(table), mFields(idxFields), mType(idxType)
	{}

	Index(Table* table, const boost::property_tree::ptree& pt);

	void Clear();

	RecordSp Find(const IndexKey& key);
	void Insert(RecordSp& newRecord);
	void Update(RecordSp& newRecord, RecordSp& oldRecord);
	void Delete(RecordSp& oldRecord);

	const std::vector<unsigned int>& GetFieldList()const;
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
namespace index
{
struct extr_filedlist
{
	typedef const std::vector<unsigned int>& result_type;
	inline result_type operator()(const Index& r)const
	{
		return r.GetFieldList();
	}
};

using Store =
	boost::multi_index_container
	<
	Index,
		indexed_by
		<
			random_access<>
			, hashed_unique< extr_filedlist >
		>
	>;




//-------------------------------------------------------------------------
class Array
{
	struct error : virtual exception_base {};
	
	Store				mIndexStore;

	Table*				mTable;
public:
	Array(Table* table = nullptr)
		:mTable(table)
	{}

	void Clear()const;

	const Index& Get(unsigned int pos)const;

	void Insert(RecordSp& record)const;
	void Update(RecordSp& newRecord, RecordSp& oldRecord)const;
	void Delete(RecordSp& oldRecord)const;

	const Index& Create(const std::vector<unsigned int>& idxFields, const wxString& idxType);
	void Drop(std::vector<unsigned int>&	idxFields);
	void Drop(unsigned int pos);

	void StructLoad(const boost::property_tree::ptree& pt);

};

}//namespace index{
}//namespace rdbs{
}//namespace wh{
#endif // __****_H