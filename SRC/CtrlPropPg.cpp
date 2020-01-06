#include "_pch.h"
#include "CtrlPropPg.h"

using namespace wh;
//---------------------------------------------------------------------------
CtrlPropPg::CtrlPropPg(const std::shared_ptr<ViewPropPg>& view
	, const std::shared_ptr<ModelPropPg>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	if (!view)
		return;

	connModel_BeforeRefresh = mModel->sigBeforeRefresh
		.connect(std::bind(&T_View::SetBeforeRefresh, mView.get(), ph::_1));
	connModel_AfterRefresh = mModel->sigAfterRefresh
		.connect(std::bind(&T_View::SetAfterRefresh, mView.get(), ph::_1));




}

//---------------------------------------------------------------------------
void CtrlPropPg::Update()
{
	//mModel->Update();
}