#ifndef __VUSERANEL_H
#define __VUSERANEL_H


#include "BaseOkCancelDialog.h"

#include "MUser2.h"
#include "TPresenter.h"


namespace wh{
namespace view{


//-----------------------------------------------------------------------------
/// Редактор для свойства
class VUserPanel
	: public wxPanel
	, public ctrlWithResMgr
	, public T_View
{
public:
	typedef MUser2 T_Model;

	VUserPanel(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetData(const rec::User& prop);
	void GetData(rec::User& prop)const;

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
private:
	virtual void OnChangeModel(const IModel& model) override;

	wxPropertyGrid*		mPropGrid = nullptr;

	std::shared_ptr<T_Model>		mModel;
	sig::scoped_connection				mChangeConnection;

};
//---------------------------------------------------------------------------






//-----------------------------------------------------------------------------
}//namespace view
}//namespace wh
#endif //__BASETABLE_H

