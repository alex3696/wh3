#ifndef __DCLSMOVEEDITOR_H
#define __DCLSMOVEEDITOR_H

#include "MClsMove.h"
#include "MGroup.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для свойства действия
class DClsMoveEditor
	: public view::DlgBaseOkCancel
	, public T_View
{
public:
	DClsMoveEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetModel(std::shared_ptr<IModel>& model)override;
	void UpdateModel()const override;
	int  ShowModal() override;
private:
	void GetData(rec::ClsSlotAccess& rec) const;
	void SetData(const rec::ClsSlotAccess& rec);

	void OnChangeModel(const IModel* model, const MClsMove::T_Data* data);

	wxPropertyGrid*					mPropGrid;

	// каталог групп
	std::shared_ptr<MGroupArray>	mGroupArray;


	/// Модель свойства класса
	std::shared_ptr<MClsMove>		mModel;
	sig::scoped_connection			mChangeConnection;
};
//-----------------------------------------------------------------------------	


}//namespace view{
}//namespace wh{
#endif // __****_H

