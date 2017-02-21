#include "_pch.h"
#include "CtrlNotebook.h"

using namespace wh;
//-----------------------------------------------------------------------------
CtrlNotebook::CtrlNotebook(std::shared_ptr<IViewNotebook> view
										, std::shared_ptr<ModelNotebook> model)
	: mView(view)
	, mModel(model)
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
//virtual override
std::shared_ptr<IViewWindow> CtrlNotebook::GetView()const
{ 
	return mView; 
}
//-----------------------------------------------------------------------------
//virtual override
void CtrlNotebook::UpdateTitle()
{

}
//-----------------------------------------------------------------------------
//virtual override
void CtrlNotebook::Show()
{

}
//-----------------------------------------------------------------------------
//virtual override
void CtrlNotebook::Close()
{

}
//-----------------------------------------------------------------------------
//virtual override
void CtrlNotebook::Load(const boost::property_tree::ptree& val)
{
	wxWindowUpdateLocker lock(mView->GetWnd());
	mModel->Load(val);
}
//-----------------------------------------------------------------------------
//virtual override
void CtrlNotebook::Save(boost::property_tree::ptree& val)
{
	mModel->Save(val);
};
