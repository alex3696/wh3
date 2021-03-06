#include "_pch.h"
#include "CtrlNotebook.h"

using namespace wh;
//-----------------------------------------------------------------------------
CtrlNotebook::CtrlNotebook(const std::shared_ptr<IViewNotebook>& view
	, const  std::shared_ptr<ModelNotebook>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connViewMkWindow = mView->sigMkWindow
		.connect(std::bind(&ModelNotebook::MkWindow, mModel.get(), ph::_1));
	connViewRmWindow = mView->sigRmWindow
		.connect(std::bind(&ModelNotebook::RmWindow, mModel.get(), ph::_1));
	connViewShowWindow = mView->sigShowWindow
		.connect(std::bind(&ModelNotebook::ShowWindow, mModel.get(), ph::_1));

	
	connModelAfterMkWindow = mModel->sigAfterMkWindow
		.connect(std::bind(&CtrlNotebook::OnSig_MkChild, this, ph::_1));
	//connCtrlAfterMkWindow = mModel->sigAfterMkWindow.connect(sig?????);

	connModelBeforeRmWindow = mModel->sigBeforeRmWindow
		.connect(std::bind(&CtrlNotebook::OnSig_RmChild, this, ph::_1));
	//connCtrlBeforeRmWindow = mModel->sigBeforeRmWindow.connect(sigClose);

	connModelShowWindow = mModel->sigShowWindow
		.connect(std::bind(&CtrlNotebook::OnSig_ShowChild, this, ph::_1));
	//connCtrlShowWindow = mModel->sigShowWindow.connect(sigShow);

	connModelAfterChWindow = mModel->sigAfterChWindow
		.connect(std::bind(&CtrlNotebook::OnSig_ChTitleChild, this, ph::_1, ph::_2, ph::_3));
	//connCtrlAfterChWindow = mModel->sigAfterChWindow.connect(sigUpdateTitle);


}
//-----------------------------------------------------------------------------
void CtrlNotebook::OnSig_MkChild(ICtrlWindow* child_ctrl)
{
	mView->MkPage(child_ctrl->GetView()->GetWnd());
}
//-----------------------------------------------------------------------------
void CtrlNotebook::OnSig_RmChild(ICtrlWindow* child_ctrl)
{
	mView->RmPage(child_ctrl->GetView()->GetWnd());
}
//-----------------------------------------------------------------------------
void CtrlNotebook::OnSig_ShowChild(ICtrlWindow* child_ctrl)
{
	mView->ShowPage(child_ctrl->GetView()->GetWnd());
}
//-----------------------------------------------------------------------------
void CtrlNotebook::OnSig_ChTitleChild(ICtrlWindow* child_ctrl, const wxString& title, const wxIcon& ico)
{
	mView->ChPage(child_ctrl->GetView()->GetWnd(), title, ico);
	//this->sigUpdateTitle(child_ctrl, title, ico);
}
//-----------------------------------------------------------------------------
void CtrlNotebook::MkWindow(const wxString& wi)
{
	wxWindowUpdateLocker lock(mView->GetWnd());
	mModel->MkWindow(wi);
}
//-----------------------------------------------------------------------------
void CtrlNotebook::RmWindow(wxWindow* wnd)
{
	wxWindowUpdateLocker lock(mView->GetWnd());
	mModel->RmWindow(wnd);
}
//-----------------------------------------------------------------------------
void CtrlNotebook::CloseAllPages()
{
	mModel->RmAll();
}
//-----------------------------------------------------------------------------
//virtual override
//void CtrlNotebook::Load(const boost::property_tree::wptree& val)
//{
	//wxWindowUpdateLocker lock(mView->GetWnd());
	//CtrlWindowBase::Load(val);
//}
//-----------------------------------------------------------------------------
////virtual override
//void CtrlNotebook::Save(boost::property_tree::wptree& val)
//{
//	CtrlWindowBase::Save(val);
//};
//-----------------------------------------------------------------------------
////virtual override
void CtrlNotebook::RmView()
{
	mModel->RmAll();
	CtrlWindowBase::RmView();
}
