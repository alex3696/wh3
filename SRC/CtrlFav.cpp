#include "_pch.h"
#include "CtrlFav.h"
#include "CtrlHelp.h"

#include "DClsPropEditor.h"
#include "DClsActEditor.h"
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
		.connect(std::bind(&CtrlFav::AddClsProp, this, ph::_1));
	connViewCmd_AddObjProp = mView->sigAddObjProp
		.connect(std::bind(&CtrlFav::AddObjProp, this, ph::_1));
	connViewCmd_AddActProp = mView->sigAddActProp
		.connect(std::bind(&CtrlFav::AddActProp, this, ph::_1, ph::_2));

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
void CtrlFav::AddClsProp(int64_t clsId)
{
	std::shared_ptr<MPropArray> mPropArray;
	mPropArray.reset(new MPropArray);
	mPropArray->Load();

	view::DPropSelector dlg(nullptr);
	dlg.SetSrcVec(mPropArray);
	if (wxID_OK == dlg.ShowModal())
	{
		wxDataViewItemArray selected;
		dlg.GetSelections(selected);
		if (!selected.empty())
		{
			unsigned int row = dlg.GetRow(selected[0]);
			auto childModel = mPropArray->GetChild(row);
			auto propModel = std::dynamic_pointer_cast<MPropChild>(childModel);
			const auto& propData = propModel->GetData();

			int64_t propId = propData.mId;
			mModel->DoAddClsProp(clsId, propId);
		}
	}
}
//---------------------------------------------------------------------------
void CtrlFav::AddObjProp(int64_t clsId)
{
	std::shared_ptr<MPropArray> mPropArray;
	mPropArray.reset(new MPropArray);
	mPropArray->Load();

	view::DPropSelector dlg(nullptr);
	dlg.SetSrcVec(mPropArray);
	if (wxID_OK == dlg.ShowModal())
	{
		wxDataViewItemArray selected;
		dlg.GetSelections(selected);
		if (!selected.empty())
		{
			unsigned int row = dlg.GetRow(selected[0]);
			auto childModel = mPropArray->GetChild(row);
			auto propModel = std::dynamic_pointer_cast<MPropChild>(childModel);
			const auto& propData = propModel->GetData();

			int64_t propId = propData.mId;
			mModel->DoAddObjProp(clsId, propId);
		}
	}
}
//---------------------------------------------------------------------------
void CtrlFav::AddActProp(int64_t clsId, FavAPropInfo info)
{
	auto mActArray = std::make_shared<MActArray>();
	mActArray->Load();

	view::DActSelector dlg(nullptr);
	dlg.SetSrcVec(mActArray);
	if (wxID_OK == dlg.ShowModal())
	{
		wxDataViewItemArray selected;
		dlg.GetSelections(selected);
		if (!selected.empty())
		{
			unsigned int row = dlg.GetRow(selected[0]);
			auto childModel = mActArray->GetChild(row);
			auto actModel = std::dynamic_pointer_cast<MAct>(childModel);
			const auto& actData = actModel->GetData();
			long val = 0;
			if (actData.mID.ToLong(&val))
			{
				int64_t actId = val;
				mModel->DoAddActProp(clsId, actId, info);
			}
		}
	}
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