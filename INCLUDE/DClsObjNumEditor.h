#ifndef __DCLSOBJNUMEDITOR_H
#define __DCLSOBJNUMEDITOR_H

#include "MClsObjNum.h"
#include "MClsTree.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для свойства действия
class DClsObjNumEditor
	: public view::DlgBaseOkCancel
	, public T_View
{
public:
	DClsObjNumEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	void GetData(rec::ClsObjNum& rec) const;
	void SetData(const rec::ClsObjNum& rec);

	virtual void OnChangeModel(const IModel& model) override;

	wxPropertyGrid*					mPropGrid;

	/// Модель свойства класса
	std::shared_ptr<MClsObjNum>		mModel; 
	scoped_connection				mChangeConnection;
};
//-----------------------------------------------------------------------------	


}//namespace view{
}//namespace wh{
#endif // __****_H