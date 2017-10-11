#include "_pch.h"
#include "CtrlBrowser.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlTableBrowser::CtrlTableBrowser(
	const std::shared_ptr<IViewTableBrowser>& view
	, const std::shared_ptr<ModelPageBrowser>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connViewCmd_Refresh = mView->sigRefresh
		.connect(std::bind(&CtrlTableBrowser::Refresh, this));
	connViewCmd_Up = mView->sigUp
		.connect(std::bind(&CtrlTableBrowser::Up, this));

	connViewCmd_Activate = mView->sigActivate
		.connect(std::bind(&CtrlTableBrowser::Activate, this, ph::_1));


	connModel_GroupByType = mModel->GetModelBrowser()->sigGroupByType.connect
		(std::bind(&IViewTableBrowser::SetGroupByType, mView.get(), ph::_1));

	connModel_BeforePathChange = mModel->GetModelBrowser()->sigBeforePathChange
		.connect(std::bind(&IViewTableBrowser::SetBeforePathChange, mView.get(), ph::_1));
	connModel_AfterPathChange = mModel->GetModelBrowser()->sigAfterPathChange
		.connect(std::bind(&IViewTableBrowser::SetAfterPathChange, mView.get(), ph::_1));

	connModel_BeforeRefreshCls = mModel->GetModelBrowser()->sigBeforeRefreshCls
		.connect(std::bind(&IViewTableBrowser::SetBeforeRefreshCls, mView.get(), ph::_1));
	connModel_AfterRefreshCls = mModel->GetModelBrowser()->sigAfterRefreshCls
		.connect(std::bind(&IViewTableBrowser::SetAfterRefreshCls, mView.get(), ph::_1));



	sig::scoped_connection connModel_BeforePathChanged;
	sig::scoped_connection connModel_AfterPathChanged;


	connModel_ObjOperation = mModel->GetModelBrowser()->sigObjOperation.connect
		(std::bind(&IViewTableBrowser::SetObjOperation, mView.get(), ph::_1, ph::_2));
	
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::Refresh()
{
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::Up()
{
	mModel->GetModelBrowser()->DoUp();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::Activate(const ICls64* cls)
{
	mModel->GetModelBrowser()->DoActivate(cls);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlToolbarBrowser::CtrlToolbarBrowser(
	const std::shared_ptr<IViewToolbarBrowser>& view
	, const std::shared_ptr<ModelPageBrowser>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connModel_GroupByType = mModel->GetModelBrowser()->sigGroupByType.connect
		(std::bind(&IViewToolbarBrowser::SetGroupByType, mView.get(), ph::_1));


	connViewCmd_Refresh = mView->sigRefresh
		.connect(std::bind(&CtrlToolbarBrowser::Refresh, this));
	connViewCmd_Up = mView->sigUp
		.connect(std::bind(&CtrlToolbarBrowser::Up, this));

	connViewCmd_Act = mView->sigAct
		.connect(std::bind(&CtrlToolbarBrowser::Act, this));
	connViewCmd_Move = mView->sigMove
		.connect(std::bind(&CtrlToolbarBrowser::Move, this));
	connViewCmd_Find = mView->sigFind
		.connect(std::bind(&CtrlToolbarBrowser::Find, this, ph::_1));
	
	connViewCmd_AddType = mView->sigAddType
		.connect(std::bind(&CtrlToolbarBrowser::AddType, this));
	connViewCmd_AddObject = mView->sigAddObject
		.connect(std::bind(&CtrlToolbarBrowser::AddObject, this));
	connViewCmd_DeleteSelected = mView->sigDeleteSelected
		.connect(std::bind(&CtrlToolbarBrowser::DeleteSelected, this));
	connViewCmd_UpdateSelected = mView->sigUpdateSelected
		.connect(std::bind(&CtrlToolbarBrowser::UpdateSelected, this));

	connViewCmd_GroupByType = mView->sigGroupByType
		.connect(std::bind(&CtrlToolbarBrowser::GroupByType, this, ph::_1));



}

//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Refresh()
{
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Up()
{
	mModel->GetModelBrowser()->DoUp();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Act()
{
	mModel->GetModelBrowser()->DoAct();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Move()
{
	mModel->GetModelBrowser()->DoMove();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Find(const wxString& str)
{
	mModel->GetModelBrowser()->DoFind(str);
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::AddType()
{
	mModel->GetModelBrowser()->DoAddType();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::AddObject()
{
	mModel->GetModelBrowser()->DoAddObject();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::DeleteSelected()
{
	mModel->GetModelBrowser()->DoDeleteSelected();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::UpdateSelected()
{
	mModel->GetModelBrowser()->DoUpdateSelected();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::GroupByType(bool enable_group_by_type)
{
	mModel->GetModelBrowser()->DoGroupByType(enable_group_by_type);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPathBrowser::CtrlPathBrowser(
	const std::shared_ptr<IViewPathBrowser>& view
	, const std::shared_ptr<ModelPageBrowser>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connModel_PathChanged = mModel->GetModelBrowser()->sigAfterPathChange
		.connect(std::bind(&IViewPathBrowser::SetPathString, mView.get(), ph::_1));

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPageBrowser::CtrlPageBrowser(
	const std::shared_ptr<IViewBrowserPage>& view
	, const std::shared_ptr<ModelPageBrowser>& model)
	: CtrlWindowBase(view, model)
{
	mCtrlTableBrowser
		= std::make_shared<CtrlTableBrowser>(view->GetViewTableBrowser(), model);

	mCtrlToolbarBrowser
		= std::make_shared<CtrlToolbarBrowser>(view->GetViewToolbarBrowser(), model);

	mCtrlPathBrowser
		= std::make_shared<CtrlPathBrowser>(view->GetViewPathBrowser(), model);


}