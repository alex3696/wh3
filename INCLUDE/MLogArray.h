#ifndef __MLOGARRAY_H
#define __MLOGARRAY_H

#include "db_rec.h"
#include "TModelArray.h"

namespace wh{

//-------------------------------------------------------------------------
class MLogItem
	: public TModelData<rec::PathNode>
	, public std::enable_shared_from_this<MLogItem>
{
public:
	MLogItem(const char option = ModelOption::EnableParentNotify);

};
//-------------------------------------------------------------------------	

class MLogArray
	: public TModelArray<MLogItem>
{
public:
	MLogArray(const char option = ModelOption::EnableNotifyFromChild);



};

//-------------------------------------------------------------------------


} //namespace model {

#endif // __****_H