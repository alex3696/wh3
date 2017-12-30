#include "_pch.h"
#include "ViewHistory.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"
#include "ViewFilterList.h"
#include "ViewObjPropList.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDataViewModelMediator
	: public wxDataViewVirtualListModel
{
	std::shared_ptr<const ModelHistoryTableData> mTD;
public:
	wxDataViewModelMediator(){}
	//virtual bool  GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const override	{}
	//virtual bool  IsEnabledByRow(unsigned int row, unsigned int col) const override {}

	virtual unsigned int  GetCount() const override
	{
		return mTD ? mTD->size() : 0;
	}

	virtual bool  GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const override	
	{
		if (!mTD || mTD->size() <= row)
			return false;
		//switch (col)
		//{
		//case 3:
		//{
			const auto& act = mTD->GetAct(row);
			wxColour clr(act.GetColour() );
			if (!act.GetId().IsEmpty() && *wxWHITE != clr)
			{
				attr.SetBackgroundColour(clr);
				return true;
			}
		//	break;
		//}
		//default:break;
		//}
		return false;
	}

	virtual void  GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const override
	{
		if (!mTD || mTD->size() <= row)
			return;
		wxString		str;
		const wxIcon*	ico = &wxNullIcon;
		wxIcon			icoHolder;
		const auto&		obj = mTD->GetObj(row);

		switch (col)
		{
		case 1:
			{
				
				//wxLocale loc;
				//loc.Init(wxLocale::GetSystemLanguage());
				//wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
				wxString format_d = wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
				//wxString format_ld = wxLocale::GetInfo(wxLOCALE_LONG_DATE_FMT);
				wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
				//wxString format_date = wxLocale::GetOSInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
				
				wxDateTime dt;
				dt.ParseDateTime(mTD->GetDate(row));

				switch (mStringPerRow)
				{
				case 1:
					str = wxString::Format("%s %s %s"
						, dt.Format(format_d), dt.Format(format_t), mTD->GetUser(row));
					break;
				case 2: 
					str = wxString::Format("%s    %s\n  %s"
						, dt.Format(format_d), dt.Format(format_t), mTD->GetUser(row));
					break;
				case 3:
					str = wxString::Format("%s\n%s\n  %s"
						, dt.Format(format_d), dt.Format(format_t), mTD->GetUser(row));
					break;
				default:
					str = wxString::Format("%s\n%s\n\n  %s"
						, dt.Format(format_d), dt.Format(format_t), mTD->GetUser(row));
					break;
				}

			}
			break;
		// тип + объект
		case 2: str = wxString::Format("%s\n%s"
			, obj.GetCls().GetTitle(), obj.GetTitle()
			);
			break;
		// свойтсва
		case 3:
		{
			const auto& act = mTD->GetAct(row);
			if (act.GetId().IsEmpty())
			{
				if(obj.GetCls().GetKind() != "1")
					str = wxString::Format(
						"Перемещение   %s(%s)\nприёмник: %s\nисточник:  %s"
						, mTD->GetQty(row), obj.GetCls().GetMeasure()
						, mTD->GetDstPath(row).AsString(), mTD->GetPath(row).AsString()	);
				else
					str = wxString::Format(
					"Перемещение\nприёмник: %s\nисточник:  %s"
					, mTD->GetDstPath(row).AsString(), mTD->GetPath(row).AsString() );

				icoHolder = wxArtProvider::GetIcon(wxART_REDO, wxART_TOOLBAR);
				ico = &icoHolder;

			}
			else
			{
				wxString prop_str;
				const auto& props = mTD->GetActProperties(row).get<1>();
				for (const auto& p : props)
				{
					prop_str += wxString::Format("%s: %s\n"
						,p->GetProp().GetTitle(), p->GetValue());
				}
				
				if (3 < mStringPerRow)
				{
					str = wxString::Format("%s\n%s", act.GetTitle(), prop_str);
					if(mPathInPropperties)
						str += "Местоположение: " + mTD->GetPath(row).AsString();
				}
				else
					str = wxString::Format("%s    %s\n%s"
					, act.GetTitle()
					, mPathInPropperties ? mTD->GetPath(row).AsString() : wxEmptyString2
					, prop_str);

				//ico = &ResMgr::GetInstance()->m_ico_act24;
			}
		}
		break;
		default:break;
		}
		variant << wxDataViewIconText2(std::move(str), *ico);
	}

	virtual bool  SetValueByRow(const wxVariant &variant, unsigned int row, unsigned int col)
	{
		return false;
	}

	virtual unsigned int	GetColumnCount() const override
	{
		return 3;
	}
	virtual wxString		GetColumnType(unsigned int col) const override
	{
		//return "string";
		return "wxDataViewIconText";
	}

	void SetTable(const std::shared_ptr<const ModelHistoryTableData>& rt)
	{
		mTD = rt;
		Reset(rt->size());
	}

	const wxString& GetLogId(const wxDataViewItem& item)
	{
		if (!mTD || !mTD->size() || !item.IsOk())
			return wxEmptyString2;
		unsigned int row = this->GetRow(item);
		return mTD->GetLogId(row);
	}

	bool   mPathInPropperties = true;
	size_t mStringPerRow = 2;
};
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
ViewTableHistory::ViewTableHistory(wxWindow* parent)
	:mColAutosize(false)
{
	auto table = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, /*wxDV_ROW_LINES | */ wxDV_VERT_RULES | wxDV_HORIZ_RULES);
	auto dv_model = new wxDataViewModelMediator();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	//int ch = table->GetCharHeight();
	//table->SetRowHeight(ch * 4 + 2);

	auto renderer1 = new wxDataViewIconMLTextRenderer();
	renderer1->SetAlignment(wxALIGN_TOP);
	auto attr = renderer1->GetAttr();
	attr.SetColour(*wxBLACK);
	renderer1->SetAttr(attr);

	auto renderer2 = new wxDataViewIconMLTextRenderer();
	renderer2->SetAlignment(wxALIGN_TOP);
	auto renderer3 = new wxDataViewIconMLTextRenderer();
	renderer3->SetAlignment(wxALIGN_TOP);

	auto col1 = new wxDataViewColumn("Время/Пользователь"
		, renderer1, 1, 85, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	col1->SetBitmap(ResMgr::GetInstance()->m_ico_sort_asc16);
	table->AppendColumn(col1);
	auto col2 = new wxDataViewColumn("Тип/Объект"
		, renderer2, 2, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col2);
	auto col3 = new wxDataViewColumn("Действие/Местоположение/Свойства"
		, renderer3, 3, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col3);

	//table->SetCanFocus(false);

	table->GetTargetWindow()->SetToolTip("ToolTip");

	std::function<void(wxMouseEvent&)> on_move = [table](wxMouseEvent& evt)
	{
		wxDataViewColumn* col = nullptr;
		wxDataViewItem item(nullptr);
		auto pos = evt.GetPosition();
		table->HitTest(pos, item, col);

		wxString str;
		if (col && item.IsOk())
		{
			wxVariant var;
			table->GetModel()->GetValue(var, item, col->GetModelColumn());
			wxDataViewIconText2 ico_txt;
			ico_txt << var;
			str = ico_txt.GetText();
		}
		table->GetTargetWindow()->GetToolTip()->SetTip(str);
	};
	table->GetTargetWindow()->Bind(wxEVT_MOTION, on_move);
	table->Bind(wxEVT_MOTION, on_move);
	
	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, &ViewTableHistory::OnCmd_SelectHistoryItem, this);
	
	table->GetTargetWindow()->Bind(wxEVT_LEFT_DCLICK
		, &ViewTableHistory::OnCmd_DClickTable, this);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		, &ViewTableHistory::OnCmd_Update, this, wxID_REFRESH);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		, &ViewTableHistory::OnCmd_Backward, this, wxID_BACKWARD);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		, &ViewTableHistory::OnCmd_Forward, this, wxID_FORWARD);

	wxAcceleratorEntry entries[4];
	entries[0].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
	entries[2].Set(wxACCEL_CTRL, WXK_PAGEUP, wxID_BACKWARD);
	entries[3].Set(wxACCEL_CTRL, WXK_PAGEDOWN, wxID_FORWARD);
	wxAcceleratorTable accel(4, entries);
	table->SetAcceleratorTable(accel);

	mTable = table;


}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableHistory::SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) //override;
{
	wxWindowUpdateLocker lock(mTable);

	auto dv = dynamic_cast<wxDataViewModelMediator*>(mTable->GetModel());
	if (!dv || !rt)
		return;
	auto p0 = GetTickCount();
	dv->SetTable(rt);

	wxLogMessage(wxString::Format("ViewTableHistory:\t%d\t SetTable", GetTickCount() - p0));
	p0 = GetTickCount();

	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
			mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	}

	auto item = dv->GetItem(0);
	if (item.IsOk())
	{
		mTable->EnsureVisible(item);
		mTable->Select(item);
		wxDataViewEvent evt;
		evt.SetItem(item);
		OnCmd_SelectHistoryItem(evt);
	}

	wxLogMessage(wxString::Format("ViewTableHistory:\t%d\t SetBestColumnWidth", GetTickCount() - p0));
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableHistory::SetCfg(const rec::PageHistory& cfg) //override;
{
	mColAutosize = cfg.mColAutosize;

	auto dv = dynamic_cast<wxDataViewModelMediator*>(mTable->GetModel());
	if (!dv)
		return;

	dv->mPathInPropperties = cfg.mPathInProperties;

	if (cfg.mStringPerRow != dv->mStringPerRow)
	{
		wxWindowUpdateLocker lock(mTable);
		dv->mStringPerRow = cfg.mStringPerRow;
		int ch = mTable->GetCharHeight();
		mTable->SetRowHeight(ch * dv->mStringPerRow + 2);
		mTable->Refresh();
	}

	auto col = mTable->GetColumn(1);
	if (col)
	{
		bool col_visible = col->IsShown();
		if (col_visible != cfg.mVisibleColumnClsObj)
			col->SetHidden(!cfg.mVisibleColumnClsObj);
	}

}
//-----------------------------------------------------------------------------

void ViewTableHistory::OnCmd_SelectHistoryItem(wxDataViewEvent& evt)
{
	auto dv = dynamic_cast<wxDataViewModelMediator*>(mTable->GetModel());
	if (!dv)
		return;
	const wxString& str_id = dv->GetLogId(evt.GetItem());
	if (str_id.IsEmpty())
		return;

	sigSelectHistoryItem(str_id);
}
//-----------------------------------------------------------------------------
void ViewTableHistory::OnCmd_DClickTable(wxMouseEvent& evt)
{
	wxCommandEvent new_evt(wxEVT_COMMAND_MENU_SELECTED, wxID_PROPERTIES);
	mTable->ProcessCommand(new_evt);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewToolbarHistory::ViewToolbarHistory(wxWindow* parent)
	:mOffset(0), mLimit(0)
{
	auto mgr = ResMgr::GetInstance();

	long style = wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_TEXT 
		//| wxAUI_TB_HORZ_TEXT
		//| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	mPageLabel = new wxStaticText(tool_bar, wxID_ANY, "0000...0000", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить(F5)");
	tool_bar->AddTool(wxID_FIND, "Фильтры", mgr->m_ico_filter24, "Фильтры(CTRL+F)", wxITEM_NORMAL);
	tool_bar->AddSeparator();
	tool_bar->AddTool(wxID_BACKWARD, "Назад", wxArtProvider::GetBitmap(wxART_GO_UP, wxART_TOOLBAR),
		"Предыдущая страница(CTRL+PAGE UP)");
	tool_bar->AddControl(mPageLabel, "Показаны строки");
	tool_bar->AddTool(wxID_FORWARD, "Вперёд", wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR)
		, "Следующая страница(CTRL+PAGE DOWN)");
	tool_bar->AddSeparator();
	tool_bar->AddTool(wxID_PROPERTIES, "Все свойства"
		, wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR)
		, "Показать/Скрыть все свойства на выбранной момент времени(CTRL+P)", wxITEM_NORMAL);
	mToolExportToExcel = tool_bar->AddTool(wxID_CONVERT, "Экспорт в Excel"
		, mgr->m_ico_export_excel24, "выполнить экспорт в Excel или Calc(CTRL+E)");
	//mToolExportToExcel->SetHasDropDown(true);
	tool_bar->AddTool(wxID_SETUP, "Настройки"
		, mgr->m_ico_options24
		, "Настройки внешнего вида таблийы истории(CTRL+N)");
	tool_bar->Realize();

	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Update, this, wxID_REFRESH);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Backward, this, wxID_BACKWARD);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Forward, this, wxID_FORWARD);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Filter, this, wxID_FIND);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_PropList, this, wxID_PROPERTIES);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Convert, this, wxID_CONVERT);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_ShowSetup, this, wxID_SETUP);

	mToolbar = tool_bar;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarHistory::SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) //override;
{
	const auto str = wxString::Format("%d...%d", mOffset, mOffset + rt->size());
	mPageLabel->SetLabel(str);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarHistory::SetRowsOffset(const size_t& offset) //override;
{
	mOffset = offset;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarHistory::SetRowsLimit(const size_t& limit) //override;
{
	mLimit = limit;
}

//-----------------------------------------------------------------------------

void ViewToolbarHistory::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	sigUpdate();
}
//-----------------------------------------------------------------------------

void ViewToolbarHistory::OnCmd_Backward(wxCommandEvent& evt)
{
	sigPageBackward();
	OnCmd_Update();
}
//-----------------------------------------------------------------------------

void ViewToolbarHistory::OnCmd_Forward(wxCommandEvent& evt)
{
	sigPageForward();
	OnCmd_Update();
}
//-----------------------------------------------------------------------------

void ViewToolbarHistory::OnCmd_Filter(wxCommandEvent& evt)
{
	int state = mToolbar->FindTool(wxID_FIND)->GetState();
	int show = state & wxAUI_BUTTON_STATE_CHECKED;
	sigShowFilterList(show ? false : true);
}
//-----------------------------------------------------------------------------
void ViewToolbarHistory::OnCmd_PropList(wxCommandEvent& evt)
{
	int state = mToolbar->FindTool(wxID_PROPERTIES)->GetState();
	int show = state & wxAUI_BUTTON_STATE_CHECKED;
	sigShowObjPropList(show ? false : true);
}
//-----------------------------------------------------------------------------
void ViewToolbarHistory::OnCmd_Convert(wxCommandEvent& evt)
{
	sigConvertToExcel();
}
//-----------------------------------------------------------------------------
void ViewToolbarHistory::OnCmd_ShowSetup(wxCommandEvent& evt)
{
	sigShowCfgWindow();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarHistory::SetCfg(const rec::PageHistory& cfg) //override;
{
	int show;
	show = cfg.mShowPropertyList ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL;
	mToolbar->FindTool(wxID_PROPERTIES)->SetState(show);

	show = cfg.mShowFilterList ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL;
	mToolbar->FindTool(wxID_FIND)->SetState(show);

}






//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewHistory::ViewHistory(const std::shared_ptr<IViewWindow>& parent)
{
	auto panel = new wxAuiPanel(parent->GetWnd());

	mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	//auto face_colour = *wxRED;
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);

	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);



	mViewToolbarHistory = std::make_shared<ViewToolbarHistory>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarHistory->GetWnd(), wxAuiPaneInfo().
		Name(wxT("HistoryToolBar"))
		.CaptionVisible(false)
		.ToolbarPane()
		.Top()
		.Fixed()
		.Dockable(false)
		.PaneBorder(false)
		.Gripper(false)
		);

	mViewTableHistory = std::make_shared<ViewTableHistory>(panel);
	panel->mAuiMgr.AddPane(mViewTableHistory->GetWnd(), wxAuiPaneInfo().
		Name(wxT("HistoryTable")).CenterPane()
		.PaneBorder(false)
		);
	
	mViewObjPropList = std::make_shared<ViewObjPropList>(panel);
	panel->mAuiMgr.AddPane(mViewObjPropList->GetWnd(), wxAuiPaneInfo().
		Name("ViewObjPropListPane").Caption("Свойства")
		.Left()
		.PaneBorder(false)
		.Hide()
		.CloseButton(false)
		.MinSize(300, 200)
		);

	mViewFilterList = std::make_shared<ViewFilterList>(panel);
	panel->mAuiMgr.AddPane(mViewFilterList->GetWnd(), wxAuiPaneInfo().
		Name("ViewFilterListPane").Caption("Фильтры")
		.Left()//Right()
		.PaneBorder(false)
		.Hide()
		.CloseButton(false)
		.MinSize(250, 200)
		);

	auto toolbar_wnd = std::dynamic_pointer_cast<ViewToolbarHistory>(mViewToolbarHistory).get();
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Update, toolbar_wnd, wxID_REFRESH);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Backward, toolbar_wnd, wxID_BACKWARD);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Forward, toolbar_wnd, wxID_FORWARD);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Filter, toolbar_wnd, wxID_FIND);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_PropList, toolbar_wnd, wxID_PREFERENCES);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Convert, toolbar_wnd, wxID_CONVERT);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_ShowSetup, toolbar_wnd, wxID_SETUP);

	wxAcceleratorEntry entries[7];
	entries[0].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
	entries[1].Set(wxACCEL_CTRL, WXK_PAGEUP, wxID_BACKWARD);
	entries[2].Set(wxACCEL_CTRL, WXK_PAGEDOWN, wxID_FORWARD);
	entries[3].Set(wxACCEL_CTRL, (int) 'F', wxID_FIND);
	entries[4].Set(wxACCEL_CTRL, (int) 'P', wxID_PREFERENCES);
	entries[5].Set(wxACCEL_CTRL, (int) 'E', wxID_CONVERT);
	entries[6].Set(wxACCEL_CTRL, (int) 'N', wxID_SETUP);
	wxAcceleratorTable accel(7, entries);
	//mViewTableHistory->GetWnd()->SetAcceleratorTable(accel);
	//mViewToolbarHistory->GetWnd()->SetAcceleratorTable(accel);
	panel->SetAcceleratorTable(accel);

	mViewTableHistory->GetWnd()->SetFocus();
	panel->mAuiMgr.Update();
	
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewToolbarHistory> ViewHistory::GetViewToolbarHistory()const
{
	return mViewToolbarHistory;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewTableHistory> ViewHistory::GetViewTableHistory()const//override 
{
	return mViewTableHistory;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewFilterList> ViewHistory::GetViewFilterList()const//override 
{ 
	return mViewFilterList; 
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewObjPropList> ViewHistory::GetViewObjPropList()const //override;
{
	return mViewObjPropList;
}
//-----------------------------------------------------------------------------
void ViewHistory::ShowFilterList(bool show)
{
	auto& pane = mAuiMgr->GetPane("ViewFilterListPane");
	if (pane.IsOk())
	{
		bool pane_shown = pane.IsShown();
		if (pane_shown != show)
		{
			wxWindowUpdateLocker lock(mPanel);
			pane.Show(show);
			mAuiMgr->Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ViewHistory::ShowObjPropList(bool show) 
{
	auto& pane = mAuiMgr->GetPane("ViewObjPropListPane");
	if (pane.IsOk())
	{
		bool pane_shown = pane.IsShown();
		if (pane_shown != show)
		{
			wxWindowUpdateLocker lock(mPanel);
			pane.Show(show);
			mAuiMgr->Update();
		}
	}//if (pane.IsOk())
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistory::SetCfg(const rec::PageHistory& cfg) //override;
{
	mCfg = cfg;
	ShowObjPropList(cfg.mShowPropertyList);
	ShowFilterList(cfg.mShowFilterList);

}