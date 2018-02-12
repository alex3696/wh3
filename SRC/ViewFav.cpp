#include "_pch.h"
#include "ViewFav.h"
using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVModel
	: public wxDataViewModel
{
public:

	wxDVModel() {};
	~wxDVModel() {};

	virtual unsigned int	GetColumnCount() const override
	{
		return 2;
	}
	virtual wxString		GetColumnType(unsigned int col) const override
	{
		switch (col)
		{
		case 0: return "wxDataViewIconText";
		default: break;
		}
		return "string";
	}
	virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const override
	{
		return true;
	}

	virtual bool IsContainer(const wxDataViewItem &item)const override
	{
		return false;
	}
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col)override
	{
		return false;
	}
	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
	}
	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override
	{
		return wxDataViewItem(nullptr);
	}

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &arr) const override
	{
		return 0;
	}



};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxAuiPanel : public wxPanel
{
public:
	wxAuiPanel(wxWindow* wnd)
		:wxPanel(wnd)
	{
		mAuiMgr.SetManagedWindow(this);
	}
	~wxAuiPanel()
	{
		mAuiMgr.UnInit();
	}
	wxAuiManager	mAuiMgr;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewFav::ViewFav(const std::shared_ptr<IViewWindow>& parent)
	:ViewFav(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------
ViewFav::ViewFav(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	mPanel = new wxDialog(parent, wxID_ANY, "Favorite attribute"
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	mPanel->SetSizer(szrMain);

	long style = wxAUI_TB_DEFAULT_STYLE
		| wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		//| wxAUI_TB_OVERFLOW
		;
	mToolBar = new wxAuiToolBar(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	mToolBar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить (CTRL+R или CTRL+F5)");
	auto add_tool = mToolBar->AddTool(wxID_ADD, "Добавить", mgr->m_ico_plus24, "Добавить новое свойство для отображения (CTRL+INSERT)");
	add_tool->SetHasDropDown(true);
	mToolBar->AddTool(wxID_DELETE, "Удалить", mgr->m_ico_delete24, "Удалить выбранное свойство (F8)");
	mToolBar->AddTool(wxID_HELP_INDEX, "Справка", wxArtProvider::GetBitmap(wxART_HELP, wxART_TOOLBAR), "Справка (F1)");
	mToolBar->Realize();
	szrMain->Add(mToolBar, 0, wxEXPAND, 0);
	
	mToolBar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN
		, [this, mgr](wxCommandEvent& evt)
	{
		wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());
		tb->SetToolSticky(evt.GetId(), true);
		wxMenu add_menu;
		AppendBitmapMenu(&add_menu, wxID_FILE1, "свойство типа", mgr->m_ico_type24);
		AppendBitmapMenu(&add_menu, wxID_FILE2, "свойство объекта", mgr->m_ico_obj24);
		AppendBitmapMenu(&add_menu, wxID_FILE3, "дату предыдущего действия", mgr->m_ico_acts24);
		AppendBitmapMenu(&add_menu, wxID_FILE4, "период действия", mgr->m_ico_acts24);
		AppendBitmapMenu(&add_menu, wxID_FILE5, "дату следующего действия", mgr->m_ico_acts24);
		AppendBitmapMenu(&add_menu, wxID_FILE6, "остаток дней до след.действия", mgr->m_ico_acts24);
		
		wxRect rect = tb->GetToolRect(evt.GetId());
		wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
		pt = tb->ScreenToClient(pt);
		tb->PopupMenu(&add_menu, pt);
		tb->SetToolSticky(evt.GetId(), false);
		tb->Refresh();

	}
	, wxID_ADD);


	auto mTable = new wxDataViewCtrl(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES
		| wxDV_VERT_RULES
		//| wxDV_HORIZ_RULES
		| wxDV_MULTIPLE
		| wxDV_NO_HEADER
	);

	auto dv_model = new wxDVModel();
	mTable->AssociateModel(dv_model);
	dv_model->DecRef();

	int ch = mTable->GetCharHeight();
	mTable->SetRowHeight(ch * 1.6);

	auto col0 = mTable->AppendTextColumn("Тип", 1, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

	szrMain->Add(mTable, 1, wxALL | wxEXPAND, 2);
	
	
	wxSizer* szrBtn = new wxBoxSizer(wxHORIZONTAL);
	//auto btnOK = new wxButton(mPanel, wxID_OK);
	auto btnCancel = new wxButton(mPanel, wxID_CANCEL,"Закрыть");
	szrBtn->Add(0, 0, 1, wxEXPAND, 2);
	//szrBtn->Add(btnOK, 0, wxALL, 2);
	szrBtn->Add(btnCancel, 0, wxALL , 2);
	szrMain->Add(szrBtn, 0, wxEXPAND, 0);

	
	mPanel->Layout();

	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigRefresh(); }, wxID_REFRESH);

	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddClsProp(); }, wxID_FILE1);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddObjProp(); }, wxID_FILE2);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddPrevios(); }, wxID_FILE3);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddPeriod(); }, wxID_FILE4);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddNext(); }, wxID_FILE5);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddLeft(); }, wxID_FILE6);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_Remove(); }, wxID_DELETE);

	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{sigShowHelp("SelectFavorite"); }, wxID_HELP_INDEX);

}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::SetShow()
{
	mPanel->ShowModal();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::SetUpdateTitle(const wxString& str, const wxIcon& ico)
{
	mPanel->SetTitle(str);
	mPanel->SetIcon(ico);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::SetBeforeUpdate(const std::vector<const IIdent64*>&, const IIdent64 *)
{
}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::SetAfterUpdate(const std::vector<const IIdent64*>&, const IIdent64*) //override;
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddClsProp(wxCommandEvent& evt )
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddObjProp(wxCommandEvent& evt )
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddPrevios(wxCommandEvent& evt )
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddPeriod(wxCommandEvent& evt )
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddNext(wxCommandEvent& evt )
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddLeft(wxCommandEvent& evt )
{

}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_Remove(wxCommandEvent & evt)
{

}
