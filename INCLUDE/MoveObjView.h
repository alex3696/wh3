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

	virtual void ShowDialog() override { this->ShowModal(); };
	virtual void UpdateRecent(const ObjTree& tree) override;
	virtual void UpdateDst(const ObjTree& tree) override;
	virtual void UpdateMoveable(const rec::PathItem& moveable) override;
	virtual void EnableRecent(bool enable) override;
private:
	void BuildTree();
	void BuildToolBar();

	void ExpandAll(const Node& node); 
	void ExpandAll();
	void OnClickSearchBtn(wxCommandEvent& event);


};



class XMoveObjView : public IMoveObjView
{
	MoveObjView* mWxView;
public:
	~XMoveObjView()
	{
		if (mWxView)
			mWxView->Destroy();
		mWxView = nullptr;
	}

	XMoveObjView(std::shared_ptr<IViewWindow> parent)
		:mWxView(new MoveObjView(parent->GetWnd()))
	{
		mWxView->sigUpdate.connect([this](){ sigUpdate(); });
		mWxView->sigEnableRecent.connect([this](bool enable){ sigEnableRecent(enable); });
		mWxView->sigFindObj.connect([this](const wxString& ss){ sigFindObj(ss); });
		mWxView->sigClose.connect([this](){ sigClose(); });
		mWxView->sigMove.connect([this](const wxString& s1, const wxString& s2){ sigMove(s1, s2); });

		mWxView->Bind(wxEVT_DESTROY, [this](wxWindowDestroyEvent& evt)
		{
			mWxView = nullptr;
		});
	}
	virtual void ShowDialog()override { mWxView->ShowDialog(); }
	virtual void UpdateRecent(const ObjTree& tree)override{ mWxView->UpdateRecent(tree); }
	virtual void UpdateDst(const ObjTree& tree)override{ mWxView->UpdateDst(tree); }
	virtual void UpdateMoveable(const rec::PathItem& moveable)override{ mWxView->UpdateMoveable(moveable); }
	virtual void EnableRecent(bool enable)override{ mWxView->EnableRecent(enable); }
};





}//namespace wh{
#endif // __****_H