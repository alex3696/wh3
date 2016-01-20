#include "_pch.h"
#include "TableCtrl.h"

using namespace wh;

//-----------------------------------------------------------------------------
TableCtrl::TableCtrl(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	:wxPanel(parent, id, pos, size, style, name)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	GetSizer()->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	mTableView = new VTable(this);
	GetSizer()->Add(mTableView, 1, wxALL | wxEXPAND, 0);

	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &TableCtrl::OnContextMenu, this);
	Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &TableCtrl::OnSelectChange, this);

	Layout();
}
//-----------------------------------------------------------------------------
void TableCtrl::SetModel(std::shared_ptr<ITable> model)
{
	mMTable = model;
	mTableView->SetModel(model);

	BuildToolBar();
	BuildPopupMenu();
	
	mConnAfterInsert.disconnect();
	mConnAfterRemove.disconnect();
	mConnAfterChange.disconnect();
	
	if (!mMTable)
		return;
	namespace ph = std::placeholders;
	
	auto fnAI = std::bind(&TableCtrl::OnAfterInsert, this, ph::_1, ph::_2, ph::_3);
	auto fnAR = std::bind(&TableCtrl::OnAfterRemove, this, ph::_1, ph::_2);
	auto fnAC = std::bind(&TableCtrl::OnAfterChange, this, ph::_1, ph::_2);
	
	mConnAfterInsert = mMTable->ConnAfterInsert(fnAI);
	mConnAfterRemove = mMTable->ConnectAfterRemove(fnAR);
	mConnAfterChange = mMTable->ConnectChangeSlot(fnAC);

	OnTableChangeState(*mMTable);
}
//-----------------------------------------------------------------------------
void TableCtrl::SetEditor(std::shared_ptr<TableRowEditor> editor)
{
	mEditor = editor;
}
//-----------------------------------------------------------------------------
void TableCtrl::GetSelected(std::vector<unsigned int>& selected)
{

}
//-----------------------------------------------------------------------------
void TableCtrl::OnSelectChange(wxDataViewEvent &event)
{

}
//-----------------------------------------------------------------------------
void TableCtrl::OnContextMenu(wxDataViewEvent &event)
{

}
//-----------------------------------------------------------------------------
void TableCtrl::BuildToolBar()
{
	wxAcceleratorEntry entries[6];
	entries[0].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5,  wxID_REFRESH);
	entries[2].Set(wxACCEL_CTRL, (int) 'S', wxID_SAVE);
	entries[3].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
	entries[4].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_REMOVE);
	entries[5].Set(wxACCEL_CTRL, (int) 'O', wxID_EDIT);
	wxAcceleratorTable accel(6, entries);
	SetAcceleratorTable(accel);

	namespace ph = std::placeholders;

	if (mEnableLoad)
	{
		std::function<void(wxCommandEvent&)> 
			fn = std::bind(&TableCtrl::OnCmdLoad, this, ph::_1);
		auto fnLoad = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_REFRESH, "Обновить", m_ResMgr->m_ico_refresh24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnLoad, wxID_REFRESH);
	}
	if (mEnableSave)
	{
		std::function<void(wxCommandEvent&)> 
			fn = std::bind(&TableCtrl::OnCmdSave, this, ph::_1);
		auto fnSave = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_SAVE, "Сохранить", m_ResMgr->m_ico_save24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnSave, wxID_SAVE);
	}

	mToolBar->AddSeparator();

	if (mEnableInsert)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdInsert, this, ph::_1);
		auto fnInsert = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_NEW, "Добавить", m_ResMgr->m_ico_create24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnInsert, wxID_NEW);
	}

	if (mEnableRemove)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdRemove, this, ph::_1);
		auto fnRemove = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_REMOVE, "Удалить", m_ResMgr->m_ico_delete24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnRemove, wxID_REMOVE);
	}

	if (mEnableChange)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdChange, this, ph::_1);
		auto fnChange = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_EDIT, "Редактировать", m_ResMgr->m_ico_edit24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnChange, wxID_EDIT);
	}
	mToolBar->Realize();
}
//-----------------------------------------------------------------------------
void TableCtrl::BuildPopupMenu()
{
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdLoad(wxCommandEvent& WXUNUSED(evt))
{
	if (!mEnableLoad || !mMTable)
		return;
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndLockUpdater(mTableView);
	mMTable->Load();
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdSave(wxCommandEvent& WXUNUSED(evt))
{
	if (!mEnableSave || !mMTable)
		return;
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndLockUpdater(mTableView);
	mMTable->Save();
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdInsert(wxCommandEvent& WXUNUSED(evt))
{
	if (!mEnableInsert || !mMTable || !mEditor)
		return;

	auto newItem = mMTable->CreateItem();
	if (!newItem)
		return;

	mMTable->Insert(newItem);
	mEditor->SetModel(newItem);

	if (wxID_OK == mEditor->ShowModal())
	{
		mEditor->DataFromWindow();
	}//if (wxID_OK == editor.ShowModal())
	else
	{
		auto state = newItem->GetState();
		if (state == msCreated || state == msNull)
			mMTable->DelChild(newItem);
	}//else if (wxID_OK == editor.ShowModal())
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdRemove(wxCommandEvent& WXUNUSED(evt))
{
	if (!mEnableRemove || !mMTable)
		return;

	int res = wxMessageBox("Вы действительно ходите удалить?",
		"Подтверждение", wxICON_QUESTION | wxYES_NO);
	if (wxYES != res)
		return;


	wxDataViewItemArray remArr;
	auto remQty = mTableView->GetSelections(remArr);

	std::vector<SptrIModel> toDelete;
	for (const auto& dvItem : remArr)
	{
		auto row = mTableView->GetRow(dvItem);
		auto model = mMTable->GetChild(row);
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
		mMTable->DelChild(item);
		

}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdChange(wxCommandEvent& WXUNUSED(evt))
{
	if (!mEnableChange || !mMTable || !mEditor)
		return;

	auto sel = mTableView->GetSelection();
	auto row = mTableView->GetRow(sel);

	auto updItem = mMTable->at(row);

	if (!updItem)
		return;
	mEditor->SetModel(updItem);
	if (wxID_OK == mEditor->ShowModal())
	{
		mEditor->DataFromWindow();
	}// if (wxID_OK
}
//-----------------------------------------------------------------------------
void TableCtrl::OnTableChangeState(const IModel& vec)
{
	bool exist = (msExist == vec.GetState());
	mToolBar->EnableTool(wxID_SAVE, !exist);
	mToolBar->Refresh();
}
//-----------------------------------------------------------------------------
void TableCtrl::OnAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	OnTableChangeState(vec);
}//OnAfterInsert
//-----------------------------------------------------------------------------
void TableCtrl::OnAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	OnTableChangeState(vec);
}//OnAfterRemove
//-----------------------------------------------------------------------------
void TableCtrl::OnAfterChange(const IModel& vec, const std::vector<unsigned int>& itemVec)
{
	OnTableChangeState(vec);
}//OnAfterChange