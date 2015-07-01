#ifndef __MRPROP_H
#define __MRPROP_H

#include "rdbs.h"

namespace wh{

class MRPropArray
	//:public rdbs::Table<rdbs::Record>
	:public rdbs::Table
{
public:
	MRPropArray();

	//bool GetFieldValue(unsigned int row, unsigned int col, wxVariant &variant);
protected:
	virtual bool GetSelectAllQuery(wxString& query)const;
	virtual bool GetSelectQuery(wxString& query, rdbs::RecordSp& rec)const;
	virtual bool GetInsertQuery(wxString& query, rdbs::RecordSp& rec)const;
	virtual bool GetUpdateQuery(wxString& query, rdbs::RecordSp& rec)const;
	virtual bool GetDeleteQuery(wxString& query, rdbs::RecordSp& rec)const;
};





}//namespace wh{
#endif // __****_H