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
	// Create Aui
	mAuiMgr.SetManagedWindow(this);
	
	// Create AuiToolbar
	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, 0 | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT );

	// Create Page AuiToolbar
	mPageToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, 0 | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT );
	mPageToolBar->AddTool(wxID_BACKWARD, "назад"
		, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_MENU), "Предыдущая страница");
	mPageLabel = new wxStaticText(mPageToolBar, wxID_ANY, "MyLabel");
	mPageLabel->Wrap(-1);

	mPageToolBar->AddControl(mPageLabel, "Показаны строки");
	mPageToolBar->AddTool(wxID_FORWARD, "вперёд"
		, wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU), "Следующая страница");
	mPageToolBar->Realize();

	// Create Filter Panel
	mFilterEditor = new FilterArrayEditor(this);
	mAuiMgr.AddPane(mFilterEditor, wxAuiPaneInfo().
		Name("FilterPane").Caption("Фильтр")
		.Left().Layer(1).Position(1)
		.MinSize(250, 200)
		.CloseButton(false).Dockable(false).Floatable(false)
		.Hide()
		.PaneBorder(false)
		);

	// Create Table Panel
	mTableView = new VTable(this);
	mAuiMgr.AddPane(mTableView, wxAuiPaneInfo().
		Name("TablePane")
		.CenterPane().Layer(1).Position(1)
		.CloseButton(true).MaximizeButton(true)
		.PaneBorder(false));

	wxAuiDockArt* ap = mAuiMgr.GetArtProvider();
	ap->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, mPageToolBar->GetBackgroundColour());
	mAuiMgr.Update();


	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &TableCtrl::OnContextMenu, this);
	Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &TableCtrl::OnSelectChange, this);

	//Bind(wxEVT_DATAVIEW_COLUMN_SORTED, &TableCtrl::OnColumnSort, this);
	Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, &TableCtrl::OnColumnHeaderlClick, this);

	Bind(wxEVT_COMMAND_MENU_SELECTED, &TableCtrl::OnCmdBackward, this, wxID_BACKWARD);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &TableCtrl::OnCmdForward, this, wxID_FORWARD);


}
//-----------------------------------------------------------------------------
TableCtrl::~TableCtrl()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void TableCtrl::SetModel(std::shared_ptr<ITable> model)
{
	mMTable = model;
	mTableView->SetModel(model);
	mFilterEditor->SetModel(model->mFieldVec);

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
std::shared_ptr<TableRowEditor> TableCtrl::GetEditor()
{
	if (!mEditor)
		mEditor = std::make_shared<wh::TableRowPGDefaultEditor>();
	return mEditor;
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
void TableCtrl::OnColumnHeaderlClick(wxDataViewEvent &event)
{
	if (!mMTable)
		return;
	auto column_no = event.GetColumn();
	auto column = event.GetDataViewColumn();

	// очистить всю сортировку и сделать сортировку в выбранном столбце
	for (unsigned int i = 0; i < mMTable->mFieldVec->GetChildQty(); ++i)
	{
		auto field = mMTable->mFieldVec->at(i)->GetData();
		wxDataViewColumn* column = mTableView->GetColumnCount() > i ? 
			mTableView->GetColumn(i) : nullptr;
		
		if (i == column_no && column)
		{
			switch (field.mSort)
			{
			case -1:	
				field.mSort = 0;	
				column->SetBitmap(wxNullBitmap);
				break;
			case 0:		
				field.mSort = 1;	
				column->SetBitmap(m_ResMgr->m_ico_sort_asc16);
				break;
			case 1:		
				field.mSort = -1;	
				column->SetBitmap(m_ResMgr->m_ico_sort_desc16);
				break;
			default:break;
			}
		}
		else
		{
			field.mSort = 0;
			if (column)
				column->SetBitmap(wxNullBitmap);
		}
			
		mMTable->mFieldVec->at(i)->SetData(field, true);
	}
	
	mMTable->mPageNo->SetData(0, true);
	OnCmdLoad(wxCommandEvent(wxID_REFRESH));
	
}

//-----------------------------------------------------------------------------
void TableCtrl::BuildToolBar()
{
	wxWindowUpdateLocker	wndLockUpdater(this);

	wxAcceleratorEntry entries[7];
	entries[0].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5,  wxID_REFRESH);
	entries[2].Set(wxACCEL_CTRL, (int) 'S', wxID_SAVE);
	entries[3].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
	entries[4].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_REMOVE);
	entries[5].Set(wxACCEL_CTRL, (int) 'O', wxID_EDIT);
	entries[6].Set(wxACCEL_CTRL, (int) 'F', wxID_FIND);
	wxAcceleratorTable accel(7, entries);
	SetAcceleratorTable(accel);

	namespace ph = std::placeholders;

	mToolBar->ClearTools();
	mAuiMgr.DetachPane(mPageToolBar);
	mAuiMgr.DetachPane(mToolBar);
	
	if (mEnableFilter)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdFind, this, ph::_1);
		auto fnFind = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_FIND, "Фильтр", m_ResMgr->m_ico_filter24
			, "Показать фильтр(CTRL+F)", wxITEM_CHECK);
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnFind, wxID_FIND);
	}
	if (mEnableLoad)
	{
		std::function<void(wxCommandEvent&)> 
			fn = std::bind(&TableCtrl::OnCmdLoad, this, ph::_1);
		auto fnLoad = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_REFRESH, "Обновить", m_ResMgr->m_ico_refresh24,"Обновить(CTRL+R)");
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnLoad, wxID_REFRESH);
	}
	if (mEnableSave)
	{
		std::function<void(wxCommandEvent&)> 
			fn = std::bind(&TableCtrl::OnCmdSave, this, ph::_1);
		auto fnSave = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_SAVE, "Сохранить", m_ResMgr->m_ico_save24,"Сохранить(CTRL+S)");
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnSave, wxID_SAVE);
	}

	mToolBar->AddSeparator();

	if (mEnableInsert)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdInsert, this, ph::_1);
		auto fnInsert = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_NEW, "Добавить", m_ResMgr->m_ico_create24, "Добавить(CTRL+N)");
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnInsert, wxID_NEW);
	}
	if (mEnableRemove)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdRemove, this, ph::_1);
		auto fnRemove = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_REMOVE, "Удалить", m_ResMgr->m_ico_delete24, "Удалить(DELETE)");
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnRemove, wxID_REMOVE);
	}
	
	if (mEnableChange)
	{
		std::function<void(wxCommandEvent&)>
			fn = std::bind(&TableCtrl::OnCmdChange, this, ph::_1);
		auto fnChange = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		mToolBar->AddTool(wxID_EDIT, "Редактировать", m_ResMgr->m_ico_edit24, "Редактировать(CTRL+O)");
		Bind(wxEVT_COMMAND_MENU_SELECTED, fnChange, wxID_EDIT);
	}


	mToolBar->Realize();
	mAuiMgr.AddPane(mToolBar, wxAuiPaneInfo().
		Name("ToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		);

	mPageToolBar->Realize();
	mAuiMgr.AddPane(mPageToolBar, wxAuiPaneInfo().
		Name("PageToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false).CaptionVisible(false)
		);

	mAuiMgr.Update();
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

	auto itemLimit = mTableView->GetClientSize().GetHeight() / mTableView->GetRowHeight() - 1;
	mMTable->mPageLimit->SetData(itemLimit, true);

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
	if (!mEnableInsert || !mMTable)
		return;

	auto newItem = mMTable->CreateItem();
	if (!newItem)
		return;

	mMTable->Insert(newItem);
	GetEditor()->SetModel(newItem);

	if (wxID_OK != GetEditor()->ShowModal())
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
	if (!mEnableChange || !mMTable)
		return;

	auto sel = mTableView->GetSelection();
	if (!sel.IsOk())
		return;

	auto row = mTableView->GetRow(sel);

	auto updItem = mMTable->at(row);

	if (!updItem)
		return;
	GetEditor()->SetModel(updItem);
	GetEditor()->ShowModal();
}
//-----------------------------------------------------------------------------
void TableCtrl::OnTableChangeState(const IModel& vec)
{
	wxWindowUpdateLocker	wndLockUpdater(this);

	bool exist = (msExist == vec.GetState());
	mToolBar->EnableTool(wxID_SAVE, !exist);
	mToolBar->Refresh();

	if (!mMTable)
		return;

	const auto& limit = mMTable->mPageLimit->GetData();
	const auto& no = mMTable->mPageNo->GetData();
	const auto& curr_qty = mMTable->GetChildQty();

	wxString page_label;
	page_label << " " << (no*limit) << " - " << (no*limit + curr_qty);
	mPageLabel->SetLabel(page_label);

	mPageToolBar->EnableTool(wxID_BACKWARD, no ? true : false);
	mPageToolBar->EnableTool(wxID_FORWARD, curr_qty >= limit);
	mPageToolBar->Refresh();
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdFind(wxCommandEvent& WXUNUSED(evt))
{
	wxAuiPaneInfo& pi = mAuiMgr.GetPane("FilterPane");
	if (pi.IsOk())
	{
		bool visible = !pi.IsShown();
		pi.Show(visible);

		wxAuiToolBarItem* tool = mToolBar->FindTool(wxID_FIND);
		if (tool)
			tool->SetState(visible ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL);

		mAuiMgr.Update();
	}//if(!pi.IsOk())	
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdBackward(wxCommandEvent& evt)
{
	if (!mMTable)
		return;
	const auto& limit = mMTable->mPageLimit->GetData();
	const auto& no = mMTable->mPageNo->GetData();

	if (no > 0)
		mMTable->mPageNo->SetData(no-1);

	OnCmdLoad(evt);
}
//-----------------------------------------------------------------------------
void TableCtrl::OnCmdForward(wxCommandEvent& evt)
{
	if (!mMTable)
		return;
	const auto& limit = mMTable->mPageLimit->GetData();
	const auto& no = mMTable->mPageNo->GetData();

	if (mMTable->GetChildQty() >= limit)
		mMTable->mPageNo->SetData(no + 1);
	
	OnCmdLoad(evt);
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