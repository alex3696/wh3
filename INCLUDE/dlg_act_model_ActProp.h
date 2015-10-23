#ifndef __DLG_ACT_MODEL_ACTPROP_H
#define __DLG_ACT_MODEL_ACTPROP_H


#include "TModelArray.h"
#include "db_rec.h"


//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace model {
//-------------------------------------------------------------------------
class Prop
	: public TModelData<rec::PropVal>
	, public std::enable_shared_from_this<Prop>

{
public:
	Prop(const char option = ModelOption::EnableParentNotify);
	
	bool LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row);

	
protected:
	
};
//-------------------------------------------------------------------------
class PropArray
	: public TModelArray<Prop>
{
public:
	PropArray();

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
