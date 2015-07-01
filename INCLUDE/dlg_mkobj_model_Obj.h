#ifndef __DLG_MKOBJ_OBJ_H
#define __DLG_MKOBJ_OBJ_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_mkobj {
namespace model {
//-------------------------------------------------------------------------

class Obj
	: public TModelData<rec::PathItem>
	, public std::enable_shared_from_this<Obj>

{
public:
	Obj(const char option = ModelOption::EnableParentNotify);
	
};





//-------------------------------------------------------------------------
} // namespace model {
} // namespace dlg_mkobj {
} // namespace wh{
#endif // __****_H
