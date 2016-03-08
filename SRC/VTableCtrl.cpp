#include "_pch.h"
#include "VTableCtrl.h"

using namespace wh;

//-----------------------------------------------------------------------------
VTableCtrl::VTableCtrl()
{
	fnOnCmdFilter = [](wxCommandEvent&){};
	fnOnCmdLoad = MakeSafeFn(&VTableCtrl::OnCmdLoad, wxID_REFRESH);
	fnOnCmdSave = MakeSafeFn(&VTableCtrl::OnCmdSave, wxID_SAVE);
	fnOnCmdInsert = MakeSafeFn(&VTableCtrl::OnCmdInsert, wxID_NEW);
	fnOnCmdRemove = MakeSafeFn(&VTableCtrl::OnCmdRemove, wxID_REMOVE);
	fnOnCmdEdit = MakeSafeFn(&VTableCtrl::OnCmdChange, wxID_EDIT);
	fnOnCmdBackward = MakeSafeFn(&VTableCtrl::OnCmdBackward, wxID_BACKWARD);
	fnOnCmdForward = MakeSafeFn(&VTableCtrl::OnCmdForward, wxID_FORWARD);
}
//-----------------------------------------------------------------------------
std::function<void(wxCommandEvent&)> VTableCtrl::MakeSafeFn
	(void (VTableCtrl::*method)(wxCommandEvent &), int cmd_id)
{
	namespace ph = std::placeholders;

	std::function<void(wxCommandEvent&)>
		fn = std::bind(method, this, ph::_1);
	auto safe_fn = std::bind(SafeCallCommandEvent(), fn, ph::_1);
	return safe_fn;
}
//-----------------------------------------------------------------------------
void VTableCtrl::SetTableViewModel(std::shared_ptr<ITable> model, VTable* tview, wxWindow* panel)
{
	//mTableView->Unbind(wxEVT_COMMAND_MENU_SELECTED,&VTableCtrl::OnCmdLoad, wxID_REFRESH);
	mTableModel = model;
	if (tview == mTableView)
		return;
	
	if (mTableView)
	{
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdLoad, wxID_FIND);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdLoad, wxID_REFRESH);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdSave, wxID_SAVE);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdInsert, wxID_NEW);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdRemove, wxID_REMOVE);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdEdit, wxID_EDIT);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdBackward, wxID_BACKWARD);
		mPanel->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdForward, wxID_FORWARD);
	}

	mTableView = tview;
	if (!mTableView)
		return;
	mPanel = panel ? panel : tview->GetParent();
	if (!mPanel)
		return;

	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdFilter, wxID_FIND);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdLoad, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdSave, wxID_SAVE);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdInsert, wxID_NEW);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdRemove, wxID_REMOVE);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdEdit, wxID_EDIT);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdBackward, wxID_BACKWARD);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdForward, wxID_FORWARD);
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


