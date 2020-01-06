#ifndef __MODEL_PROPABLE_H
#define __MODEL_PROPABLE_H

#include "ModelBrowserData.h"

namespace wh {
//-----------------------------------------------------------------------------
// PROP TABLE
//-----------------------------------------------------------------------------
class ModelPropTable;
//-----------------------------------------------------------------------------
class PropRec64 : public IProp64
{
private:
	int64_t		mId;
	wxString	mTitle;
	FieldType	mFieldType;
	wxArrayString mVar;
	bool mVarStrict;
	
	ModelPropTable*	mTable = nullptr;
public:
	//ObjRec64() {}
	PropRec64(const int64_t id
		, ModelPropTable* table)
		:mId(id), mTable(table)
	{
	}

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };
	virtual FieldType GetKind()const override { return mFieldType; };
	virtual const wxArrayString& GetVar()const override { return mVar; };
	virtual bool GetVarStrict()const override { return mVarStrict; };

	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }
	void SetTitle(const wxString& str) { mTitle = str; };
	void SetKind(const FieldType ft) { mFieldType = ft; };
	void SetKind(const wxString& str) { mFieldType = ToFieldType(str); };

	void SetVar(const wxArrayString& vec)	{ mVar = vec; };
	void SetVar(const wxString& str)  		{ mVar = Sql2ArrayString(str);	};
	void SetVarStrict(bool strict) { mVarStrict = strict; };
	void SetVarStrict(const wxString& str) { mVarStrict = Sql2Bool(str); };

};
//-----------------------------------------------------------------------------
// PROP TABLE
//-----------------------------------------------------------------------------
class ModelPropTable
{
public:
	typedef IProp64 RowType;

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
	ModelPropTable()
	{}

	inline const Storage& GetStorage()const { return mData; }
	inline bool empty()const { mData.empty(); }
	inline void Clear()	{ mData.clear(); }

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
			auto prop = std::make_shared<PropRec64>(id, this);
			fn(prop);
			auto ins_it = idxId.emplace(prop);
			return *ins_it.first;
		}

		idxId.modify(it, fn);
		return *it;
	}

	void UpdateExist()
	{
		if (this->mData.empty())
			return;
		//Clear();

		wxString str_id;
		for (const auto& it : mData)
		{
			if (it->GetTitle().empty())// do not load if title already exists
				str_id += wxString::Format(" OR id=%s", it->GetIdAsString());
		}
		if (str_id.empty())
			return;

		str_id.Remove(0, 3);

		wxString query = wxString::Format(
			"SELECT id, title, kind, var, var_strict "
			" FROM prop WHERE %s"
			, str_id);

		auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
		if (table)
		{
			unsigned int rowQty = table->GetRowCount();
			size_t row = 0;
			const fnModify fn = [this, &table, &row](const std::shared_ptr<RowType>& irec)
			{
				auto record = std::dynamic_pointer_cast<PropRec64>(irec);
				//record->SetId(table->GetAsString(0, row));
				record->SetTitle(table->GetAsString(1, row));
				record->SetKind(table->GetAsString(2, row));
				record->SetVar(table->GetAsString(3, row));
				record->SetVarStrict(table->GetAsString(4, row));
			};


			for (; row < rowQty; row++)
			{
				int64_t id;
				if (!table->GetAsString(0, row).ToLongLong(&id))
					throw;

				InsertOrUpdate(id, fn);
			}//for
		}//if (table)
	}
};
//---------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H