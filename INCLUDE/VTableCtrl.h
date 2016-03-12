#ifndef __TABLECTRL_H
#define __TABLECTRL_H

#include "VTable.h"
#include "VTableToolBar.h"
#include "TableRowEditor.h"
#include "FilterArrayEditor.h"

namespace wh{
//-----------------------------------------------------------------------------
class	VTableCtrl
{
public:
	VTableCtrl();

	void SetModel(std::shared_ptr<ITable> model);
	void SetAuiMgr(wxAuiManager* auimgr);
	void SetViewToolBar(VTableToolBar* tool_bar);
	void SetViewTable(VTable* table);
	void SetViewFilter(FilterArrayEditor* filter);
	
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

	void OnCmdFind(wxCommandEvent& WXUNUSED(evt));
	void OnCmdLoad(wxCommandEvent& WXUNUSED(evt));
	void OnCmdSave(wxCommandEvent& WXUNUSED(evt));
	void OnCmdInsert(wxCommandEvent& WXUNUSED(evt));
	void OnCmdRemove(wxCommandEvent& WXUNUSED(evt));
	void OnCmdChange(wxCommandEvent& WXUNUSED(evt));
	void OnCmdBackward(wxCommandEvent& WXUNUSED(evt));
	void OnCmdForward(wxCommandEvent& WXUNUSED(evt));
protected:
	wxAuiManager*			mAuiMgr = nullptr;
	std::shared_ptr<ITable> mTableModel = nullptr;
	
	template <class CLASS>
	std::function<void(wxCommandEvent&)> MakeSafeFn
		(void (CLASS::*method)(wxCommandEvent &), int cmd_id, CLASS* cls)
	{
		namespace ph = std::placeholders;

		std::function<void(wxCommandEvent&)>
			fn = std::bind(method, cls, ph::_1);
		auto safe_fn = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		return safe_fn;

	}

	virtual wxAcceleratorTable GetAcceleratorTable()const;

	virtual void BindCmd(wxWindow* wnd);
	virtual void UnbindCmd(wxWindow* wnd);

private:
	wxWindow* mPanel = nullptr;;
	
	
	
	VTable*					mTableView = nullptr;
	VTableToolBar*			mToolBarView = nullptr;
	FilterArrayEditor*		mFilterView = nullptr;
	
	std::shared_ptr<TableRowEditor> mEditor;


};

//-----------------------------------------------------------------------------
}//namespace wh
#endif //__*_H