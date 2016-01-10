#ifndef __DLGSELECTCLS_CTRLPNL_H
#define __DLGSELECTCLS_CTRLPNL_H

#include "VObjCatalogCtrl.h"

namespace wh{

class CatDlg:
	public wxDialog
	,public ctrlWithResMgr
{
public:
	typedef std::function<bool(const wh::rec::Cls*,const wh::rec::Obj*)> TargetValidator;

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

	void SetTargetValidator(const TargetValidator& tv)	{ mTargetValidator = tv; }
	
	bool GetSelectedCls(wh::rec::Cls& cls);
	bool GetSelectedObj(wh::rec::ObjInfo& obj);

protected:
	wxButton*				m_btnOK;
	wxButton*				m_btnCancel;
	wxStdDialogButtonSizer*	m_sdbSizer;
	view::VObjCatalogCtrl*	mMainPanel;
	std::shared_ptr<object_catalog::MObjCatalog>	mCatalog;
	TargetValidator			mTargetValidator;


	void OnActivated(wxDataViewEvent& evt);
	void OnSelect(wxDataViewEvent& evt);

};


}//namespace wh
#endif //__*_H