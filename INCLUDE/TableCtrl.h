#ifndef __TABLECTRL_H
#define __TABLECTRL_H

#include "BaseTable.h"
#include "TableRowEditor.h"
#include "VTable.h"
#include "FilterArrayEditor.h"

namespace wh{
//-----------------------------------------------------------------------------
class TableCtrl
	: public wxPanel
	, public ctrlWithResMgr
{
public:
	TableCtrl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~TableCtrl();

	void SetModel(std::shared_ptr<ITable> model);
	void SetEditor(std::shared_ptr<TableRowEditor> model);

	void SetEnableFilter(bool enable = true) { mEnableFilter = enable; }
	void SetEnableLoad(bool enable = true) { mEnableLoad = enable; }
	void SetEnableSave(bool enable = true) { mEnableSave = enable; }
	void SetEnableInsert(bool enable = true) { mEnableInsert = enable; }
	void SetEnableRemove(bool enable = true) { mEnableRemove = enable; }
	void SetEnableChange(bool enable = true) { mEnableChange = enable; }

	bool IsEnableFilter()const { return mEnableFilter; }
	bool IsEnableLoad()const { return mEnableLoad; }
	bool IsEnableSave()const { return mEnableSave; }
	bool IsEnableInsert()const { return mEnableInsert; }
	bool IsEnableRemove()const { return mEnableRemove; }
	bool IsEnableChange()const { return mEnableChange; }

	void GetSelected(std::vector<unsigned int>& selected);
protected:
	wxAuiManager			mAuiMgr;
	wxAuiToolBar*			mToolBar = nullptr;
	wxAuiToolBar*			mPageToolBar = nullptr;
	wxStaticText*			mPageLabel = nullptr;
	VTable*					mTableView = nullptr;
	wxMenu					mContextMenu;
	
	FilterArrayEditor*				mFilterEditor;
	std::shared_ptr<ITable>			mMTable;
	
	std::shared_ptr<TableRowEditor> mEditor;
	
	void OnSelectChange(wxDataViewEvent &event);
	void OnContextMenu(wxDataViewEvent &event);
	void OnColumnHeaderlClick(wxDataViewEvent &event);

	void BuildToolBar();
	void BuildPopupMenu();
private:
	bool					mEnableFilter = true;
	bool					mEnableLoad = true;
	bool					mEnableSave = true;
	bool					mEnableInsert = true;
	bool					mEnableRemove = true;
	bool					mEnableChange = true;
	
	void OnCmdLoad(wxCommandEvent& WXUNUSED(evt));
	void OnCmdSave(wxCommandEvent& WXUNUSED(evt));
	void OnCmdInsert(wxCommandEvent& WXUNUSED(evt));
	void OnCmdRemove(wxCommandEvent& WXUNUSED(evt));
	void OnCmdChange(wxCommandEvent& WXUNUSED(evt));
	void OnCmdFind(wxCommandEvent& WXUNUSED(evt));
	void OnCmdBackward(wxCommandEvent& WXUNUSED(evt));
	void OnCmdForward(wxCommandEvent& WXUNUSED(evt));


	void OnTableChangeState(const IModel& vec);

	sig::scoped_connection	mConnAfterInsert;
	sig::scoped_connection	mConnAfterRemove;
	sig::scoped_connection	mConnAfterChange;
	void OnAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnAfterChange(const IModel& vec, const std::vector<unsigned int>& itemVec);
};
//-----------------------------------------------------------------------------


}//namespace wh
#endif //__*_H