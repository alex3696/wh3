#ifndef __MCATOBJ_H
#define __MCATOBJ_H

#include "TModelArray.h"
#include "db_rec.h"

namespace wh{
//-------------------------------------------------------------------------
class MPath
	: public TModelData<SqlString>
{
public:
	MPath(const char option	= ModelOption::EnableParentNotify);

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
};
//-------------------------------------------------------------------------
class MCat
	: public TModelData<bool>
{
public:
	MCat(const char option
		= 
		  ModelOption::EnableNotifyFromChild
		| ModelOption::CommitLoad
		);
};



}//namespace wh
#endif // __*_H