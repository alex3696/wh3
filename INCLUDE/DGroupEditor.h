#ifndef __DGROUPEDITOR_H
#define __DGROUPEDITOR_H


#include "BaseOkCancelDialog.h"

#include "MGroup.h"
#include "TPresenter.h"


namespace wh{
namespace view{


//-----------------------------------------------------------------------------
/// Редактор для свойства
class DGroupEditor
	: public DlgBaseOkCancel
	, public T_View
{
public:
	typedef MGroup T_Model;

	DGroupEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetData(const rec::Role& prop);
	void GetData(rec::Role& prop)const;

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	virtual void OnChangeModel(const IModel& model) override;

	wxPropertyGrid*		mPropGrid = nullptr;
	//wxAuiToolBar*		mToolBar = nullptr;
	//wxAuiToolBarItem*	mLoadTool = nullptr;
	//wxAuiToolBarItem*	mSaveTool = nullptr;


	std::shared_ptr<T_Model>		mModel;
	scoped_connection				mChangeConnection;
	//boost::signals2::scoped_connection	slotOnChange;

};
//---------------------------------------------------------------------------






//-----------------------------------------------------------------------------
}//namespace view
}//namespace wh
#endif //__BASETABLE_H

