#ifndef __TMODELTABLE_H
#define __TMODELTABLE_H

#include "TModelArray.h"
#include "filter_data.h"

namespace wh{


using ITableField = TModelData<Field>;
using IFieldArray = TModelArray<ITableField>;

using TableRowData = std::vector<wxString>;

using UIntData = TModelData<unsigned int>;
//-------------------------------------------------------------------------
class ITableRow
	: public TModelData<TableRowData>
{
public:
	ITableRow(const char option = ModelOption::EnableParentNotify)
		:TModelData<TableRowData>(option)
	{}

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>& db, const size_t pos)override;

protected:
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;


	
};
//-------------------------------------------------------------------------
template <class ITEM_TYPE>
class TTableDataArr
	:public IModel
{
public:
	TTableDataArr(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
		: IModel(option)
	{}

	std::shared_ptr<ITEM_TYPE> at(size_t pos)const 
	{
		return std::dynamic_pointer_cast<ITEM_TYPE>(GetChild(pos));
	}

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		auto child = std::make_shared <typename ITEM_TYPE>();
		return std::dynamic_pointer_cast<IModel>(child);
	};
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t row)override
	{
		auto childModel = std::dynamic_pointer_cast<ITableRow>(child);
		auto mtable = dynamic_cast<ITable*>(GetParent());
		if (!childModel || !mtable)
			return false;

		TableRowData data;

		for (size_t col = 0; col < mtable->mFieldVec->GetChildQty(); col++)
			data.emplace_back(table->GetAsString(col, row));
		childModel->SetData(data);
		return true;
	}


	virtual bool GetSelectChildsQuery(wxString& query)const override
	{
		auto mtable = dynamic_cast<ITable*>(GetParent());
		if (!mtable)
			return false;


		const wxString& table = mtable->mTableName->GetData();
		const auto& field_vec = mtable->mFieldVec;

		wxString fields, filter_sql, limit_page, sort;
		for (unsigned int i = 0; i < field_vec->size(); ++i)
		{
			const auto& field = field_vec->at(i)->GetData();
			fields += field.mDbTitle + ",";

			for (const auto& filter : field.mFilter)
			{
				if (!filter.mEnable)
					continue;

				if (!filter_sql.IsEmpty())
					filter_sql << " " << ToSqlString(filter.mConn);

				filter_sql << field.mDbTitle << ToSqlString(filter.mOp);
				if (foLike == filter.mOp || foNotLike == filter.mOp)
					filter_sql << "'%" << filter.mVal << "%' ";
				else
					filter_sql << "'" << filter.mVal << "' ";
			}

			if (field.mSort > 0)
				sort << field.mDbTitle << " ASC,";
			else if (field.mSort < 0)
				sort << field.mDbTitle << " DESC,";
		}
		fields.RemoveLast();

		if (!sort.IsEmpty())
		{
			sort.RemoveLast();
			sort = " ORDER BY " + sort;
		}

		const unsigned int page_limit = mtable->mPageLimit->GetData();
		const unsigned int page_no = mtable->mPageNo->GetData();
		limit_page << " LIMIT " << page_limit << " OFFSET " << page_limit * page_no;

		query.clear();

		query << "SELECT " << fields << " FROM " << table;
		if (!filter_sql.IsEmpty())
			query << " WHERE " << filter_sql;
		query << sort;
		query << limit_page;

		return !query.empty();
	}

};




//-------------------------------------------------------------------------
class ITable
	: public IModel
{
public: 
	ITable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	std::shared_ptr<IFieldArray>			mFieldVec;
	std::shared_ptr<UIntData>				mPageLimit;
	std::shared_ptr<UIntData>				mPageNo;
	std::shared_ptr<TModelData<wxString>>	mTableName;
	
	
	virtual const std::shared_ptr<IModel> GetDataArr()const = 0;
	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col) {};
	virtual bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const{ return false; };
protected:
	sig::scoped_connection mConnAFI;
	sig::scoped_connection mConnAFR;

	void OnFieldAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newVec
		, const SptrIModel& itemBefore);
	void OnFieldBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);

	virtual void LoadChilds()override
	{
		auto data_arr = GetDataArr();
		if (data_arr)
			data_arr->Load();
	}

};
//-------------------------------------------------------------------------
template <class DATA_ARR>
class TTable
	:public ITable
{
public:
	virtual const std::shared_ptr<IModel> GetDataArr()const override
	{
		return mDataArr;
	}

	std::shared_ptr<DATA_ARR>				mDataArr;
	
	TTable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
		:ITable(option)
		, mDataArr(new DATA_ARR)
	{
		this->Insert(mDataArr);
	}

	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)override
	{
		if (mDataArr->GetChildQty() <= row)
			return;
		auto mrow = mDataArr->at(row);
		if (!mrow || msNull == mrow->GetState())
			return;

		const auto& row_data = mrow->GetData();
		//col--;
		if (row_data.size() > col)
			val = row_data.at(col);
	}
	virtual bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const override
	{
		if (mDataArr->GetChildQty() <= row)
			return false;

		const ModelState state = mDataArr->GetChild(row)->GetState();

		switch (state)
		{
			//msNull
		default:  break;
		case msCreated:
			attr.SetBold(true);
			attr.SetColour(*wxBLUE);
			break;
		case msExist:
			attr.SetBold(false);
			attr.SetColour(*wxBLACK);
			break;
		case msUpdated:
			attr.SetBold(true);
			attr.SetColour(wxColour(128, 64, 0));
			break;
		case msDeleted:
			attr.SetBold(true);
			attr.SetColour(*wxRED);
			break;
		}//switch
		return true;
	}

};


//-------------------------------------------------------------------------

}//namespace wh{
#endif // __****_H