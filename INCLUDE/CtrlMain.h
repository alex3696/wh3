#ifndef __CTRLMAIN_H
#define __CTRLMAIN_H

#include "ICtrlWindow.h"
#include "ViewMain.h"
#include "ModelMain.h"

#include "globaldata.h"

namespace wh{
//---------------------------------------------------------------------------
class CtrlMain : public CtrlWindowBase<ViewMain, ModelMain>
{
	std::shared_ptr<CtrlNotebook> mCtrlNotebook;

	sig::scoped_connection connAfterDbConnected;
	sig::scoped_connection connBeforeDbDisconnected;

	sig::scoped_connection connViewCmd_MkPageGroup;
	sig::scoped_connection connViewCmd_MkPageUser;
	sig::scoped_connection connViewCmd_MkPageProp;
	sig::scoped_connection connViewCmd_MkPageAct;
	sig::scoped_connection connViewCmd_MkPageObjByType;
	sig::scoped_connection connViewCmd_MkPageObjByPath;
	sig::scoped_connection connViewCmd_MkPageHistory;
	sig::scoped_connection connViewCmd_MkPageReportList;

	sig::scoped_connection connViewCmd_DoConnectDB;
	sig::scoped_connection connViewCmd_DoDisconnectDB;
	
	void OnConnectDb(const whDB& db);
	void OnDicsonnectDb(const whDB& db);
public:
	CtrlMain(const std::shared_ptr<ViewMain>& view, const std::shared_ptr<ModelMain>& model);

	void ConnectDB();
	void DisconnectDB();

	void MkPageGroup();
	void MkPageUser();
	void MkPageProp();
	void MkPageAct();
	void MkPageObjByType();
	void MkPageObjByPath();
	void MkPageHistory();
	void MkPageReportList();
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