#ifndef __VACTPANEL_H
#define __VACTPANEL_H


#include "BaseOkCancelDialog.h"

#include "MAct.h"
#include "TPresenter.h"


namespace wh{
namespace view{


//-----------------------------------------------------------------------------
/// �������� ��� ��������
class VActPanel
	: public wxPanel
	, public ctrlWithResMgr
	, public T_View
{
public:
	typedef MAct T_Model;

	VActPanel(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetData(const rec::Act& prop);
	void GetData(rec::Act& prop)const;

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
private:
	void OnChangeModel(const IModel* model, const MAct::T_Data* data);

	wxPropertyGrid*		mPropGrid = nullptr;

	std::shared_ptr<T_Model>		mModel;
	sig::scoped_connection			mChangeConnection;

};
//---------------------------------------------------------------------------






//-----------------------------------------------------------------------------
}//namespace view
}//namespace wh
#endif //__BASETABLE_H

