#include "_pch.h"
#include "CtrlUndo.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlUndoWindow::CtrlUndoWindow(const std::shared_ptr<IViewUndoWindow>& view
	, const std::shared_ptr<ModelUndoWindow>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connViewCmd_ExecuteUndo = mView->sigExecuteUndo
		.connect(std::bind(&CtrlUndoWindow::ExecuteUndo, this));
	connViewCmd_Load = mView->sigLoad
		.connect(std::bind(&CtrlUndoWindow::Load, this));

	connModel_HistoryRecordLoaded = mModel->mModelUndo->sigHistoryRecordLoaded
		.connect([this](const ModelHistoryRecord& rt)
		{
			mView->SetHistoryRecord(rt);
		});
}
//---------------------------------------------------------------------------
void CtrlUndoWindow::ExecuteUndo()
{
	mModel->mModelUndo->ExecuteUndo();
}
//---------------------------------------------------------------------------
void CtrlUndoWindow::Load()
{
	mModel->mModelUndo->Load();
}
