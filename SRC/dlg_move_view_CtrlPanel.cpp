#include "_pch.h"
#include "dlg_move_view_CtrlPanel.h"
#include "TViewCtrlPanel.h"


using namespace wh;
using namespace wh::dlg_move::view;

CtrlPanel::CtrlPanel(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	GetSizer()->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	mTree = new Tree(this);
	GetSizer()->Add(mTree, 1, wxALL | wxEXPAND, 0);


	//Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &CtrlPanel::OnActivated, this);

	Layout();
}//
//-----------------------------------------------------------------------------
CtrlPanel::~CtrlPanel()
{

}
//-----------------------------------------------------------------------------

void CtrlPanel::BuildCtrlItem(const int winid, const wxString& label,
	const std::function<void()>& itemFunc,
	const wxBitmap&	ico16, const wxBitmap&	ico24)
{
	std::function<void(wxCommandEvent&)> eventFunctor =
		std::bind(SafeCallEvent(), itemFunc, std::placeholders::_1);

	mToolBar->AddTool(winid, label, ico24, label);
	Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, winid);
	AppendBitmapMenu(&mContextMenu, winid, label, ico16);
}



//-----------------------------------------------------------------------------

void CtrlPanel::BuildLoadItem()
{
	std::function<void()> itemFunc = [this]()
	{
		wxWindowDisabler	wndDisabler(mTree);
		mMovable->Load();
		//mTableView->RebuildColumns();
		mTree->ExpandAll();
		//mTableView->Select(wxDataViewItem(nullptr));
	};

	const int		winid(wxID_REFRESH);
	const wxString	label(L"Обновить");
	const wxBitmap&	ico16 = m_ResMgr->m_ico_refresh16;
	const wxBitmap&	ico24 = m_ResMgr->m_ico_refresh24;

	BuildCtrlItem(winid, label, itemFunc, ico16, ico24);

}

//-----------------------------------------------------------------------------

void CtrlPanel::SetModel(std::shared_ptr<IModel>& model)
{
	auto movable = std::dynamic_pointer_cast<model::MovableObj>(model);
	SetModel(movable);
}
//-----------------------------------------------------------------------------

void CtrlPanel::SetModel(std::shared_ptr<dlg_move::model::MovableObj>& model)
{
	mMovable = model;
	if (mMovable)
	{
		mTree->SetModel(model);
		mToolBar->Clear();
		BuildLoadItem();
		mToolBar->AddSeparator();
		
		mToolBar->Realize();
				
		mMovable->Load();
		mTree->ExpandAll();
	}
}
//---------------------------------------------------------------------------
void CtrlPanel::OnActivated(wxDataViewEvent& evt)
{

	wxDataViewItem selectedItem = evt.GetItem();

	if (selectedItem.IsOk())
	{
		auto mov_data = mMovable->GetData();

		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto typeItem = dynamic_cast<model::DstType *> (modelInterface);
		auto objItem = dynamic_cast<model::DstObj*> (modelInterface);
		if (objItem)
		{
			const auto& obj = objItem->GetData();
			mov_data.mObj.mPID = obj.mID;
			mMovable->SetData(mov_data);
			mMovable->Save();
		}

			
		
		wxMessageBox("TODO execute move transaction", "capt");

	}//if (selectedItem.IsOk())
}

//---------------------------------------------------------------------------
void CtrlPanel::GetSelected(std::vector<unsigned int>& selected)
{
	if (mTree)
		mTree->GetSelected(selected);
}
//---------------------------------------------------------------------------
std::shared_ptr<wh::dlg_move::model::DstObj> CtrlPanel::GetSelected()const
{
	return mTree ? mTree->GetSelected() : std::shared_ptr<wh::dlg_move::model::DstObj>();
}