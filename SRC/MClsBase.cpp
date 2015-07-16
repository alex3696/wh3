#include "_pch.h"
#include "MClsBase.h"


using namespace wh;

const std::vector<Field> gClsNodeFieldVec =
{
	{ "Имя", FieldType::ftName, true },
	{ "Комментарий", FieldType::ftText, true },
	{ "Messure", FieldType::ftName, true },
	{ "ID", FieldType::ftInt, true },
	{ "Тип", FieldType::ftName, false },
	{ "Parent", FieldType::ftName, false }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsBase::MClsBase(const char option)
	:TModelData<rec::Cls>(option)
{
}
//-------------------------------------------------------------------------
bool MClsBase::GetSelectQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"SELECT id, label, description, type, measurename, pid "
		" FROM t_cls "
		//" WHERE id=%s"
		//, cls.mID);
		" WHERE label='%s' "
		, cls.mLabel);
	return true;
}
//-------------------------------------------------------------------------
bool MClsBase::GetInsertQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.IsEmpty() ? "1" : cls.mParent;

	wxString measure;
	if ("0" == cls.mType)
		measure = "NULL";
	else if ("1" == cls.mType)
		measure = "'ед.'";
	else
		measure = "'" + cls.mMeasure + "'";

	query = wxString::Format(
		"INSERT INTO t_cls "
		" (label, description, type, measurename, pid) VALUES "
		" ('%s', %s, %s, %s, '%s') "
		" RETURNING id, label, description, type, measurename, pid, vid"
		, cls.mLabel
		, cls.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", cls.mComment)
		, cls.mType
		, measure
		, parent
		);
	return true;
}
//-------------------------------------------------------------------------
bool MClsBase::GetUpdateQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.IsEmpty() ? "1" : cls.mParent;

	wxString measure;
	if ("0" == cls.mType)
		measure = "NULL";
	else if ("1" == cls.mType)
		measure = "'ед.'";
	else
		measure = "'" + cls.mMeasure + "'";

	query = wxString::Format(
		"UPDATE t_cls SET "
		" label='%s', description=%s, type=%s, measurename=%s, pid=%s "
		" WHERE id = %s "
		, cls.mLabel
		, cls.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", cls.mComment)
		, cls.mType
		, measure
		, parent
		, cls.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MClsBase::GetDeleteQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"DELETE FROM t_cls WHERE id = %s ",
		cls.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MClsBase::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel);
	table->GetAsString(2, row, data.mComment);
	table->GetAsString(3, row, data.mType);
	table->GetAsString(4, row, data.mMeasure);
	table->GetAsString(5, row, data.mParent);
	//table->GetAsString(1, row, data.mVID);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsBase::GetFieldValue(unsigned int col, wxVariant &val)
{
	const auto& rec = this->GetData();
	auto mgr = ResMgr::GetInstance();

	switch (col)
	{
	default: break;
	case 1:	if ("0" == rec.mType)
		val << wxDataViewIconText(rec.mLabel, mgr->m_ico_type_abstract24);
			else if ("1" == rec.mType)
				val << wxDataViewIconText(rec.mLabel, mgr->m_ico_type_num24);
			else
				val << wxDataViewIconText(rec.mLabel, mgr->m_ico_type_qty24);
			break;
	case 2: val = rec.mComment; break;
	case 3: val = rec.mMeasure; break;
	case 4: val = rec.mID; break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsBase::GetFieldVector()const
{
	return gClsNodeFieldVec;
	//return gEmptyFieldVec;
}

