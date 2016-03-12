#include "_pch.h"
#include "VTableCtrl.h"

using namespace wh;

//-----------------------------------------------------------------------------
VTableCtrl::VTableCtrl()
{
	//fnOnCmdFilter = [](wxCommandEvent&){};
	fnOnCmdFilter = MakeSafeFn(&VTableCtrl::OnCmdFind, wxID_FIND, this);
	fnOnCmdLoad = MakeSafeFn(&VTableCtrl::OnCmdLoad, wxID_REFRESH, this);
	fnOnCmdSave = MakeSafeFn(&VTableCtrl::OnCmdSave, wxID_SAVE, this);
	fnOnCmdInsert = MakeSafeFn(&VTableCtrl::OnCmdInsert, wxID_NEW, this);
	fnOnCmdRemove = MakeSafeFn(&VTableCtrl::OnCmdRemove, wxID_REMOVE, this);
	fnOnCmdEdit = MakeSafeFn(&VTableCtrl::OnCmdChange, wxID_EDIT, this);
	fnOnCmdBackward = MakeSafeFn(&VTableCtrl::OnCmdBackward, wxID_BACKWARD, this);
	fnOnCmdForward = MakeSafeFn(&VTableCtrl::OnCmdForward, wxID_FORWARD, this);
}
//-----------------------------------------------------------------------------
void VTableCtrl::SetModel(std::shared_ptr<ITable> model)
{
	mTableModel = model;
}
//-----------------------------------------------------------------------------
void VTableCtrl::SetAuiMgr(wxAuiManager* auimgr)
{
	mAuiMgr = auimgr;
}
//-----------------------------------------------------------------------------
void VTableCtrl::SetViewToolBar(VTableToolBar* tool_bar)
{
	if (mToolBarView)
	{
		mToolBarView->SetModel(nullptr);
		UnbindCmd(mToolBarView);
	}
	mToolBarView = tool_bar;
	
	if (mToolBarView && mTableModel)
	{
		mToolBarView->SetEnableFilter((bool)fnOnCmdFilter);
		mToolBarView->SetEnableLoad((bool)fnOnCmdLoad);
		mToolBarView->SetEnableSave((bool)fnOnCmdSave);
		mToolBarView->SetEnableInsert((bool)fnOnCmdInsert);
		mToolBarView->SetEnableRemove((bool)fnOnCmdRemove);
		mToolBarView->SetEnableChange((bool)fnOnCmdEdit);

		mToolBarView->SetModel(mTableModel);

		BindCmd(mToolBarView);

		if (mAuiMgr)
		{
			wxAuiPaneInfo&  pi = mAuiMgr->GetPane(mToolBarView);
			if (pi.IsOk())
				pi.BestSize(mToolBarView->GetClientSize());
		}

	}
	
}
//-----------------------------------------------------------------------------
void VTableCtrl::SetViewTable(VTable* table)
{
	if (mTableView)
	{
		mTableView->SetModel(nullptr);
		UnbindCmd(mTableView);
	}
	mTableView = table;
	if (mTableView && mTableModel)
	{
		mTableView->SetModel(mTableModel);

		BindCmd(mTableView);
	}

}
//-----------------------------------------------------------------------------
void VTableCtrl::SetViewFilter(FilterArrayEditor* filter)
{
	if (mFilterView)
	{
		mFilterView->SetModel(nullptr);
	}
	mFilterView = filter;
	if (mFilterView)
	{
		mFilterView->SetModel(mTableModel);
	}
}
//-----------------------------------------------------------------------------
void VTableCtrl::SetEditor(std::shared_ptr<TableRowEditor> editor)
{
	mEditor = editor;
}
//-----------------------------------------------------------------------------
std::shared_ptr<TableRowEditor> VTableCtrl::GetEditor()
{
	if (!mEditor)
		mEditor = std::make_shared<wh::TableRowPGDefaultEditor>();
	return mEditor;
}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdLoad(wxCommandEvent& WXUNUSED(evt))
{
	if (mTableModel)
		mTableModel->Load();
}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdSave(wxCommandEvent& WXUNUSED(evt))
{
	if (mTableModel)
		mTableModel->Save();
}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdInsert(wxCommandEvent& WXUNUSED(evt))
{
	if (!mTableModel)
		return;

	auto newItem = mTableModel->CreateItem();
	if (!newItem)
		return;

	mTableModel->Insert(newItem);
	GetEditor()->SetModel(newItem);

	if (wxID_OK != GetEditor()->ShowModal())
	{
		auto state = newItem->GetState();
		if (state == msCreated || state == msNull)
			mTableModel->DelChild(newItem);
	}//else if (wxID_OK == editor.ShowModal())
}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdRemove(wxCommandEvent& WXUNUSED(evt))
{
	if (!mTableModel || !mTableView)
		return;

	int res = wxMessageBox("�� ������������� ������ �������?",
		"�������������", wxICON_QUESTION | wxYES_NO);
	if (wxYES != res)
		return;


	wxDataViewItemArray remArr;
	auto remQty = mTableView->GetSelections(remArr);

	std::vector<SptrIModel> toDelete;
	for (const auto& dvItem : remArr)
	{
		auto row = mTableView->GetRow(dvItem);
		auto model = mTableModel->GetChild(row);
		ModelState state = model->GetState();
		switch (state)
		{
		default: case msNull: case msCreated:
			toDelete.emplace_back(model);
			break;
		case msExist: case msUpdated:
			model->MarkDeleted();
		case msDeleted:
			break;
		} //switch (state)
	}

	for (auto item : toDelete)
		mTableModel->DelChild(item);

}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdChange(wxCommandEvent& WXUNUSED(evt))
{
	if (!mTableModel || !mTableView)
		return;

	auto sel = mTableView->GetSelection();
	if (!sel.IsOk())
		return;

	auto row = mTableView->GetRow(sel);

	auto updItem = mTableModel->at(row);

	if (!updItem)
		return;
	GetEditor()->SetModel(updItem);
	GetEditor()->ShowModal();
}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdFind(wxCommandEvent& WXUNUSED(evt))
{
	if (!mFilterView)
		return;
	
	bool visible = false;

	if (mAuiMgr)
	{
		wxAuiPaneInfo& pi = mAuiMgr->GetPane("FilterPane");
		if (pi.IsOk())
		{
			visible = !pi.IsShown();
			pi.Show(visible);
			mAuiMgr->Update();
		}//if(!pi.IsOk())	
	}
	else
	{
		visible = !mFilterView->IsShown();
		mFilterView->Show(visible);
	}



	if (mToolBarView)
	{
		wxAuiToolBarItem* tool = mToolBarView->FindTool(wxID_FIND);
		if (tool)
			tool->SetState(visible ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL);
	}

	mAuiMgr->Update();

}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdBackward(wxCommandEvent& WXUNUSED(evt))
{
	if (!mTableModel)
		return;
	const auto& limit = mTableModel->mPageLimit->GetData();
	const auto& no = mTableModel->mPageNo->GetData();

	if (no > 0)
		mTableModel->mPageNo->SetData(no - 1);
	mTableModel->Load();
}
//-----------------------------------------------------------------------------
void VTableCtrl::OnCmdForward(wxCommandEvent& WXUNUSED(evt))
{
	if (!mTableModel)
		return;
	const auto& limit = mTableModel->mPageLimit->GetData();
	const auto& no = mTableModel->mPageNo->GetData();

	if (mTableModel->GetChildQty() >= limit)
		mTableModel->mPageNo->SetData(no + 1);
	mTableModel->Load();
}
//-----------------------------------------------------------------------------
wxAcceleratorTable VTableCtrl::GetAcceleratorTable()const
{
	wxAcceleratorEntry entries[9];
	entries[0].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
	entries[2].Set(wxACCEL_CTRL, (int) 'S', wxID_SAVE);
	entries[3].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
	entries[4].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_REMOVE);
	entries[5].Set(wxACCEL_CTRL, (int) 'O', wxID_EDIT);
	entries[6].Set(wxACCEL_CTRL, (int) 'F', wxID_FIND);
	entries[7].Set(wxACCEL_NORMAL, WXK_PAGEUP, wxID_BACKWARD);
	entries[8].Set(wxACCEL_NORMAL, WXK_PAGEDOWN, wxID_FORWARD);
	wxAcceleratorTable accel(9, entries);
	return accel;
}
//-----------------------------------------------------------------------------
void VTableCtrl::BindCmd(wxWindow* wnd)
{
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdFilter, wxID_FIND);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdLoad, wxID_REFRESH);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdSave, wxID_SAVE);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdInsert, wxID_NEW);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdRemove, wxID_REMOVE);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdEdit, wxID_EDIT);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdBackward, wxID_BACKWARD);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdForward, wxID_FORWARD);

	wnd->SetAcceleratorTable(GetAcceleratorTable());
}
//-----------------------------------------------------------------------------
void VTableCtrl::UnbindCmd(wxWindow* wnd)
{
	wxAcceleratorTable accel;
	wnd->SetAcceleratorTable(accel);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdFilter, wxID_FIND);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdLoad, wxID_REFRESH);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdSave, wxID_SAVE);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdInsert, wxID_NEW);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdRemove, wxID_REMOVE);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdEdit, wxID_EDIT);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdBackward, wxID_BACKWARD);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdForward, wxID_FORWARD);
}

