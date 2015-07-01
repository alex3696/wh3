#include "_pch.h"
#include "MRProp.h"

using namespace wh;
using namespace wh::rdbs;

//-------------------------------------------------------------------------
MRPropArray::MRPropArray()
	//:rdbs::Table<rdbs::Record>()
	:rdbs::Table()
{
	this->SetFieldArray({ { "label", ftName }
						, { "type", ftInt } 
						, { "id", ftInt }    } );
}
//-------------------------------------------------------------------------
bool MRPropArray::GetSelectAllQuery(wxString& query)const
{
	query = "SELECT label, type, id FROM t_prop";
	return true;

}
//-------------------------------------------------------------------------
bool MRPropArray::GetSelectQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format("SELECT label, type, id FROM t_prop WHERE id=%s"
		, data[0]);
	return true;
}
//-------------------------------------------------------------------------
bool MRPropArray::GetInsertQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format
		("INSERT INTO t_prop(label, type)VALUES('%s', %s)RETURNING id, label, type",
		data[0], data[1]);
	return true;
}
//-------------------------------------------------------------------------
bool MRPropArray::GetUpdateQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format(
		"UPDATE t_prop SET label='%s' , type=%s WHERE id=%s"
		, data[0], data[1], data[2]);
	return true;
}
//-------------------------------------------------------------------------
bool MRPropArray::GetDeleteQuery(wxString& query, RecordSp& rec)const
{
	const auto& data = rec->GetData();
	query = wxString::Format(
		"DELETE FROM t_act WHERE id = %s "
		, data[2]);
	return true;
}