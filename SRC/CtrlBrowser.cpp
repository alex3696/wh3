#include "_pch.h"
#include "CtrlBrowser.h"
#include "CtrlNotebook.h"
#include "CtrlClsEditor.h"
#include "CtrlObjEditor.h"
#include "CtrlFav.h"
#include "CtrlHelp.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlTableBrowser::CtrlTableBrowser(
	const std::shared_ptr<IViewTableBrowser>& view
	, const std::shared_ptr<ModelPageBrowser>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connModel_BeforeRefreshCls = mModel->GetModelBrowser()->sigBeforeRefreshCls
		.connect(std::bind(&IViewTableBrowser::SetBeforeRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));
	connModel_AfterRefreshCls = mModel->GetModelBrowser()->sigAfterRefreshCls
		.connect(std::bind(&IViewTableBrowser::SetAfterRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));
	connModel_ObjOperation = mModel->GetModelBrowser()->sigObjOperation
		.connect(std::bind(&IViewTableBrowser::SetObjOperation, mView.get(), ph::_1, ph::_2));

	connViewCmd_Activate = mView->sigActivate
		.connect(std::bind(&CtrlTableBrowser::Activate, this, ph::_1));
	connViewCmd_RefreshClsObjects = mView->sigRefreshClsObjects
		.connect(std::bind(&CtrlTableBrowser::RefreshClsObjects, this, ph::_1));

	connViewCmd_Refresh = mView->sigRefresh
		.connect(std::bind(&CtrlTableBrowser::Refresh, this));
	connViewCmd_Up = mView->sigUp
		.connect(std::bind(&CtrlTableBrowser::Up, this));

	connViewCmd_ShowObjDetail = mView->sigShowDetail
		.connect(std::bind(&CtrlTableBrowser::ShowDetail, this, ph::_1, ph::_2));

	connViewCmd_ClsInsert = mView->sigClsInsert
		.connect(std::bind(&CtrlTableBrowser::ClsInsert, this, ph::_1));
	connViewCmd_ClsDelete = mView->sigClsDelete
		.connect(std::bind(&CtrlTableBrowser::ClsDelete, this, ph::_1));
	connViewCmd_ClsUpdate = mView->sigClsUpdate
		.connect(std::bind(&CtrlTableBrowser::ClsUpdate, this, ph::_1));

	connViewCmd_ObjInsert = mView->sigObjInsert
		.connect(std::bind(&CtrlTableBrowser::ObjInsert, this, ph::_1));
	connViewCmd_ObjDelete = mView->sigObjDelete
		.connect(std::bind(&CtrlTableBrowser::ObjDelete, this, ph::_1, ph::_2));
	connViewCmd_ObjUpdate = mView->sigObjUpdate
		.connect(std::bind(&CtrlTableBrowser::ObjUpdate, this, ph::_1, ph::_2));

	connViewCmd_ToggleGroupByType = mView->sigToggleGroupByType
		.connect(std::bind(&CtrlTableBrowser::ToggleGroupByType, this));
	connViewCmd_ShowFav = mView->sigShowFav
		.connect(std::bind(&CtrlTableBrowser::ShowFav, this, ph::_1));
	connViewCmd_ShowSettings = mView->sigShowSettings
		.connect(std::bind(&CtrlTableBrowser::ShowSettings, this));
	connViewCmd_ShowHelp = mView->sigShowHelp
		.connect(std::bind(&CtrlTableBrowser::ShowHelp, this, ph::_1));

	connViewCmd_Close = mView->sigClosePage
		.connect(std::bind(&CtrlTableBrowser::ClosePage, this));

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
void CtrlTableBrowser::Activate(int64_t cid)
{
	mModel->GetModelBrowser()->DoActivate(cid);
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::RefreshClsObjects(int64_t cid)
{
	mModel->GetModelBrowser()->DoRefreshObjects (cid);
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::Act()
{

}
//---------------------------------------------------------------------------
void CtrlTableBrowser::Move()
{
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::SetInsertType()
{
	mView->SetInsertType();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::SetInsertObj()
{
	mView->SetInsertObj();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::SetDelete()
{
	mView->SetDeleteSelected();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::SetUpdate()
{
	mView->SetUpdateSelected();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::SetShowDetail()
{
	mView->SetShowDetail();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ShowDetail(int64_t oid, int64_t parent_oid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto detail_obj = container->GetObject<rec::ObjInfo>("DefaultDetailObjInfo");
	if (!detail_obj)
		return;

	detail_obj->mObj.mId = oid;
	detail_obj->mObj.mParent.mId = parent_oid;

	auto nb2 = container->GetObject<CtrlNotebook>("CtrlNotebook");
	if (nb2)
	{
		//nb2->MkWindow("CtrlPageDetailObj");
		nb2->MkWindow("CtrlPageDetail");
	}
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ToggleGroupByType()
{
	mModel->GetModelBrowser()->DoToggleGroupByType();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::SetShowFav()
{
	mView->SetShowFav();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ShowFav(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	if (!container)
		return;

	auto ctrlFav = container->GetObject<CtrlFav>("CtrlFav");
	if (!ctrlFav)
		return;

	ctrlFav->EditFav(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ShowSettings()
{

}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ShowHelp(const wxString& index)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto ctrl_help = container->GetObject<CtrlHelp>("CtrlHelp");
	if (!ctrl_help)
		return;

	ctrl_help->Show(index);
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ClosePage()
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb = container->GetObject<CtrlNotebook>("CtrlNotebook");
	if (!nb)
		return;

	auto table_wnd = mView->GetWnd();
	if (!table_wnd)
		return;

	auto page_wnd = table_wnd->GetParent();
	if (!page_wnd)
		return;

	nb->RmWindow(page_wnd);
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ClsInsert(int64_t parent_cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlClsEditor = container->GetObject<CtrlClsEditor>("CtrlClsEditor");
	if (!ctrlClsEditor)
		return;

	ctrlClsEditor->Insert(parent_cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ClsDelete(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlClsEditor = container->GetObject<CtrlClsEditor>("CtrlClsEditor");
	if (!ctrlClsEditor)
		return;

	ctrlClsEditor->Delete(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ClsUpdate(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlClsEditor = container->GetObject<CtrlClsEditor>("CtrlClsEditor");
	if (!ctrlClsEditor)
		return;

	ctrlClsEditor->Update(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ObjInsert(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlObjEditor = container->GetObject<CtrlObjEditor>("CtrlObjEditor");
	if (!ctrlObjEditor)
		return;

	ctrlObjEditor->Insert(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ObjDelete(int64_t oid, int64_t parent_oid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlObjEditor = container->GetObject<CtrlObjEditor>("CtrlObjEditor");
	if (!ctrlObjEditor)
		return;

	ctrlObjEditor->Delete(oid, parent_oid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ObjUpdate(int64_t oid, int64_t parent_oid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlObjEditor = container->GetObject<CtrlObjEditor>("CtrlObjEditor");
	if (!ctrlObjEditor)
		return;

	ctrlObjEditor->Update(oid, parent_oid);
	mModel->GetModelBrowser()->DoRefresh();
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlToolbarBrowser::CtrlToolbarBrowser(
	const std::shared_ptr<IViewToolbarBrowser>& view
	, const std::shared_ptr<ModelPageBrowser>& model
	, CtrlTableBrowser* table_ctrl)
	: CtrlWindowBase(view, model)
	, mTableCtrl(table_ctrl)
{
	namespace ph = std::placeholders;

	connModel_AfterRefreshCls = mModel->GetModelBrowser()->sigAfterRefreshCls
		.connect(std::bind(&IViewToolbarBrowser::SetAfterRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));

	connViewCmd_Refresh = mView->sigRefresh
		.connect(std::bind(&CtrlToolbarBrowser::Refresh, this));
	connViewCmd_Up = mView->sigUp
		.connect(std::bind(&CtrlToolbarBrowser::Up, this));

	connViewCmd_Act = mView->sigAct
		.connect(std::bind(&CtrlToolbarBrowser::Act, this));
	connViewCmd_Move = mView->sigMove
		.connect(std::bind(&CtrlToolbarBrowser::Move, this));
	connViewCmd_ShowDetail = mView->sigShowDetail
		.connect(std::bind(&CtrlToolbarBrowser::ShowDetail, this));

	connViewCmd_AddType = mView->sigInsertType
		.connect(std::bind(&CtrlToolbarBrowser::InsertType, this));
	connViewCmd_AddObject = mView->sigInsertObject
		.connect(std::bind(&CtrlToolbarBrowser::InsertObject, this));
	connViewCmd_DeleteSelected = mView->sigDelete
		.connect(std::bind(&CtrlToolbarBrowser::Delete, this));
	connViewCmd_UpdateSelected = mView->sigUpdate
		.connect(std::bind(&CtrlToolbarBrowser::Update, this));

	connViewCmd_GroupByType = mView->sigGroupByType
		.connect(std::bind(&CtrlToolbarBrowser::GroupByType, this, ph::_1));
	connViewCmd_ToggleGroupByType = mView->sigToggleGroupByType
		.connect(std::bind(&CtrlToolbarBrowser::ToggleGroupByType, this));
	connViewCmd_ShowFav = mView->sigShowFav
		.connect(std::bind(&CtrlToolbarBrowser::ShowFav, this));
	connViewCmd_ShowSettings = mView->sigShowSettings
		.connect(std::bind(&CtrlToolbarBrowser::ShowSettings, this));
	connViewCmd_ShowHelp = mView->sigShowHelp
		.connect(std::bind(&CtrlToolbarBrowser::ShowHelp, this, ph::_1));




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
	mTableCtrl->Act();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Move()
{
	mTableCtrl->Move();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::ShowDetail()
{
	mTableCtrl->SetShowDetail();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::InsertType()
{
	mTableCtrl->SetInsertType();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::InsertObject()
{
	mTableCtrl->SetInsertObj();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Delete()
{
	mTableCtrl->SetDelete();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::Update()
{
	mTableCtrl->SetUpdate();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::GroupByType(bool enable_group_by_type)
{
	mModel->GetModelBrowser()->DoGroupByType(enable_group_by_type);
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::ToggleGroupByType()
{
	mModel->GetModelBrowser()->DoToggleGroupByType();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::ShowFav()
{
	mTableCtrl->SetShowFav();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::ShowSettings()
{

}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::ShowHelp(const wxString& index)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto ctrl_help = container->GetObject<CtrlHelp>("CtrlHelp");
	if (!ctrl_help)
		return;

	ctrl_help->Show(index);
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
	mCtrlTableBrowser = std::make_shared<CtrlTableBrowser>(view->GetViewTableBrowser(), model);
	mCtrlToolbarBrowser = std::make_shared<CtrlToolbarBrowser>(view->GetViewToolbarBrowser(), model, mCtrlTableBrowser.get());
	mCtrlPathBrowser = std::make_shared<CtrlPathBrowser>(view->GetViewPathBrowser(), model);


	namespace ph = std::placeholders;

	connViewCmd_Find = mView->sigFind
		.connect(std::bind(&CtrlPageBrowser::Find, this, ph::_1));

	connModel_AfterRefreshCls = mModel->GetModelBrowser()->sigAfterRefreshCls
		.connect(std::bind(&IViewBrowserPage::SetAfterRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));

}
//---------------------------------------------------------------------------
void CtrlPageBrowser::Find(const wxString& str)
{
	mModel->GetModelBrowser()->DoFind(str);
}
