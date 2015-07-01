#ifndef __DCLSFAVORITEACTPROPEDITOR_H
#define __DCLSFAVORITEACTPROPEDITOR_H

#include "MClsActProp.h"
#include "MProp.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для избранных свойств класса
class DClsFavoriteActPropEditor
	: public DlgBaseOkCancel
	, public T_View
{
public:

	DClsFavoriteActPropEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	~DClsFavoriteActPropEditor();

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	virtual void OnChangeModel(const IModel& model) override;

	typedef view::TViewTable	PropTable;

	std::shared_ptr<MClsAllActPropArray>	mPropArray;
	PropTable*								mPropArrayView;
		
	std::shared_ptr<MClsActProp>		mModel; 
	scoped_connection					mChangeConnection;
};
//-----------------------------------------------------------------------------	


}//namespace view{
}//namespace wh{
#endif // __****_H