#include "_pch.h"
#include "MHistory.h"

using namespace wh;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MLogItem::MLogItem(const char option)
	:ITableRow(option)
{

}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MLogTable::MLogTable(const char option)
	:ITable(option)
{
	std::vector<SptrIModel> fields;

	Field fieldID("LOG_ID", FieldType::ftLong, true, "log_id");
	fieldID.mKey = true;
	fieldID.mGuiEdit = false;
	fieldID.mInsert = false;
	fieldID.mUpdate = false;
	fields.emplace_back(mFieldVec->CreateItem(fieldID, true));


	fields.emplace_back(
		mFieldVec->CreateItem(Field("Дата+Время+Пояс", FieldType::ftDate, true, "log_time"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Пользователь", FieldType::ftName, true, "log_user"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Действие", FieldType::ftName, true, "act_title"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Тип", FieldType::ftName, true, "mcls_title"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("Объект", FieldType::ftName, true, "mobj_title"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("Количество", FieldType::ftDouble, true, "qty"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Откуда", FieldType::ftText, true, "src_path"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("Куда", FieldType::ftText, false, "dst_path"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Свойства", FieldType::ftText, true, "prop"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Цвет", FieldType::ftLong, false, "act_color"), true));


	mFieldVec->Insert(fields);
}

//-------------------------------------------------------------------------
void MLogTable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)
{
	auto mrow = at(row);
	if (!mrow)
		return;
	const auto& row_data = mrow->GetData();
	if (row_data.size() <= col)
		return;


	switch (col)
	{
	case 7: 
		val = (row_data.at(7).IsEmpty() ? "/" : row_data.at(7))
			 + wxString("\n") + row_data.at(8);
		break;
	default:val = row_data.at(col);
		break;
	}

}
//-------------------------------------------------------------------------
bool MLogTable::GetAttrByRow(unsigned int row
	, unsigned int col, wxDataViewItemAttr &attr) const 
{
	auto mrow = at(row);
	if (!mrow)
		return false;
	const auto& row_data = mrow->GetData();
	if (row_data.size() <= col)
		return false;

	if (!row_data.at(10).IsEmpty() && col==3)
	{
		wxColour clr(row_data.at(10));
		if (*wxWHITE != clr)
		{
			attr.SetBackgroundColour(clr);
			return true;
		}
	}
	
	//attr.SetBackgroundColour(*wxWHITE);
	//return ITable::GetAttrByRow(row, col, attr);
	//return true;
	return false;
}
//-------------------------------------------------------------------------
wxString MLogTable::GetTableName()const
{
	return "log";
};