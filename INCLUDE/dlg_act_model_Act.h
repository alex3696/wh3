#ifndef __DLG_ACT_MODEL_ACT_H
#define __DLG_ACT_MODEL_ACT_H


#include "dlg_act_model_ActProp.h"


//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace model {
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
class Act
	: public TModelData<rec::Act>
	, public std::enable_shared_from_this<Act>

{
public:
	std::shared_ptr<PropArray>	mPropArray;


	Act(const char option = ModelOption::EnableParentNotify);
		
		
	std::shared_ptr<PropArray> GetPropArray()const;

	void DoAct();

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	virtual void LoadChilds()override;
		
};

//-------------------------------------------------------------------------
class ActArray
	: public TModelArray<Act>
{
public:
	ActArray();
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	//virtual void LoadChilds()override;
	//virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	//	std::shared_ptr<whTable>& table, const size_t pos)override;


};







//-------------------------------------------------------------------------
} // namespace model {
} // namespace dlg_act {
} // namespace wh{
#endif // __****_H
