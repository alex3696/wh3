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
		//| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave)
		:ITableRow(option)
	{

	}

};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MPropTable
	: public TTable<TTableDataArr<MPropItem2> >
{
public:
	MPropTable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);


	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)override;
protected:
	

};





}//namespace wh{
#endif // __****_H