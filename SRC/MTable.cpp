#include "_pch.h"
#include "MTable.h"

using namespace wh;


bool ITableRow::LoadThisDataFromDb(std::shared_ptr<whTable>& db, const size_t row)
{
	auto mtable = dynamic_cast<ITable*>(GetParent());
	if (!mtable)
		return false;

	TableRowData data;

	for (size_t col = 0; col < mtable->mFieldVec->GetChildQty(); col++)
		data.emplace_back(db->GetAsString(col, row));
	SetData(data);
	return true;
}
//-------------------------------------------------------------------------
bool ITableRow::GetSelectQuery(wxString& query)const
{
	auto mtable = dynamic_cast<ITable*>(GetParent());
	if (!mtable)
		return false;
	const wxString table = mtable->GetTableName();
	const auto& field_vec = mtable->mFieldVec;
	const auto& stored_row_data = GetStored();

	wxString fields, keycond, filter_sql;
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

			filter_sql << field.mDbTitle << ToSqlString(filter.mOp)
				<< "'" << filter.mVal << "' ";
		}

		if (field.mKey)
		{
			if (!keycond.IsEmpty())
				keycond << " AND ";
			keycond << field.mDbTitle << "='" << stored_row_data[i]<<"'";
		}
	}
	fields.RemoveLast();

	query.clear();

	query << "SELECT " << fields << " FROM " << table;
	if (!filter_sql.IsEmpty())
		query << " WHERE " << filter_sql;
	
	return true;
}
//-------------------------------------------------------------------------
bool ITableRow::GetInsertQuery(wxString& query)const
{
	auto mtable = dynamic_cast<ITable*>(GetParent());
	if (!mtable)
		return false;
	const wxString table = mtable->GetTableName();
	const auto& field_vec = mtable->mFieldVec;

	const auto& row_data = GetData();

	wxString fields, values, ins_table_fields, keylist, keycond;
	for (unsigned int i = 0; i < field_vec->size(); ++i)
	{
		const auto& field = field_vec->at(i)->GetData();
		if (field.mInsert)
		{
			fields << field.mDbTitle << ",";
			values << "'" << row_data[i] << "',";
		}
		ins_table_fields += "ins." + field.mDbTitle + ",";
		if (field.mKey)
		{ 
			if (!keycond.IsEmpty())
				keycond << " AND ";
			keycond << "ins." << field.mDbTitle << "=" << table << "." << field.mDbTitle;

			keylist << field.mDbTitle + ",";
		}
	}
	fields.RemoveLast();
	values.RemoveLast(); 
	ins_table_fields.RemoveLast();
	keylist.RemoveLast();

	query.clear();

	query << "WITH ins AS( "
		<< "INSERT INTO " << table
		<< "(" << fields << ")VALUES(" << values << ")"
		<< "RETURNING * )"
		<< " SELECT " << ins_table_fields << " FROM ins";

	return true;
}
//-------------------------------------------------------------------------
bool ITableRow::GetUpdateQuery(wxString& query)const
{
	auto mtable = dynamic_cast<ITable*>(GetParent());
	if (!mtable)
		return false;
	const wxString table = mtable->GetTableName();
	const auto& field_vec = mtable->mFieldVec;
	const auto& row_data = GetData();
	const auto& stored_row_data = GetStored();


	wxString update_fields, keycond;
	for (unsigned int i = 0; i < field_vec->size(); ++i)
	{
		const auto& field = field_vec->at(i)->GetData();
		if (field.mUpdate)
		{
			update_fields << field.mDbTitle << "='" << row_data[i] << "',";
		}
		if (field.mKey)
		{
			if (!keycond.IsEmpty())
				keycond << " AND ";
			keycond << field.mDbTitle << "='" << stored_row_data[i]<<"'";
		}
	}
	update_fields.RemoveLast();

	query.clear();
	query	<< "UPDATE prop SET " << update_fields 
		<< " WHERE " << keycond;
	return true;
}
//-------------------------------------------------------------------------
bool ITableRow::GetDeleteQuery(wxString& query)const
{
	auto mtable = dynamic_cast<ITable*>(GetParent());
	if (!mtable)
		return false;
	const wxString table = mtable->GetTableName();
	const auto& field_vec = mtable->mFieldVec;
	const auto& stored_row_data = GetStored();

	wxString keycond;
	for (unsigned int i = 0; i < field_vec->size(); ++i)
	{
		const auto& field = field_vec->at(i)->GetData();
		if (field.mKey)
		{
			if (!keycond.IsEmpty())
				keycond << " AND ";
			keycond << field.mDbTitle << "='" << stored_row_data[i] << "'";
		}
	}

	const auto& data = GetData();
	const auto& id = data[2];
	query.clear();
	query << "DELETE FROM "<< table <<" WHERE " << keycond;
	return true;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
ITable::ITable(const char option)
	:IModel(option)
	, mFieldVec(new IFieldArray)
	, mPageLimit(new UIntData)
	, mPageNo(new UIntData)

{
	namespace ph = std::placeholders;

	auto fnAI = std::bind(&ITable::OnFieldAfterInsert, this, ph::_1, ph::_2, ph::_3);
	auto fnBR = std::bind(&ITable::OnFieldBeforeRemove, this, ph::_1, ph::_2);
		
	mFieldVec->ConnAfterInsert(fnAI);
	mFieldVec->ConnectBeforeRemove(fnBR);

	mPageLimit->SetData(100, true);
	mPageNo->SetData(0, true);
}
//-------------------------------------------------------------------------
bool ITable::GetFieldValue(unsigned int col, wxVariant &variant)
{
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<ITableRow>  ITable::CreateItem(const TableRowData& data, bool stored)const
{
	auto child = std::make_shared < ITableRow >();
	child->SetData(data, stored);
	return child;
}
//-------------------------------------------------------------------------
std::shared_ptr<ITableRow> ITable::at(size_t pos)const
{
	return std::dynamic_pointer_cast<ITableRow>(GetChild(pos));
}
//-------------------------------------------------------------------------
void ITable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)
{
	auto mrow = at(row);
	if (!mrow)
		return;
	const auto& row_data = mrow->GetData();
	//col--;
	if (row_data.size() > col)
		val = row_data.at(col);
}
//-------------------------------------------------------------------------
bool ITable::GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const
{
	const ModelState state = GetChild(row)->GetState();

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
//-------------------------------------------------------------------------
bool ITable::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& table, const size_t row)
{
	auto childModel = std::dynamic_pointer_cast<ITableRow>(child);
	if (!childModel)
		return false;

	TableRowData data;
		
	for (size_t col = 0; col < mFieldVec->GetChildQty(); col++)
		data.emplace_back(table->GetAsString(col, row));
	childModel->SetData(data);
	return true;
}
//-------------------------------------------------------------------------
void ITable::OnFieldAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newVec
	, const SptrIModel& itemBefore)
{
	//this->Clear();
}
//-------------------------------------------------------------------------
void ITable::OnFieldBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	//this->Clear();
}

//-------------------------------------------------------------------------
bool ITable::GetSelectChildsQuery(wxString& query)const
{
	const wxString table = GetTableName();
	const auto& field_vec = this->mFieldVec;

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
	
	const unsigned int page_limit = mPageLimit->GetData();
	const unsigned int page_no = mPageNo->GetData();
	limit_page << " LIMIT " << page_limit << " OFFSET " << page_limit * page_no;

	query.clear();

	query << "SELECT " << fields << " FROM " << table;
	if (!filter_sql.IsEmpty())
		query << " WHERE " << filter_sql;
	query << sort;
	query << limit_page;

	return !query.empty();
}

//-------------------------------------------------------------------------
wxString ITable::GetTableName()const
{
	return wxEmptyString; 
};