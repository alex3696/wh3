#ifndef __MHISTORY_H
#define __MHISTORY_H

#include "db_rec.h"
#include "MTable.h"

namespace wh{

//-------------------------------------------------------------------------
class MLogItem
	:public ITableRow

{
public:
	MLogItem(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);
	

	
	
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MLogTable
	: public ITable
{
public:
	MLogTable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		return std::make_shared<MLogItem>();
	};

	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)override;
	virtual bool GetAttrByRow(unsigned int row
		, unsigned int col, wxDataViewItemAttr &attr) const override;
protected:
	

	virtual wxString GetTableName()const override;
};





}//namespace wh{
#endif // __****_H