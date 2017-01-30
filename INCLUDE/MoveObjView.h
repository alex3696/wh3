#ifndef __MOVEOBJVIEW_H
#define __MOVEOBJVIEW_H

#include "IMoveObjView.h"

namespace wh{

class Node
	:public boost::noncopyable
{
public:
	Node(const Node* parent, int tid = 0, void* val = nullptr)
		:mParent(parent), mTypeId(tid), mVal(val)
	{}

	const Node*			mParent;
	int					mTypeId;
	void*				mVal;
	
	std::vector< std::shared_ptr<Node> >	mChilds;
};


class MoveObjView
	: public wxDialog
	, public IMoveObjView
	, public ctrlWithResMgr
{
	wxButton*				mbtnOK;
	wxButton*				mbtnCancel;
	wxStdDialogButtonSizer*	msdbSizer;
	wxStaticText*			mLblMovableObj;
	wxStaticText*			mLblDstObj;

	wxSpinCtrl* mqtySpin;
	wxTextCtrl* mqtyCtrl;
	
	wxTextCtrl*		mFindCtrl;
	wxAuiToolBar*	mToolBar;
	wxDataViewCtrl* mTree;
	
	void OnClose(wxCloseEvent& evt);
	void OnCancel(wxCommandEvent& evt = wxCommandEvent());
	void OnOk(wxCommandEvent& evt = wxCommandEvent());
	void OnActivated(wxDataViewEvent &evt = wxDataViewEvent());
public:

	MoveObjView(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(500, 400),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

private:
	void BuildTree();
	void BuildToolBar();

	void ExpandAll(const Node& node); 
	void ExpandAll();
	void OnClickSearchBtn(wxCommandEvent& event);

	virtual void ShowDialog() override { this->ShowModal();  };
	virtual void UpdateRecent(const ObjTree& tree) override;
	virtual void UpdateDst(const ObjTree& tree) override;
	virtual void UpdateMoveable(const rec::PathItem& moveable) override;
	virtual void EnableRecent(bool enable) override;

};








}//namespace wh{
#endif // __****_H