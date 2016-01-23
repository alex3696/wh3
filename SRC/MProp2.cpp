#include "_pch.h"
#include "MProp2.h"

using namespace wh;






//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MPropItem2::MPropItem2(const char option)
	:ITableRow(option)
{

}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MPropTable::MPropTable(const char option)
	:ITable(option)
{
	std::vector<SptrIModel> fields;
	
	fields.emplace_back(mFieldVec->CreateItem(Field("Èìÿ", FieldType::ftName, true,"title"), true));
	fields.emplace_back(mFieldVec->CreateItem(Field("Òèï", FieldType::ftName, true, "kind"), true));
	
	Field fieldID("ID", FieldType::ftLong, true, "id");
	fieldID.mKey = true;
	fieldID.mGuiEdit = false;
	fieldID.mInsert = false;
	fieldID.mUpdate = false;

	//fieldID.mFilter.emplace_back(Field::Filter("100"));
	fields.emplace_back(mFieldVec->CreateItem(fieldID, true));

	mFieldVec->Insert(fields);

}

//-------------------------------------------------------------------------
void MPropTable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)
{
	auto mrow = at(row);
	if (!mrow)
		return;
	const auto& row_data = mrow->GetData();
	
	switch (col)
	{
	case 0: 
		if (row_data.size()>col)
			val = row_data.at(0); 
		return;
	case 1: 
		if (row_data.size()>col)
			if (!row_data.at(1).IsEmpty())
				val = ToText(ToFieldType(row_data.at(1))); 
		return;
	case 2: 
		if (row_data.size()>col)
			val = row_data.at(2); 
		return;
	default:break;
	}
}
//-------------------------------------------------------------------------
wxString MPropTable::GetTableName()const
{ 
	return "prop"; 
};