#ifndef __TABLETOOLBAR_H
#define __TABLETOOLBAR_H

#include "BaseTable.h"
#include "MTable.h"

namespace wh{
//-----------------------------------------------------------------------------

class VTableToolBar:
	public wxAuiToolBar
	,public ctrlWithResMgr

{
public:
	VTableToolBar(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxAUI_TB_DEFAULT_STYLE);


	void SetModel(std::shared_ptr<ITable> model);
	void BuildToolBar();

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

private:
	wxStaticText*			mPageLabel = nullptr;

	bool					mEnableFilter = true;
	bool					mEnableLoad = true;
	bool					mEnableSave = true;
	bool					mEnableInsert = true;
	bool					mEnableRemove = true;
	bool					mEnableChange = true;

	void OnTableChange(const IModel& vec);

	sig::scoped_connection	mConnAfterInsert;
	sig::scoped_connection	mConnAfterRemove;
	sig::scoped_connection	mConnAfterChange;
	void OnAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnAfterChange(const IModel& vec, const std::vector<unsigned int>& itemVec);

	//sig::scoped_connection	mConnChangePageLimit;
	//sig::scoped_connection	mConnChangePageNo;
	//void OnChangePageLimit(const IModel& model);
	//void OnChangePageNo(const IModel& model);
};

//-----------------------------------------------------------------------------
}//namespace wh
#endif //__*_H