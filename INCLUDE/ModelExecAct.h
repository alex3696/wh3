#ifndef __MODELEXECACT_H
#define __MODELEXECACT_H

#include "ModelActBrowser.h"
#include "ModelBrowser.h"
#include "ModelPropPg.h"


namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class ModelActExecWindow : public IModelWindow
{
	int	 mCurrentPage = 0;
	std::set<ObjectKey> mObjects;
	
	ActRec64 mAct;

	ModelActBrowserWindow::FuncActivateCallback mOnActivateAct;

	void UnlockObjectsWithoutTransaction();

	int BuildExecQuery(wxString& query);
public:
	std::shared_ptr<ModelActBrowserWindow>	mModelActBrowser;
	std::shared_ptr<ModelBrowser>			mModelObjBrowser;
	std::shared_ptr<ModelPropPg>			mModelPropPg;
	
		
	ModelActExecWindow();
	~ModelActExecWindow();
	void LockObjects(const std::set<ObjectKey>& obj);
	void UnlockObjects();
	void DoShowActProperty();
	void DoShowActList();
	void DoSelectAct();
	void DoExecute();
	

	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Init()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

	sig::signal<void(int)> sigSelectPage;
};
//---------------------------------------------------------------------------



}//namespace wh{
#endif // __****_H