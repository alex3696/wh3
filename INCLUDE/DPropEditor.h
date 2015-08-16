#ifndef __DPROPEDITOR_H
#define __DPROPEDITOR_H


#include "BaseOkCancelDialog.h"

#include "MProp.h"
#include "TPresenter.h"


namespace wh{
namespace view{


//-----------------------------------------------------------------------------
/// Редактор для свойства
class DPropEditor
	: public DlgBaseOkCancel
	, public T_View
{
public:
	typedef MPropChild T_Model;

	DPropEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetData(const rec::Prop& prop);
	void GetData(rec::Prop& prop)const;

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	void OnChangeModel(const IModel* model, const T_Model::T_Data* data);

	wxPropertyGrid*		mPropGrid = nullptr;
	//wxAuiToolBar*		mToolBar = nullptr;
	//wxAuiToolBarItem*	mLoadTool = nullptr;
	//wxAuiToolBarItem*	mSaveTool = nullptr;


	std::shared_ptr<T_Model>		mModel;
	sig::scoped_connection				mChangeConnection;
	//boost::signals2::scoped_connection	slotOnChange;

};
//---------------------------------------------------------------------------






//-----------------------------------------------------------------------------
}//namespace view
}//namespace wh
#endif //__BASETABLE_H

