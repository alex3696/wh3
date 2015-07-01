#ifndef __DLG_MOVE_MODEL_DSTTYPE_H
#define __DLG_MOVE_MODEL_DSTTYPE_H

#include "dlg_move_model_MovableObj.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_move {
namespace model {

//-------------------------------------------------------------------------
class DstType
	: public TModelData<rec::Cls>
{
public:
	std::shared_ptr<DstObjArray>		mObjects;
	
	DstType();

	std::shared_ptr<DstObjArray> GetObjects()const;
	
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	
	inline bool IsAbstract()const { return GetData().IsAbstract(); }
protected:
	
	virtual void LoadChilds()override;
};
//-------------------------------------------------------------------------	

class DstTypeArray
	: public TModelArray<DstType>
{
public:
	DstTypeArray();

	std::map<wxString, std::shared_ptr<DstType> > mTypeUnique;
protected:

	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual void LoadChilds()override;
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t pos)override;

	
};
//-------------------------------------------------------------------------
} // namespace model {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H
