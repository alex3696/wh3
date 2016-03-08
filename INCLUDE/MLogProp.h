#ifndef __MLOGPROP_H
#define __MLOGPROP_H

#include "db_rec.h"
#include "MTable.h"


namespace wh{
//-------------------------------------------------------------------------

class MLogPropItem
	:public ITableRow
{
public:
	MLogPropItem(const char option = ModelOption::EnableParentNotify)
		:ITableRow(option)
	{}

protected:
	virtual bool GetSelectQuery(wxString&)const override	{ return false; }
	virtual bool GetInsertQuery(wxString&)const override	{ return false; }
	virtual bool GetUpdateQuery(wxString&)const override	{ return false; }
	virtual bool GetDeleteQuery(wxString&)const override	{ return false; }


};
//-------------------------------------------------------------------------
class MLogProp
	: public ITable
{
public:
	MLogProp(const char option = ModelOption::EnableNotifyFromChild);

	void SetLogProp(const std::set<unsigned long>& prop_id);
	
	virtual std::shared_ptr<IModel> CreateChild()override;
	virtual bool GetSelectChildsQuery(wxString& query)const override;
protected:
	wxString mPropArr;
};


//-------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H