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
		.connect(std::bind(&IViewNotebook::MkPage, mView.get(), ph::_1));
	connModelAfterChWindow = mModel->sigAfterChWindow
		.connect(std::bind(&IViewNotebook::ChPage, mView.get(), ph::_1, ph::_2, ph::_3));
	connModelBeforeRmWindow = mModel->sigBeforeRmWindow
		.connect(std::bind(&IViewNotebook::RmPage, mView.get(), ph::_1));
	connModelShowWindow = mModel->sigShowWindow
		.connect(std::bind(&IViewNotebook::ShowPage, mView.get(), ph::_1));
}
//-----------------------------------------------------------------------------
void CtrlNotebook::MkWindow(const wxString& wi)
{
	mModel->MkWindow(wi);
}
//-----------------------------------------------------------------------------
void CtrlNotebook::RmWindow(wxWindow* wnd)
{
	mModel->RmWindow(wnd);
}
