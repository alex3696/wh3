#ifndef __DLG_MOVE_MODEL_DSTOBJ_H
#define __DLG_MOVE_MODEL_DSTOBJ_H

#include "dlg_move_model_MovableObj.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_move {
namespace model {
//-------------------------------------------------------------------------
class DstObj
	: public TModelData<rec::ObjTitle>
	, public std::enable_shared_from_this<DstObj>
	
{
public:
	DstObj(const char option = ModelOption::EnableParentNotify)
		:TModelData<rec::ObjTitle>()
	{};

	//virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	//virtual void LoadChilds()override;
};
//-------------------------------------------------------------------------	

class DstObjArray
	: public TModelArray<DstObj>
{
public:
	DstObjArray(const char option
		= ModelOption::EnableParentNotify)
		: TModelArray<ItemType>(option)
	{}

protected:
	//virtual bool GetSelectChildsQuery(wxString& query)const override;

};
//-------------------------------------------------------------------------
} // namespace model {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H
