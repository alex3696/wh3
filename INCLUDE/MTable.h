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
//-------------------------------------------------------------------------
class ITable
	: public IModel
{
public:
	ITable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	std::shared_ptr<IFieldArray>	mFieldVec;
	std::shared_ptr<UIntData>		mPageLimit;
	std::shared_ptr<UIntData>		mPageNo;

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant);

	virtual std::shared_ptr<ITableRow>  CreateItem(const TableRowData& data
		= TableRowData(), bool stored = false)const;

	std::shared_ptr<ITableRow> at(size_t pos)const;

	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col);
	virtual wxString GetTableName()const;
protected:
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t row)override;

	void OnFieldAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newVec
		, const SptrIModel& itemBefore);
	void OnFieldBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);

	virtual bool GetSelectChildsQuery(wxString& query)const override;

};

}//namespace wh{
#endif // __****_H