#include "_pch.h"
#include "CtrlDetail.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPageDetail::CtrlPageDetail(const std::shared_ptr<IViewPageDetail>& view
	, const std::shared_ptr<ModelPageDetail>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	auto& mh = mModel->GetModelHistory();

	mCtrlToolbarHistory
		= std::make_shared<CtrlToolbarHistory>(view->GetViewToolbarHistory(), mh);

	mCtrlTableHistory
		= std::make_shared<CtrlTableHistory>(view->GetViewTableHistory(), mh);

	mCtrlFilterList = std::make_shared<CtrlFilterList>(
		mView->GetViewFilterList()
		, mh->GetModelHistory().GetFilterList());

	mCtrlObjPropList = std::make_shared<CtrlObjPropList>(
		mView->GetViewObjPropList()
		, mh->GetModelHistory().GetObjPropList());


	connModel_SetCfg = mh->sigCfgUpdated.connect
		(std::bind(&IViewPageDetail::SetCfg, mView.get(), ph::_1));

	mView->SetCfg(mh->GetGuiModel());
}
