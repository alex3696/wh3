#ifndef __MODELEXECACT_H
#define __MODELEXECACT_H

#include "ModelActBrowser.h"
#include "ModelBrowser.h"


namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class ModelActExecWindow : public IModelWindow
{
public:
	std::shared_ptr<ModelActBrowserWindow>	mModelActBrowser;
	std::shared_ptr<ModelBrowser>			mModelObjBrowser;
		
	ModelActExecWindow();
	void SetObjects(const std::set<ObjectKey>& obj);

	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Init()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

};
//---------------------------------------------------------------------------



}//namespace wh{
#endif // __****_H