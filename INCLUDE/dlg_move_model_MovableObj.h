#ifndef __DLG_MOVE_MODEL_MOVABLEOBJ_H
#define __DLG_MOVE_MODEL_MOVABLEOBJ_H

#include "TModelArray.h"
#include "db_rec.h"

#include "dlg_move_model_DstObj.h"
#include "dlg_move_model_DstType.h"

/** Full hierarchy
namespace wh
{
	namespace obj_catalog
	{
		namespace model
		{
			// dlg_move_model_MovableObj
			// dlg_move_model_DstTypeArray
			// dlg_move_model_DstType
			// obj_catalog_model_DstObjArray
			// obj_catalog_model_DstObj
		};

		namespace view
		{
			// dlg_move_view_Frame
			// dlg_move_view_Tree
			// dlg_move_view_DwModel
		};

	};
}//namespace wh{

*/


namespace wh {
namespace rec {







} // namespace model {
namespace dlg_move {
namespace model {

//template <typename T> using sptr = std::shared_ptr<T>;
//-------------------------------------------------------------------------	
class MovableObj
	: public TModelData<rec::PathItem>
{
public:
	MovableObj();

	const std::shared_ptr<DstTypeArray>&	GetDstTypes()const;

	void Unlock();
	void Move(std::shared_ptr<model::DstObj> dst, const wxString& qty = "1");

	std::shared_ptr<DstObj> GetDstObj(){ return mDstObj; }
	std::shared_ptr<TModelData<wxString>> GetQty(){ return mQty; }
	
protected:
	std::shared_ptr<DstTypeArray>			mDstTypes;
	std::shared_ptr<DstObj>					mDstObj;
	std::shared_ptr<TModelData<wxString>>	mQty;
	
	virtual void LoadChilds()override;

	
};


//-------------------------------------------------------------------------
} // namespace model {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H
