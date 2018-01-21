#include "_pch.h"
#include "CtrlBrowser.h"
#include "CtrlNotebook.h"
#include "CtrlClsEditor.h"
#include "CtrlObjEditor.h"

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
	connViewCmd_RefreshClsObjects = mView->sigRefreshClsObjects
		.connect(std::bind(&CtrlTableBrowser::RefreshClsObjects, this, ph::_1));
	connViewCmd_ShowObjDetail = mView->sigShowObjectDetail
		.connect(std::bind(&CtrlTableBrowser::ShowObjDetail, this, ph::_1, ph::_2));

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


	connModel_BeforeRefreshCls = mModel->GetModelBrowser()->sigBeforeRefreshCls
		.connect(std::bind(&IViewTableBrowser::SetBeforeRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));
	connModel_AfterRefreshCls = mModel->GetModelBrowser()->sigAfterRefreshCls
		.connect(std::bind(&IViewTableBrowser::SetAfterRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));



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
void wh::CtrlTableBrowser::Act()
{

}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::Move()
{
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ShowSelectedObjDetail()
{
	mView->SetShowDetail();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::AddType()
{
	mView->SetInsertType();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::AddObject()
{
	mView->SetInsertObj();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::DeleteSelected()
{
	mView->SetDeleteSelected();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::UpdateSelected()
{
	mView->SetUpdateSelected();
}
//---------------------------------------------------------------------------
void CtrlTableBrowser::ShowObjDetail(int64_t oid, int64_t parent_oid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto detail_obj = container->GetObject<wh::rec::ObjInfo>("DefaultDetailObjInfo");
	if (!detail_obj)
		return;

	detail_obj->mObj.mId = oid;
	detail_obj->mObj.mParent.mId = parent_oid;

	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
	{
		//nb2->MkWindow("CtrlPageDetailObj");
		nb2->MkWindow("CtrlPageDetail");
	}
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ClsInsert(int64_t parent_cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlClsEditor = container->GetObject<wh::CtrlClsEditor>("CtrlClsEditor");
	if (!ctrlClsEditor)
		return;

	ctrlClsEditor->Insert(parent_cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ClsDelete(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlClsEditor = container->GetObject<wh::CtrlClsEditor>("CtrlClsEditor");
	if (!ctrlClsEditor)
		return;

	ctrlClsEditor->Delete(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ClsUpdate(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlClsEditor = container->GetObject<wh::CtrlClsEditor>("CtrlClsEditor");
	if (!ctrlClsEditor)
		return;

	ctrlClsEditor->Update(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ObjInsert(int64_t cid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlObjEditor = container->GetObject<wh::CtrlObjEditor>("CtrlObjEditor");
	if (!ctrlObjEditor)
		return;

	ctrlObjEditor->Insert(cid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ObjDelete(int64_t oid, int64_t parent_oid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlObjEditor = container->GetObject<wh::CtrlObjEditor>("CtrlObjEditor");
	if (!ctrlObjEditor)
		return;

	ctrlObjEditor->Delete(oid, parent_oid);
	mModel->GetModelBrowser()->DoRefresh();
}
//---------------------------------------------------------------------------
void wh::CtrlTableBrowser::ObjUpdate(int64_t oid, int64_t parent_oid)
{
	auto container = whDataMgr::GetInstance()->mContainer;

	auto ctrlObjEditor = container->GetObject<wh::CtrlObjEditor>("CtrlObjEditor");
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


	connModel_AfterRefreshCls = mModel->GetModelBrowser()->sigAfterRefreshCls
		.connect(std::bind(&IViewToolbarBrowser::SetAfterRefreshCls, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4));


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
	mTableCtrl->ShowSelectedObjDetail();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::AddType()
{
	mTableCtrl->AddType();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::AddObject()
{
	mTableCtrl->AddObject();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::DeleteSelected()
{
	mTableCtrl->DeleteSelected();
}
//---------------------------------------------------------------------------
void CtrlToolbarBrowser::UpdateSelected()
{
	mTableCtrl->UpdateSelected();
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
