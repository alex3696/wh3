#ifndef __DLGSELECTCLS_CTRLPNL_H
#define __DLGSELECTCLS_CTRLPNL_H

#include "VObjCatalogCtrl.h"

namespace wh{

class CatDlg:
	public wxDialog
	,public ctrlWithResMgr
{
protected:
	wxButton*				m_btnOK;
	wxButton*				m_btnCancel;
	wxStdDialogButtonSizer*	m_sdbSizer;
	view::VObjCatalogCtrl*	mMainPanel;
	std::shared_ptr<object_catalog::MObjCatalog>	mCatalog;
	bool					mIsTargetObj = true;

	void OnActivated(wxDataViewEvent& evt);
	void OnSelect(wxDataViewEvent& evt);
public:	
	CatDlg(wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxString& title=wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 500,400 ),//wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,
				const wxString& name = wxDialogNameStr);


	template <class T>
	void SetModel(std::shared_ptr<T> model)
	{
		SetModel(std::dynamic_pointer_cast<IModel>(model));
	}

	void SetModel(std::shared_ptr<IModel> model);

	void SetTargetObj(bool isTargetObj)	{ mIsTargetObj = isTargetObj; }

	bool GetSelectedCls(wh::rec::Cls& cls);
	bool GetSelectedObj(wh::rec::ObjInfo& obj);
};


}//namespace wh
#endif //__*_H