#include "_pch.h"
#include "MLogProp.h"

using namespace wh;

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// MLogPropItem
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MLogPropItem::LoadThisDataFromDb(std::shared_ptr<whTable>& db, const size_t pos)
{
	return ITableRow::LoadThisDataFromDb(db, pos);
}

//-------------------------------------------------------------------------
MLogProp::MLogProp(const char option)
	: TTable<MLogPropDataArr>(option)
{
	std::vector<SptrIModel> fields;

	fields.emplace_back(
		mFieldVec->CreateItem(Field("id", FieldType::ftLong, true, "id"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("Èìÿ", FieldType::ftName, true, "title"), true));
	fields.emplace_back(
		mFieldVec->CreateItem(Field("Òèï", FieldType::ftLong, true, "kind"), true));

	mFieldVec->Insert(fields);
}


//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void MLogProp::SetLogProp(const std::set<unsigned long>& prop_id)
{
	mPropArr.clear();
	this->mDataArr->mPropAct.clear();
	for (const unsigned long& id : prop_id)
		mPropArr << id << ",";
	mPropArr.RemoveLast();
	
	this->Load();
}
//-------------------------------------------------------------------------
bool MLogPropDataArr::GetSelectChildsQuery(wxString& query)const
{
	auto mtable = dynamic_cast<MLogProp*>(GetParent());
	if (!mtable)
		return false;


	query = wxString::Format(
		"SELECT  ref_act_prop.prop_id "
		"      , prop.title "
		"      , prop.kind "
		"	   , ref_act_prop.act_id "
		"      , CASE WHEN fav.prop_id IS NOT NULL AND fav.prop_id<>0 "
		"             THEN TRUE ELSE FALSE  "
		"	     END AS selected "
		"FROM(SELECT unnest('{%s}'::BIGINT[]))curr_act "
		"LEFT  JOIN ref_act_prop ON curr_act.unnest = ref_act_prop.act_id "
		"LEFT  JOIN prop  ON prop.id = ref_act_prop.prop_id "
		"INNER JOIN favorite_prop fav "
		"  ON  fav.prop_id = ref_act_prop.prop_id "
		"  AND fav.act_id = ref_act_prop.act_id "
		"  AND fav.user_label = CURRENT_USER "
		"ORDER BY ref_act_prop.act_id, ref_act_prop.prop_id "
		, mtable->GetPropArr());
	return true;
}
//-------------------------------------------------------------------------
bool MLogPropDataArr::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	bool ret = TTableDataArr<MLogPropItem>::LoadChildDataFromDb(child, table, pos);
	auto childModel = std::dynamic_pointer_cast<MLogPropItem>(child);
	if (!ret || !childModel)
		return false;

	ret = false;
	long prop_id = 0;
	long act_id = 0;
	long visible = !childModel->GetData().at(4).IsNull() ;
	if (childModel->GetData().at(0).ToLong(&prop_id)
		&& childModel->GetData().at(3).ToLong(&act_id)
		&& visible
		)
	{
		
		auto it = mPropAct.find(prop_id);
		if (it == mPropAct.end())
			ret = true;
			
		PropSet tmp = mPropAct[prop_id];
		tmp.insert(std::make_pair(act_id, childModel));
		mPropAct[prop_id] = tmp;
	}
		
	return ret;
}



