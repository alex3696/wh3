#include "_pch.h"
#include "VClsFavActPropCtrl.h"


using namespace wh;
using namespace view;
//-----------------------------------------------------------------------------
VClsFavActPropCtrl::VClsFavActPropCtrl(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: TViewCtrlPanel(parent, id, pos, size, style, name)
{
	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_NORMAL, WXK_BACK, wxID_BACKWARD); // , KEY_SPACE);
	wxAcceleratorTable accel(1, entries);
	SetAcceleratorTable(accel);

	Bind(wxEVT_MENU, &VClsFavActPropCtrl::OnBack, this, wxID_BACKWARD);
	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &VClsFavActPropCtrl::OnActivated, this);

}//
//-----------------------------------------------------------------------------
void VClsFavActPropCtrl::BuildGoUpItem()
{
	std::function<void()> itemFunc =
		[this]() {	this->OnBack(wxCommandEvent());	};

	const int		winid(wxID_BACKWARD);
	const wxString	label(L"Назад");
	const wxBitmap&	ico16 = m_ResMgr->m_ico_slot_list16;
	const wxBitmap&	ico24 = m_ResMgr->m_icoBack24;

	BuildCtrlItem(winid, label, itemFunc, ico16, ico24);

}

//-----------------------------------------------------------------------------
void VClsFavActPropCtrl::SetModel(std::shared_ptr<IModel> model)
{
	mClsNode = std::dynamic_pointer_cast<MClsFavorite>(model);
	if (mClsNode)
	{
		auto clsArray = mClsNode->GetClsArray();
		if (clsArray)
		{
			TViewCtrlPanel::SetModel(clsArray);
			mToolBar->AddSeparator();
			BuildGoUpItem();
			mToolBar->Realize();
		}
	}
}
//---------------------------------------------------------------------------
void VClsFavActPropCtrl::OnActivated(wxDataViewEvent& evt)
{
	wxDataViewItem selectedItem = evt.GetItem();

	auto clsNodeArray = std::dynamic_pointer_cast<MClsFavoriteArray>(mVecModel);

	if (clsNodeArray && mClsNode && selectedItem.IsOk())
	{
		unsigned int row = mTableView->GetRow(selectedItem);
		std::shared_ptr<IModel> childItem = clsNodeArray->GetChild(row);

		auto item = std::dynamic_pointer_cast<MClsFavorite>(childItem);
		if (item)
		{
			auto cls = item->GetData();
			if ("0" == cls.mType)
			{
				clsNodeArray->Clear();
				mClsNode->SetData(cls);
				mClsNode->MarkSavedData();
				mClsNode->Load();
			}// if ("0" == cls.mType)
		}// if (item)
	}//if (mNodeModel && selectedItem.IsOk() )
}
//---------------------------------------------------------------------------
void VClsFavActPropCtrl::OnBack(wxCommandEvent& event)
{
	if (mClsNode)
	{
		const auto& cls = mClsNode->GetData();

		if (cls.mLabel != "Object")
		{
			rec::Cls cls("-1", cls.mParent);

			std::shared_ptr<IModel> childsArray = mClsNode->GetClsArray();
			childsArray->Clear();
			mClsNode->SetData(cls);
			mClsNode->Load();
		}


	}
}
