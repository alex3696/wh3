#ifndef __VCLSDAEDITORTAPANEL_H
#define __VCLSDATAPANEL_H


#include "BaseOkCancelDialog.h"

#include "TPresenter.h"
#include "MTypeNode.h"


namespace wh{
namespace view{


//-----------------------------------------------------------------------------
/// �������� ��� ��������
class VClsDataEditorPanel
	: public wxPanel
	, public ctrlWithResMgr
	, public T_View
{
public:
	typedef object_catalog::MTypeItem T_Model;

	VClsDataEditorPanel(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetData(const rec::Cls& prop);
	void GetData(rec::Cls& prop)const;

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
private:
	void OnChangeModel(const IModel* model, const T_Model::T_Data* data);
	void OnClassTypeChange(wxPropertyGridEvent& evt);

	wxPropertyGrid*		mPropGrid = nullptr;

	std::shared_ptr<T_Model>		mModel;
	sig::scoped_connection			mChangeConnection;

};
//---------------------------------------------------------------------------






//-----------------------------------------------------------------------------
}//namespace view
}//namespace wh
#endif //__BASETABLE_H

