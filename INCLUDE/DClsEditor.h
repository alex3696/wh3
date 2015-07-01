#ifndef __DCLSEDITOR_H
#define __DCLSEDITOR_H

#include "BaseOkCancelDialog.h"

#include "MClsTree.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"
#include "VClsDataEditorPanel.h"

#include "DClsPropEditor.h"

#include "DClsActEditor.h"
#include "DClsMoveEditor.h"

#include "DClsObjNumEditor.h"
#include "DClsObjQtyEditor.h"

namespace wh{
namespace view{

//-----------------------------------------------------------------------------
/// Редактор для действия
class DClsEditor
	: public wxDialog
	, public ctrlWithResMgr
	, public T_View
{
public:
	DClsEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);
	~DClsEditor();

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
	virtual void OnChangeModel(const IModel& model)override;
private:
	std::shared_ptr<MClsNode>	mClsNode;
	scoped_connection			mChangeConnection;

	typedef TViewCtrlPanel <CtrlTool::All, DClsPropEditor, false>	VClsPropPanel;
	typedef TViewCtrlPanel <CtrlTool::All, DClsActEditor, false>	VClsActPanel;
	typedef TViewCtrlPanel <CtrlTool::All, DClsMoveEditor, false>	VClsMovePanel;
	typedef TViewCtrlPanel <CtrlTool::All, DClsObjQtyEditor, false> VClsObjQtyPanel;
	typedef TViewCtrlPanel <CtrlTool::All, DClsObjNumEditor, false> VClsObjNumPanel;

	VClsDataEditorPanel*	mClsPanel;
	VClsPropPanel*			mClsPropPanel;
	VClsActPanel*			mClsActPanel;
	VClsMovePanel*			mClsMovePanel;
	VClsObjNumPanel*		mClsObjNumPanel;
	VClsObjQtyPanel*		mClsObjQtyPanel;

	wxAuiNotebook*			mNotebook;
	wxAuiManager			mAuiMgr;
	wxButton*				mBtnOK;
	wxButton*				mBtnCancel;
	wxStdDialogButtonSizer* mBtnSizer;

};
//---------------------------------------------------------------------------




}//namespace view
}//namespace wh
#endif //__BASETABLE_H

