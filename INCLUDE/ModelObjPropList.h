#ifndef __MODEL_OBJPROPLIST_H
#define __MODEL_OBJPROPLIST_H

#include "ModelHistoryData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ModelObjPropList : public IModelWindow
{
	//sig::scoped_connection connListItemChange;
public:
	ModelObjPropList();

	void Update();
	sig::signal<void(const PropValTable& rt, const IAct*)>	sigUpdatePropList;

	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

};
//---------------------------------------------------------------------------





} //namespace mvp{
#endif // __IMVP_H