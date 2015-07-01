#ifndef __DACTEDITOR_H
#define __DACTEDITOR_H

#include "BaseOkCancelDialog.h"

#include "MAct.h"
#include "TPresenter.h"
#include "DActPropEditor.h"
#include "VActPanel.h"

namespace wh{
namespace view{

//-----------------------------------------------------------------------------
/// Редактор для действия
class DActEditor
	: public wxDialog
	, public ctrlWithResMgr
	, public T_View
{
public:
	typedef MAct T_Model;

	DActEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);
	virtual ~DActEditor();

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	//std::shared_ptr<T_Model>		mModel;


	typedef TViewCtrlPanel <CtrlTool::AddDel | CtrlTool::Load,
		DActPropEditor, false> VActPropCtrlPanel;


	VActPanel*			mActPanel;
	VActPropCtrlPanel*	mPropsPanel;

	wxAuiManager			mAuiMgr;
	wxButton*				mBtnOK;
	wxButton*				mBtnCancel;
	wxStdDialogButtonSizer* mBtnSizer;

};
//---------------------------------------------------------------------------





}//namespace view
}//namespace wh
#endif //__BASETABLE_H

