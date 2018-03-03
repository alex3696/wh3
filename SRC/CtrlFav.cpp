#include "_pch.h"
#include "CtrlFav.h"
#include "CtrlHelp.h"
using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlFav::CtrlFav(
	const std::shared_ptr<IViewFav>& view
	, const std::shared_ptr<ModelFav>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connModel_BeforeRefreshCls = mModel->sigBeforeRefresh
		.connect(std::bind(&IViewFav::SetBeforeUpdate, mView.get(), ph::_1, ph::_2));
	connModel_AfterRefreshCls = mModel->sigAfterRefresh
		.connect(std::bind(&IViewFav::SetAfterUpdate, mView.get(), ph::_1, ph::_2));

	connViewCmd_Refresh = mView->sigRefresh
		.connect(std::bind(&CtrlFav::Refresh, this));

	connViewCmd_AddClsProp = mView->sigAddClsProp
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1, ph::_2));
	connViewCmd_AddObjProp = mView->sigAddObjProp
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1, ph::_2));
	connViewCmd_AddPrevios = mView->sigAddPrevios
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1, ph::_2));
	connViewCmd_AddPeriod = mView->sigAddPeriod
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1, ph::_2));
	connViewCmd_AddNext = mView->sigAddNext
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1, ph::_2));
	connViewCmd_AddLeft = mView->sigAddLeft
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1, ph::_2));

	connViewCmd_RemoveClsProp = mView->sigRemoveClsProp
		.connect(std::bind(&CtrlFav::RemoveClsProp, this, ph::_1, ph::_2));
	connViewCmd_RemoveObjProp = mView->sigRemoveObjProp
		.connect(std::bind(&CtrlFav::RemoveObjProp, this, ph::_1, ph::_2));
	connViewCmd_RemoveActProp = mView->sigRemoveActProp
		.connect(std::bind(&CtrlFav::RemoveActProp, this, ph::_1, ph::_2, ph::_3));

	connViewCmd_ShowHelp = mView->sigShowHelp
		.connect(std::bind(&CtrlFav::ShowHelp, this, ph::_1));

}
//---------------------------------------------------------------------------
void CtrlFav::EditFav(int64_t cid)
{
	mModel->DoSetClsId(cid);
	mView->SetShow();
}
//---------------------------------------------------------------------------
void CtrlFav::Refresh()
{
	mModel->DoRefresh();
}
//---------------------------------------------------------------------------
void CtrlFav::AddClsProp(int64_t clsId, int64_t propId)
{
	mModel->DoAddClsProp(clsId, propId);
}
//---------------------------------------------------------------------------
void CtrlFav::AddObjProp(int64_t clsId, int64_t propId)
{
	mModel->DoAddObjProp(clsId, propId);
}
//---------------------------------------------------------------------------
void CtrlFav::AddPrevios(int64_t clsId, int64_t actId)
{
	mModel->DoAddPrevios(clsId, actId);
}
//---------------------------------------------------------------------------
void CtrlFav::AddPeriod(int64_t clsId, int64_t actId)
{
	mModel->DoAddPeriod(clsId, actId);
}
//---------------------------------------------------------------------------
void CtrlFav::AddNext(int64_t clsId, int64_t actId)
{
	mModel->DoAddNext(clsId, actId);
}
//---------------------------------------------------------------------------
void CtrlFav::AddLeft(int64_t clsId, int64_t actId)
{
	mModel->DoAddLeft(clsId, actId);
}
//---------------------------------------------------------------------------
void CtrlFav::RemoveClsProp(int64_t clsId, int64_t propId)
{
	mModel->DoRemoveClsProp(clsId, propId);
}
//---------------------------------------------------------------------------
void CtrlFav::RemoveObjProp(int64_t clsId, int64_t propId)
{
	mModel->DoRemoveObjProp(clsId, propId);
}
//---------------------------------------------------------------------------
void CtrlFav::RemoveActProp(int64_t clsId, int64_t actId, FavAPropInfo info)
{
	mModel->DoRemoveActProp(clsId, actId, info);
}
//---------------------------------------------------------------------------
void CtrlFav::ShowHelp(const wxString& index)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto ctrl_help = container->GetObject<CtrlHelp>("CtrlHelp");
	if (!ctrl_help)
		return;

	ctrl_help->Show(index);
}
//---------------------------------------------------------------------------