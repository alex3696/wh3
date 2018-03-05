#include "_pch.h"
#include "ViewFav.h"
using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVModel
	: public wxDataViewModel
{
	const wxDataViewItem  NullDataViewItem = wxDataViewItem(nullptr);

	
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
		case 1: return "string";
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
		if (!item.IsOk())
			return true;

		const auto node = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*>(node);
		return (bool)cls;
		//if(cls)
		//	return false;
		//return (ClsKind::Abstract == cls->GetKind());

	}
	void GetErrorValue(wxVariant &variant, unsigned int col) const
	{
		switch (col)
		{
		case 0:variant << wxDataViewIconText("*ERROR*", wxNullIcon); break;
		default: variant = "*ERROR*"; break;
		}
	}
	void GetValue(wxVariant &variant, unsigned int col
		, const FavAProp& prop) const
	{
		switch (col)
		{
		case 0: {
			auto mgr = ResMgr::GetInstance();

			wxString str = wxString::Format("%s : %s"
				, prop.mAct->GetTitle()
				, FavAPropInfo2Text(prop.mInfo));
			const wxIcon& ico = GetIcon(prop.mInfo);
			variant << wxDataViewIconText(str, ico);
		}break;
		case 1: variant = "Данные последнего действия"; break;
		default: break;
		}//switch
	}

	void GetValue(wxVariant &variant, unsigned int col
		, const ObjProp& prop) const
	{
		switch (col)
		{
		case 0: {
			auto mgr = ResMgr::GetInstance();
			const wxIcon& ico = mgr->m_ico_classprop24;
			variant << wxDataViewIconText(prop.GetTitle(), ico);
		}break;
		case 1: variant = "Свойство объекта"; break;
		default: break;
		}//switch
	}

	void GetValue(wxVariant &variant, unsigned int col
		, const IProp64& prop) const
	{
		switch (col)
		{
		case 0: {
			auto mgr = ResMgr::GetInstance();
			const wxIcon& ico = mgr->m_ico_classprop24;
			variant << wxDataViewIconText(prop.GetTitle(), ico);
		}break;
		case 1: variant = "Свойство класса"; break;
		default: break;
		}//switch
	}

	void GetValue(wxVariant &variant, unsigned int col
		, const ICls64& cls) const
	{
		auto mgr = ResMgr::GetInstance();
		const wxIcon*  ico(&wxNullIcon);

		switch (col)
		{
		case 0: {
			switch (cls.GetKind())
			{
			case ClsKind::Abstract: ico = &mgr->m_ico_type_abstract24; break;
			case ClsKind::Single:	ico = &mgr->m_ico_type_num24; break;
			case ClsKind::QtyByOne:
			case ClsKind::QtyByFloat:
			default: ico = &mgr->m_ico_type_qty24;	break;
			}//switch
			variant << wxDataViewIconText(cls.GetTitle(), *ico);
		}break;
		default: break;
		}
	}
	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
		const auto iobj = static_cast<const IObject*> (dvitem.GetID());

		const auto cls = dynamic_cast<const ICls64*>(iobj);
		if (cls)
		{
			GetValue(variant, col, *cls);
			return;
		}
		const auto oprop = dynamic_cast<const ObjProp*>(iobj);
		if (oprop)
		{
			GetValue(variant, col, *oprop);
			return;
		}
		const auto cprop = dynamic_cast<const IProp64*>(iobj);
		if (cprop)
		{
			GetValue(variant, col, *cprop);
			return;
		}
		const auto act_prop = dynamic_cast<const FavAProp*>(iobj);
		if (act_prop)
		{
			GetValue(variant, col, *act_prop);
			return;
		}

		GetErrorValue(variant, col);
	}
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col)override
	{
		return false;
	}
	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override
	{
		if (!item.IsOk())
			return NullDataViewItem;
		const auto ident = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
		{
			const auto parent = cls->GetParent();
			if (!parent)
				return NullDataViewItem;
			return wxDataViewItem((void*)parent.get());
		}

		const auto it = mPropParent.find(item);
		if (mPropParent.cend() != it)
			return it->second;

		return NullDataViewItem;
	}

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &arr) const override
	{
		if (!parent.IsOk())
		{
			for (const auto& cls : mClsBranch)
			{
				wxDataViewItem item((void*)cls);
				arr.Add(item);
			}
		}
		else
		{
			const auto ident = static_cast<const IIdent64*> (parent.GetID());
			const auto cls = dynamic_cast<const ICls64*>(ident);
			if (cls)
			{
				const auto fav_prop_table = cls->GetFavCPropValue();
				for (const auto& fp : fav_prop_table)
				{
					wxDataViewItem item((void*)fp->mProp.get());
					arr.Add(item);
				}
				const auto fav_oprop_table = cls->GetFavOProp();
				for (const auto& fp : fav_oprop_table)
				{
					wxDataViewItem item((void*)fp.get());
					arr.Add(item);
				}
				const auto fav_act_table = cls->GetFavAProp();
				for (const auto& p : fav_act_table)
				{
					wxDataViewItem item((void*)p.get());
					arr.Add(item);
				}
			}
		}

		return arr.size();
	}


	std::vector<const ICls64*> mClsBranch;
	std::map<wxDataViewItem, wxDataViewItem> mPropParent;


	void SetClsBranch(const std::vector<const ICls64*>& cls_branch)
	{
		mPropParent.clear();
		mClsBranch = cls_branch;

		for (const auto& cls : cls_branch)
		{
			wxDataViewItem parent((void*)cls);

			const auto fav_prop_table = cls->GetFavCPropValue();
			for (const auto& fp : fav_prop_table)
			{
				wxDataViewItem item((void*)fp->mProp.get());
				mPropParent.insert(std::make_pair(item, parent));
			}
			const auto fav_oprop_table = cls->GetFavOProp();
			for (const auto& fp : fav_oprop_table)
			{
				wxDataViewItem item((void*)fp.get());
				mPropParent.insert(std::make_pair(item, parent));
			}
			const auto fav_act_table = cls->GetFavAProp();
			for (const auto& p : fav_act_table)
			{
				wxDataViewItem item((void*)p.get());
				mPropParent.insert(std::make_pair(item, parent));
			}
		}


		Cleared();
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
		| wxAUI_TB_OVERFLOW
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
		AppendBitmapMenu(&add_menu, wxID_FILE1, "свойство типа", mgr->m_ico_folder_type24 );
		AppendBitmapMenu(&add_menu, wxID_FILE2, "свойство объекта", mgr->m_ico_obj24);
		AppendBitmapMenu(&add_menu, wxID_FILE3, "дату предыдущего действия", mgr->m_ico_act_previos16);
		AppendBitmapMenu(&add_menu, wxID_FILE4, "период действия", mgr->m_ico_act_period16);
		AppendBitmapMenu(&add_menu, wxID_FILE5, "дату следующего действия", mgr->m_ico_act_next16);
		AppendBitmapMenu(&add_menu, wxID_FILE6, "остаток дней до след.действия", mgr->m_ico_act_left16);
		
		wxRect rect = tb->GetToolRect(evt.GetId());
		wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
		pt = tb->ScreenToClient(pt);
		tb->PopupMenu(&add_menu, pt);
		tb->SetToolSticky(evt.GetId(), false);
		tb->Refresh();

	}
	, wxID_ADD);


	mTable = new wxDataViewCtrl(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES
		| wxDV_VERT_RULES
		//| wxDV_HORIZ_RULES
		//| wxDV_MULTIPLE
		//| wxDV_NO_HEADER
	);

	auto dv_model = new wxDVModel();
	mTable->AssociateModel(dv_model);
	dv_model->DecRef();

	#define ICON_HEIGHT 24+2
	int row_height = mTable->GetCharHeight() + 2;// + 1px in bottom and top 
	if (ICON_HEIGHT > row_height)
		row_height = ICON_HEIGHT;
	mTable->SetRowHeight(row_height);

	auto col0 = mTable->AppendIconTextColumn("Тип", 0, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);

	auto col1 = mTable->AppendTextColumn("Описание", 1, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);


	szrMain->Add(mTable, 1, wxALL | wxEXPAND, 2);
	
	
	wxSizer* szrBtn = new wxBoxSizer(wxHORIZONTAL);
	//auto btnOK = new wxButton(mPanel, wxID_OK);
	auto btnCancel = new wxButton(mPanel, wxID_CANCEL,"Закрыть");
	szrBtn->Add(0, 0, 1, wxEXPAND, 2);
	//szrBtn->Add(btnOK, 0, wxALL, 2);
	szrBtn->Add(btnCancel, 0, wxALL , 2);
	szrMain->Add(szrBtn, 0, wxEXPAND, 0);

	mPanel->Layout();
	mPanel->SetSize(500, 400);

	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigRefresh(); }, wxID_REFRESH);

	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddClsProp(); }, wxID_FILE1);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddObjProp(); }, wxID_FILE2);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddPrevios(); }, wxID_FILE3);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddPeriod(); }, wxID_FILE4);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddNext(); }, wxID_FILE5);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_AddLeft(); }, wxID_FILE6);
	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {OnCmd_Remove(); }, wxID_DELETE);

	mToolBar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{sigShowHelp("PageBrowserByType_favorite"); }, wxID_HELP_INDEX);

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
void ViewFav::SetBeforeUpdate(const std::vector<const ICls64*>& cls_branch, const ICls64&)
{
	auto dvmodel = dynamic_cast<wxDVModel*>(mTable->GetModel());
	if (!dvmodel)
		return;
	dvmodel->SetClsBranch(cls_branch);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::SetAfterUpdate(const std::vector<const ICls64*>& cls_branch, const ICls64&) //override;
{
	auto dvmodel = dynamic_cast<wxDVModel*>(mTable->GetModel());
	if (!dvmodel)
		return;
	dvmodel->SetClsBranch(cls_branch);

	std::for_each(cls_branch.cbegin(), cls_branch.cend(), 
		[this](const ICls64* cls)
		{
			wxDataViewItem item((void*)cls);
			mTable->Expand(item);
		});

	for (size_t i = 0; i < mTable->GetColumnCount(); i++)
	{
		auto col_pos = mTable->GetModelColumnIndex(i);
		auto col = mTable->GetColumn(col_pos);
		if (col)
			col->SetWidth(mTable->GetBestColumnWidth(i));
	}

	auto top_item = mTable->GetTopItem();
	if (top_item.IsOk())
		mTable->Select(top_item);

}
//-----------------------------------------------------------------------------
const ICls64* ViewFav::GetSelectedItemCls()const
{
	auto dvmodel = dynamic_cast<wxDVModel*>(mTable->GetModel());
	if (!dvmodel)
		return nullptr;

	wxDataViewItem item = mTable->GetCurrentItem();
	const IObject* ident = static_cast<const IObject *> (item.GetID());
	if (!ident)
		return nullptr;

	auto cls = dynamic_cast<const ICls64*>(ident);
	if (!cls)
	{
		auto parent = dvmodel->GetParent(item);
		if (!parent.IsOk())
			return nullptr;
		cls = static_cast<const ICls64*>(parent.GetID());
	}
	return cls;
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddClsProp(wxCommandEvent& evt )
{
	const auto cls = GetSelectedItemCls();
	if (cls)
		sigAddClsProp(cls->GetId() );
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddObjProp(wxCommandEvent& evt )
{
	const auto cls = GetSelectedItemCls();
	if (cls)
		sigAddObjProp(cls->GetId());
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddPrevios(wxCommandEvent& evt )
{
	const auto cls = GetSelectedItemCls();
	if (cls)
		sigAddActProp(cls->GetId(), FavAPropInfo::PreviosDate);
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddPeriod(wxCommandEvent& evt )
{
	const auto cls = GetSelectedItemCls();
	if (cls)
		sigAddActProp(cls->GetId(), FavAPropInfo::PeriodDay);
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddNext(wxCommandEvent& evt )
{
	const auto cls = GetSelectedItemCls();
	if (cls)
		sigAddActProp(cls->GetId(), FavAPropInfo::NextDate);
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_AddLeft(wxCommandEvent& evt )
{
	const auto cls = GetSelectedItemCls();
	if (cls)
		sigAddActProp(cls->GetId(), FavAPropInfo::LeftDay);
}
//-----------------------------------------------------------------------------
void ViewFav::OnCmd_Remove(wxCommandEvent & evt)
{
	wxDataViewItem item = mTable->GetCurrentItem();
	const IObject* ident = static_cast<const IObject *> (item.GetID());
	if (!ident)
		return;

	auto dvmodel = dynamic_cast<wxDVModel*>(mTable->GetModel());
	if (!dvmodel)
		return;


	const auto oprop = dynamic_cast<const ObjProp*>(ident);
	if (oprop)
	{
		auto parent = dvmodel->GetParent(item);
		if (!parent.IsOk())
			return;
		const auto cls = static_cast<const ICls64*>(parent.GetID());

		sigRemoveObjProp(cls->GetId(), oprop->GetId());
		return;
	}
	const auto cprop = dynamic_cast<const IProp64*>(ident);
	if (cprop)
	{
		auto parent = dvmodel->GetParent(item);
		if (!parent.IsOk())
			return;
		const auto cls = static_cast<const ICls64*>(parent.GetID());

		sigRemoveClsProp(cls->GetId(), cprop->GetId());
		return;
	}
	const auto act_prop = dynamic_cast<const FavAProp*>(ident);
	if (act_prop)
	{
		auto parent = dvmodel->GetParent(item);
		if (!parent.IsOk())
			return;
		const auto cls = static_cast<const ICls64*>(parent.GetID());
		sigRemoveActProp(cls->GetId(), act_prop->mAct->GetId(), act_prop->mInfo );
		return;
	}

}
