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
	namespace ph = std::placeholders;
	
	auto fnBI = std::bind(&MLogTable::OnRowBeforeInsert, this, ph::_1, ph::_2, ph::_3);
	auto fnAR = std::bind(&MLogTable::OnRowAfterRemove, this, ph::_1, ph::_2);
	mConnRowBI = ConnBeforeInsert(fnBI);
	mConnRowAR = ConnectAfterRemove(fnAR);

	std::vector<SptrIModel> fields;

	Field fieldID("LOG_ID", FieldType::ftLong, false, "log_id");
	fieldID.mKey = true;
	fieldID.mGuiEdit = false;
	fieldID.mInsert = false;
	fieldID.mUpdate = false;
	fields.emplace_back(mFieldVec->CreateItem(fieldID, true));


	Field filed_log_dt("���� �����", FieldType::ftDateTime, true, "log_dt");
	filed_log_dt.mSort = -1;
	fields.emplace_back(mFieldVec->CreateItem(filed_log_dt, true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("������������", FieldType::ftName, true, "log_user"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("��������", FieldType::ftName, true, "act_title"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("���", FieldType::ftName, true, "mcls_title"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("������", FieldType::ftName, true, "mobj_title"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("����������", FieldType::ftDouble, true, "qty"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("������", FieldType::ftText, true, "src_path"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("����", FieldType::ftText, true, "dst_path"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("��������", FieldType::ftText, false, "prop"), true));
	// 10
	fields.emplace_back(
		mFieldVec->CreateItem(Field("����", FieldType::ftLong, false, "act_color"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("����", FieldType::ftDate, false, "log_date"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("�����ID", FieldType::ftLong, false, "mcls_id"), true));


	mFieldVec->Insert(fields);

	mStaticColumnQty = mFieldVec->size();
	/*
	mStaticColumnQty = 0;
	for (unsigned int i = 0; i < mFieldVec->GetChildQty(); ++i)
	{
		if (mFieldVec->at(i)->GetData().mGuiShow)
			mStaticColumnQty++;
	}
	*/


	
}

//-------------------------------------------------------------------------
void MLogTable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)
{
	auto mrow = at(row);
	if (!mrow)
		return;
	const auto& row_data = mrow->GetData();
	//if (row_data.size() <= col)
	//	return;

	wxString tmp;
	wxDateTime dt;
	

	switch (col)
	{
		
	case 1: 
		dt.ParseDateTime(row_data.at(1));
		val = dt.Format(wxS("%Y.%m.%d\n%H:%M"));
		break;
		/*
	case 7: 
		val = (row_data.at(7).IsEmpty() ? "/" : row_data.at(7))
			 + wxString("\n") + row_data.at(8);
		break;
		*/
	default:
		if (col>=mStaticColumnQty)
		{
			boost::property_tree::ptree prop_arr;
			const wxString& json_prop = row_data.at(9);
			if (!json_prop.IsEmpty())
			{
				std::stringstream ss; ss << json_prop;
				boost::property_tree::read_json(ss, prop_arr);
			}
			//const boost::property_tree::ptree& prop_arr = mProp[row];
			const auto prop_row = mLogProp.at(col - mStaticColumnQty);
			const auto& prop_row_data = prop_row->GetData();
			const wxString& id = prop_row_data.at(0);
			auto it = prop_arr.find(std::string(id.c_str()));
			if(it != prop_arr.not_found())
				val = it->second.get_value<std::string>();
		}
		else
			val = row_data.at(col);
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
}
//-------------------------------------------------------------------------
void MLogTable::OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	if (!vec.size())
		mClsId.clear();
}
//-------------------------------------------------------------------------
bool MLogTable::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& db, const size_t pos)
{
	bool res = ITable::LoadChildDataFromDb(child, db, pos);
	if (res)
	{
		auto mrow = std::dynamic_pointer_cast<ITableRow>(child);
		if (mrow)
		{
			unsigned long val = 0;
			if (mrow->GetData().at(12).ToCULong(&val))
				mClsId.emplace(val);
		}
	}
	return res;
}
//-------------------------------------------------------------------------
void MLogTable::OnRowBeforeInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	std::vector<SptrIModel> rem_fields;
	for (auto col = mStaticColumnQty; col < mFieldVec->size(); ++col)
		rem_fields.emplace_back(mFieldVec->at(col));
	mFieldVec->DelChild(rem_fields);

	mLogProp.SetLogProp(mClsId);
	std::vector<SptrIModel> fields;
	for (size_t i = 0; i < mLogProp.size(); ++i)
	{
		const wxString id = mLogProp.at(i)->GetData().at(0);
		const wxString title = mLogProp.at(i)->GetData().at(1);
		
		fields.emplace_back(mFieldVec->CreateItem(
			//Field(col_title, FieldType::ftName, true, col_title), true));
			Field(title, FieldType::ftName, true, "prop->>'" + id + "'"), true));
	}
	mFieldVec->Insert(fields);


	unsigned int row_idx = 0;
	for (const auto& irow : newItems)
	{
		auto row = std::dynamic_pointer_cast<MLogItem>(irow);
		const auto& data = row->GetData();
		const wxString& col_prop = data.at(9);
		boost::property_tree::ptree prop_arr;

		if (!col_prop.IsEmpty())
		{
			std::stringstream ss; ss << col_prop;
			boost::property_tree::read_json(ss, prop_arr);
		}
		//mProp.emplace_back(prop_arr);
	}


}