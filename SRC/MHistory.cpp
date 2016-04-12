#include "_pch.h"
#include "MHistory.h"

using namespace wh;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// MLogTableDataArr
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MLogTableDataArr::MLogTableDataArr(const char option)
	:TTableDataArr<MLogItem>(option)
{
	namespace ph = std::placeholders;

	auto fnBI = std::bind(&MLogTableDataArr::OnRowBeforeInsert, this, ph::_1, ph::_2, ph::_3);
	auto fnAR = std::bind(&MLogTableDataArr::OnRowAfterRemove, this, ph::_1, ph::_2);
	mConnRowBI = ConnBeforeInsert(fnBI);
	mConnRowAR = ConnectAfterRemove(fnAR);

}
//-------------------------------------------------------------------------
bool MLogTableDataArr::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& db, const size_t pos)
{
	bool res = TTableDataArr<MLogItem>::LoadChildDataFromDb(child, db, pos);
	if (res)
	{
		auto mrow = std::dynamic_pointer_cast<ITableRow>(child);
		if (mrow)
		{
			unsigned long val = 0;
			if (mrow->GetData().at(14).ToCULong(&val))
				mActId.emplace(val);
		}
	}
	return res;
}
//-------------------------------------------------------------------------
void MLogTableDataArr::OnRowBeforeInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	auto mtable = dynamic_cast<MLogTable*>(GetParent());
	if (!mtable)
		return;
	
	// удаляем столбцы свойств
	std::vector<SptrIModel> rem_fields;
	for (auto col = mtable->mStaticColumnQty; col < mtable->mFieldVec->size(); ++col)
		rem_fields.emplace_back(mtable->mFieldVec->at(col));
	mtable->mFieldVec->DelChild(rem_fields);

	mLogProp.SetLogProp(mActId);
	std::vector<SptrIModel> fields;
	for (size_t i = 0; i < mLogProp.mDataArr->size(); ++i)
	{
		const wxString id = mLogProp.mDataArr->at(i)->GetData().at(0);
		const wxString title = mLogProp.mDataArr->at(i)->GetData().at(1);
		const wxString fav = mLogProp.mDataArr->at(i)->GetData().at(4);

		fields.emplace_back(mtable->mFieldVec->CreateItem(
			//Field(col_title, FieldType::ftName, true, col_title), true));
			Field(title, FieldType::ftName, true, "prop->>'" + id + "'"), true));
	}
	// добавляем столбцы свойств
	mtable->mFieldVec->Insert(fields);

}
//-------------------------------------------------------------------------
void MLogTableDataArr::OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	if (!vec.size())
		mActId.clear();
}


	
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MLogTable::MLogTable(const char option)
	: TTable<MLogTableDataArr>(option)
{
	this->mTableName->SetData("log", true);
	

	std::vector<SptrIModel> fields;

	Field fieldID("LOG_ID", FieldType::ftLong, false, "log_id");
	fieldID.mKey = true;
	fieldID.mGuiEdit = false;
	fieldID.mInsert = false;
	fieldID.mUpdate = false;
	fields.emplace_back(mFieldVec->CreateItem(fieldID, true));


	Field filed_log_dt("Дата время", FieldType::ftDateTime, true, "log_dt");
	filed_log_dt.mSort = -1;
	fields.emplace_back(mFieldVec->CreateItem(filed_log_dt, true));

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
		mFieldVec->CreateItem(Field("Куда", FieldType::ftText, true, "dst_path"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Свойства", FieldType::ftText, false, "prop"), true));
	// 10
	fields.emplace_back(
		mFieldVec->CreateItem(Field("Цвет", FieldType::ftLong, false, "act_color"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("Дата", FieldType::ftDate, false, "log_date"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("КлассID", FieldType::ftLong, false, "mcls_id"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("ОбъектID", FieldType::ftLong, false, "mobj_id"), true));

	fields.emplace_back(
		mFieldVec->CreateItem(Field("ActID", FieldType::ftLong, false, "act_id"), true));


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
	auto mrow = mDataArr->at(row);
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
			// получаем столбец с свойством 

			const auto prop_row = mDataArr->mLogProp.mDataArr->at(col - mStaticColumnQty);
			const auto& prop_row_data = prop_row->GetData();
			const wxString& pid_str = prop_row_data.at(0);// получаем идентификатор свойства

			long act_id, prop_id;
			if (row_data.at(14).ToLong(&act_id) && pid_str.ToLong(&prop_id))
			{
				const auto& pa = this->mDataArr->mLogProp.mDataArr->mPropAct;
				auto it = pa.find(prop_id);
				if (it != pa.end())
				{
					const MLogPropDataArr::PropSet& ps = it->second;

					MLogPropDataArr::PropSet::const_iterator
						itp = ps.find(act_id);
					if (itp != ps.end())
					{
						const auto& as = *itp;
						if (!as.second->GetData().at(4).IsEmpty())
						{
							boost::property_tree::ptree prop_arr;
							const wxString& json_prop = row_data.at(9);
							if (!json_prop.IsEmpty())
							{
								std::stringstream ss; ss << json_prop;
								boost::property_tree::read_json(ss, prop_arr);
							}

							auto it = prop_arr.find(std::string(pid_str.c_str()));
							if (it != prop_arr.not_found())
								val = it->second.get_value<std::string>();
						}
					}
				}
			}








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
	bool bg_color = false;

	auto mrow = mDataArr->at(row);
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
			bg_color = true;
		}
	}
	
	return TTable<MLogTableDataArr>::GetAttrByRow(row, col, attr) || bg_color;
}
