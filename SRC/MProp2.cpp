#include "_pch.h"
#include "MProp2.h"

using namespace wh;

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// MPropTable
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MPropTable::MPropTable(const char option)
	:TTable<TTableDataArr<MPropItem2> > (option)
{
	this->mTableName->SetData("prop", true);
	
	std::vector<SptrIModel> fields;
	
	fields.emplace_back(mFieldVec->CreateItem(Field("Имя", FieldType::ftName, true,"title"), true));
		
	Field field_type("Тип", FieldType::ftLong, true, "kind");
	field_type.mEditor = FieldEditor::Type;
	fields.emplace_back(mFieldVec->CreateItem(field_type, true));
	
	Field fieldID("ID", FieldType::ftLong, true, "id");
	fieldID.mKey = true;
	fieldID.mGuiEdit = false;
	fieldID.mInsert = false;
	fieldID.mUpdate = false;

	//fieldID.mFilter.emplace_back(Field::Filter("100"));
	fields.emplace_back(mFieldVec->CreateItem(fieldID, true));

	Field field_var("Варианты", FieldType::ftTextArray, true, "var");
	Field field_var_strict("Только варианты", FieldType::ftBool, true, "var_strict");
	fields.emplace_back(mFieldVec->CreateItem(field_var, true));
	fields.emplace_back(mFieldVec->CreateItem(field_var_strict, true));

	mFieldVec->Insert(fields);

}

//-------------------------------------------------------------------------
void MPropTable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)
{
	if (mDataArr->GetChildQty() <= row)
		return;
	auto mrow = mDataArr->at(row);
	if (!mrow || msNull==mrow->GetState())
		return;

	const auto& row_data = mrow->GetData();
	if (row_data.size() <= col)
		return;

	switch (col)
	{
	case 1: if (!row_data.at(col).IsEmpty())
				val = ToText(ToFieldType(row_data.at(1))); 
		break;
	case 0:
	case 2:
	default:val = row_data.at(col);
		break;
	}
}
