#ifndef __DLG_ACT_MODEL_OBJ_H
#define __DLG_ACT_MODEL_OBJ_H

#include "dlg_act_model_Act.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace model {
//-------------------------------------------------------------------------

class Obj
	: public TModelData<rec::PathItem>
	, public std::enable_shared_from_this<Obj>

{
public:
	Obj(const char option = ModelOption::EnableParentNotify);
	void Unlock();
	

	std::shared_ptr<ActArray>			mActArray;
};





//-------------------------------------------------------------------------
} // namespace model {
} // namespace dlg_act {
} // namespace wh{
#endif // __****_H
