#ifndef __CTRLMAIN_H
#define __CTRLMAIN_H

#include "ICtrlWindow.h"
#include "ViewMain.h"
#include "ModelMain.h"


namespace wh{
//---------------------------------------------------------------------------
class CtrlMain : public CtrlWindowBase<ViewMain, ModelMain>
{
	std::shared_ptr<CtrlNotebook> mCtrlNotebook;

	sig::scoped_connection connAfterDbConnected;
	sig::scoped_connection connBeforeDbDisconnected;

	sig::scoped_connection connViewCmd_MkHistoryWindow;


	void OnConnectDb(const whDB& db);
	void OnDicsonnectDb(const whDB& db);
public:
	CtrlMain(const std::shared_ptr<ViewMain>& view, const std::shared_ptr<ModelMain>& model);

	void MkHistoryWindow();
	std::shared_ptr<CtrlNotebook> GetNotebook();

	virtual void RmView()override;
	virtual void Load(const boost::property_tree::ptree& app_cfg) override;
	virtual void Save(boost::property_tree::ptree& app_cfg) override;

	void Load();
	void Save();


	
};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H