#ifndef __MODEL_ACTTABLE_H
#define __MODEL_ACTTABLE_H

#include "ModelBrowserData.h"

namespace wh{
//-----------------------------------------------------------------------------
// ACT TABLE
//-----------------------------------------------------------------------------
class ModelActTable;
//-----------------------------------------------------------------------------
class ActRec64 : public IAct64
{
private:
	int64_t		mId;
	wxString	mTitle;
	wxString	mColour;
	wxString	mNote;
	
	ModelActTable*	mTable = nullptr;
public:
	ActRec64(const int64_t id, ModelActTable* table)
		:mId(id), mTable(table)
	{
	}

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };
	virtual const wxString& GetColour()const override { return mColour; };
	virtual const wxString& GetNote()const override { return mNote; };

	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }
	void SetTitle(const wxString& str) { mTitle = str; };
	void SetColour(const wxString& str) { mColour = str; };
	void SetNote(const wxString& str) { mNote = str; };
};
//-----------------------------------------------------------------------------
class ModelActTable
{
public:
	typedef IAct64 RowType;

	using Storage = boost::multi_index_container
	<
		std::shared_ptr<RowType>,
		indexed_by
		<
			 ordered_unique <	extr_id_IIdent64 >
			//, random_access<> //SQL order	
			//, ordered_unique< extr_void_ptr_IIdent64 >
		>
	>;
private:
	Storage		mData;
public:
	ModelActTable()
	{}

	inline const Storage& GetStorage()const { return mData; }
	inline bool empty()const { return mData.empty(); }
	inline void Clear() { mData.clear(); }

	size_t size()const
	{
		return mData.size();
	}

	using fnModify = std::function<void(const std::shared_ptr<RowType>& obj)>;

	std::shared_ptr<const RowType> GetById(const int64_t& id)
	{
		auto& idxId = mData.get<0>();
		auto it = idxId.find(id);
		if (idxId.end() == it)
			throw;
		return *it;
	}

	const std::shared_ptr<RowType>& InsertOrUpdate(const int64_t& id, const fnModify& fn)
	{
		auto& idxId = mData.get<0>();
		auto it = idxId.find(id);
		if (idxId.end() == it)
		{
			auto act = std::make_shared<ActRec64>(id, this);
			fn(act);
			auto ins_it = idxId.emplace(act);
			return *ins_it.first;
		}
		
		idxId.modify(it, fn);
		return *it;
	}

};

	
//---------------------------------------------------------------------------





} //namespace wh{
#endif // __*_H