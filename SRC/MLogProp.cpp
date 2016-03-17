#include "_pch.h"
#include "MLogProp.h"

using namespace wh;


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
		"SELECT distinct favorite_prop.prop_id, prop.title, prop.kind "
		" FROM favorite_prop "
		" LEFT JOIN prop  ON prop.id = favorite_prop.prop_id "
		" INNER JOIN "
		"   ( "
		"       SELECT distinct(upcls.id) FROM "
		"         (SELECT id FROM fn_array1_to_table('{%s}'::BIGINT[]))downcls, "
		"          LATERAL(SELECT * FROM get_path_cls_info(downcls.id)) upcls "
		"   )arr ON "
		"   favorite_prop.cls_id = arr.id "
		" WHERE favorite_prop.user_label = CURRENT_USER "
		" ORDER BY title"
		, mtable->GetPropArr());
	return true;
}


