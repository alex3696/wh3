#ifndef __MPROP2_H
#define __MPROP2_H

#include "db_rec.h"
#include "MTable.h"

namespace wh{

//-------------------------------------------------------------------------
class MPropItem2
	:public ITableRow

{
public:
	MPropItem2(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);
	

	
	
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MPropTable
	: public ITable
{
public:
	MPropTable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		return std::make_shared<MPropItem2>();
	};

	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)override;
protected:
	

	virtual wxString GetTableName()const override;
};





}//namespace wh{
#endif // __****_H