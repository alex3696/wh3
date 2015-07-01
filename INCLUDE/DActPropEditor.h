#ifndef __DACTPROPEDITOR_H
#define __DACTPROPEDITOR_H

#include "MAct.h"
#include "MProp.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для свойства действия
class DActPropEditor
	: public view::DlgBaseOkCancel
	, public T_View
{
public:
	typedef MActProp T_Model;

	DActPropEditor(wxWindow* parent = nullptr,
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
	virtual void OnChangeModel(const IModel& model) override;

	typedef view::TViewTable	PropTable;

	std::shared_ptr<MPropArray>		mPropArray;
	PropTable*						mPropArrayView;
		
	std::shared_ptr<T_Model>		mModel; // std::shared_ptr<MActProp>
	scoped_connection				mChangeConnection;
};
//-----------------------------------------------------------------------------	


}//namespace view{
}//namespace wh{
#endif // __****_H