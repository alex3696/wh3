#include "_pch.h"
#include "CtrlObjPropList.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlObjPropList::CtrlObjPropList(const std::shared_ptr<IViewObjPropList>& view
	, const std::shared_ptr<ModelObjPropList>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	connViewCmd_Update = mView->sigUpdate
		.connect(std::bind(&CtrlObjPropList::Update, this));

	connModel_SetPropList = mModel->sigUpdatePropList
		.connect([this](const PropValTable& rt, const IAct* act)
	{
		mView->SetPropList(rt, act);
	});



}

//---------------------------------------------------------------------------
void CtrlObjPropList::Update()
{
	mModel->Update();
}