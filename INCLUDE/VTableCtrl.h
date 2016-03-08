#ifndef __TABLECTRL_H
#define __TABLECTRL_H

#include "VTable.h"
#include "TableRowEditor.h"

namespace wh{
//-----------------------------------------------------------------------------
class	VTableCtrl
{
public:
	VTableCtrl();
	void SetTableViewModel(std::shared_ptr<ITable> model, VTable* tview, wxWindow* panel = nullptr);

	void SetEditor(std::shared_ptr<TableRowEditor> model);
	std::shared_ptr<TableRowEditor> GetEditor();

	std::function<void(wxCommandEvent&)> fnOnCmdFilter;
	std::function<void(wxCommandEvent&)> fnOnCmdLoad;
	std::function<void(wxCommandEvent&)> fnOnCmdSave;
	std::function<void(wxCommandEvent&)> fnOnCmdInsert;
	std::function<void(wxCommandEvent&)> fnOnCmdRemove;
	std::function<void(wxCommandEvent&)> fnOnCmdEdit;
	std::function<void(wxCommandEvent&)> fnOnCmdBackward;
	std::function<void(wxCommandEvent&)> fnOnCmdForward;

	void OnCmdLoad(wxCommandEvent& WXUNUSED(evt));
	void OnCmdSave(wxCommandEvent& WXUNUSED(evt));
	void OnCmdInsert(wxCommandEvent& WXUNUSED(evt));
	void OnCmdRemove(wxCommandEvent& WXUNUSED(evt));
	void OnCmdChange(wxCommandEvent& WXUNUSED(evt));
	void OnCmdFind(wxCommandEvent& WXUNUSED(evt));
	void OnCmdBackward(wxCommandEvent& WXUNUSED(evt));
	void OnCmdForward(wxCommandEvent& WXUNUSED(evt));
private:
	wxWindow* mPanel = nullptr;;
	VTable* mTableView = nullptr;
	std::shared_ptr<ITable> mTableModel = nullptr;
	std::shared_ptr<TableRowEditor> mEditor;

	std::function<void(wxCommandEvent&)> MakeSafeFn
		(void (VTableCtrl::*method)(wxCommandEvent &), int cmd_id);



};

//-----------------------------------------------------------------------------
}//namespace wh
#endif //__*_H