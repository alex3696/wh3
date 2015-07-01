#ifndef __DUSERGROUPEDITOR_H
#define __DUSERGROUPEDITOR_H

#include "MUser2.h"
#include "MGroup.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для свойства действия
class DUserGroupEditor
	: public view::DlgBaseOkCancel
	, public T_View
{
public:
	typedef MUserGroup T_Model;

	DUserGroupEditor(wxWindow* parent = nullptr,
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

	typedef view::TViewTable	GroupTable;

	std::shared_ptr<MGroupArray>	mGroupArray;
	GroupTable*						mGroupArrayView;

	std::shared_ptr<T_Model>		mModel; // std::shared_ptr<MUserGroup>
	scoped_connection				mChangeConnection;
};
//-----------------------------------------------------------------------------	


}//namespace view{
}//namespace wh{
#endif // __****_H