#include "_pch.h"
#include "CtrlFav.h"
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

	//connViewCmd_Find = mView->sigFind
	//	.connect(std::bind(&CtrlPageBrowser::Find, this, ph::_1));


}
//---------------------------------------------------------------------------
void CtrlFav::EditFav(int64_t cid)
{
	mModel->DoShowFavEditor(cid);
}
//---------------------------------------------------------------------------
