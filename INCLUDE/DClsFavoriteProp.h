#ifndef __DCLSFAVORITEPROP_H
#define __DCLSFAVORITEPROP_H


#include "BaseOkCancelDialog.h"

#include "MClsFavoriteActProp.h"
#include "TPresenter.h"
#include "DClsFavoriteActPropEditor.h"

namespace wh{
namespace view{


//-----------------------------------------------------------------------------
/// Редактор для свойства
class DClsFavoriteProp
	: public wxDialog
	, public ctrlWithResMgr
	, public T_View
{
public:
	DClsFavoriteProp(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);
	virtual ~DClsFavoriteProp();

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	//std::shared_ptr<T_Model>		mModel;


	typedef TViewCtrlPanel <CtrlTool::AddDel | CtrlTool::Load,
		DClsFavoriteActPropEditor, false> VClsFavoritePropCtrlPanel;

	VClsFavoritePropCtrlPanel*	mPropsPanel;

	wxAuiManager			mAuiMgr;
	wxButton*				mBtnOK;
	wxButton*				mBtnCancel;
	wxStdDialogButtonSizer* mBtnSizer;
};
//---------------------------------------------------------------------------






//-----------------------------------------------------------------------------
}//namespace view
}//namespace wh
#endif //__BASETABLE_H

