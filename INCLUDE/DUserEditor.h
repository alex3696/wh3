#ifndef __DUSERDITOR_H
#define __DUSERDITOR_H

#include "BaseOkCancelDialog.h"

#include "MUser2.h"
#include "TPresenter.h"
#include "DUserGroupEditor.h"
#include "VUserPanel.h"

namespace wh{
namespace view{

//-----------------------------------------------------------------------------
/// Редактор для действия
class DUserEditor
	: public wxDialog
	, public ctrlWithResMgr
	, public T_View
{
public:
	typedef MUser2 T_Model;

	DUserEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);
	virtual ~DUserEditor();

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	//std::shared_ptr<T_Model>		mModel;


	typedef TViewCtrlPanel <CtrlTool::AddDel | CtrlTool::Load,
		DUserGroupEditor, false> VGroupsCtrlPanel;


	VUserPanel*			mUserPanel;
	VGroupsCtrlPanel*	mGroupsPanel;

	wxAuiManager			mAuiMgr;
	wxButton*				mBtnOK;
	wxButton*				mBtnCancel;
	wxStdDialogButtonSizer* mBtnSizer;

};
//---------------------------------------------------------------------------





}//namespace view
}//namespace wh
#endif //__BASETABLE_H

