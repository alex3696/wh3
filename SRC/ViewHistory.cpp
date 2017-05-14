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
				str = wxString::Format(
					"Перемещение   %s(%s)\nприёмник: %s\nисточник:  %s"
					, mTD->GetQty(row), obj.GetCls().GetMeasure()
					, mTD->GetDstPath(row).AsString(), mTD->GetPath(row).AsString()
					);
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
	size_t mStringPerRow;
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
ViewHistory::ViewHistory(std::shared_ptr<IViewWindow> parent)
	:IViewHistory(), mOffset(0), mLimit(0)

{
	auto panel = new wxAuiPanel(parent->GetWnd());

	mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	//auto face_colour = *wxRED;
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);

	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);


	mToolbar = BuildToolBar(mPanel);
	panel->mAuiMgr.AddPane(mToolbar, wxAuiPaneInfo().
		Name(wxT("ReportToolBar"))
		.CaptionVisible(false)
		.ToolbarPane()
		.Top()
		.Fixed()
		.Dockable(false)
		.PaneBorder(false)
		.Gripper(false)
		);

	mTable = BuildTable(mPanel);
	panel->mAuiMgr.AddPane(mTable, wxAuiPaneInfo().
		Name(wxT("ReportTable")).CenterPane()
		.PaneBorder(false)
		);

	mViewFilterList = std::make_shared<ViewFilterList>(panel);
	panel->mAuiMgr.AddPane(mViewFilterList->GetWnd(), wxAuiPaneInfo().
		Name("ViewFilterListPane").Caption("Фильтры").
		Right()
		.PaneBorder(false)
		.Hide()
		.CloseButton(false)
		.MinSize(250,200)
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



	panel->mAuiMgr.Update();
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Backward, this, wxID_BACKWARD);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Forward, this, wxID_FORWARD);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Filter, this, wxID_FIND);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_PropList, this, wxID_PROPERTIES);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_ShowSetup, this, wxID_SETUP);

	mTable->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &ViewHistory::OnCmd_SelectHistoryItem
		, this);

	mTable->GetTargetWindow()->Bind(wxEVT_LEFT_DCLICK, &ViewHistory::OnCmd_DClickTable
		, this);

	
}
//-----------------------------------------------------------------------------

wxWindow* ViewHistory::GetWnd()const
{
	return mPanel;
}
//-----------------------------------------------------------------------------

wxAuiToolBar* ViewHistory::BuildToolBar(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT /*| wxAUI_TB_OVERFLOW*/);
	
	mPageLabel = new wxStaticText(tool_bar, wxID_ANY, "00000 - 00000", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24);
	tool_bar->AddTool(wxID_BACKWARD, "Назад", wxArtProvider::GetIcon(wxART_GO_BACK, wxART_TOOLBAR));
	tool_bar->AddControl(mPageLabel, "Показаны строки");
	tool_bar->AddTool(wxID_FORWARD, "Вперёд", wxArtProvider::GetIcon(wxART_GO_FORWARD, wxART_TOOLBAR));
	tool_bar->AddTool(wxID_FIND, "Фильтры", mgr->m_ico_filter24,wxEmptyString, wxITEM_CHECK);
	mToolExportToExcel = tool_bar->AddTool(wxID_CONVERT, "Экспорт в Excel"
		, mgr->m_ico_export_excel24, "выполнить экспорт в Excel или Calc");
	mToolExportToExcel->SetHasDropDown(true);

	tool_bar->AddTool(wxID_PROPERTIES, "Все свойства"
		, wxArtProvider::GetIcon(wxART_LIST_VIEW, wxART_TOOLBAR)
		, "Показать/Скрыть все свойства на выбранной момент времени", wxITEM_CHECK);
	tool_bar->AddTool(wxID_SETUP, "Настройки"
		, wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_TOOLBAR));
	tool_bar->Realize();
	return tool_bar;
}
//-----------------------------------------------------------------------------

wxDataViewCtrl* ViewHistory::BuildTable(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize
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
		, renderer1, 1, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
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



	return table;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistory::SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) //override;
{
	auto dv = dynamic_cast<wxDataViewModelMediator*>(mTable->GetModel());
	if (!dv || !rt)
		return;

	auto p0 = GetTickCount();

	dv->SetTable(rt);
	
	
	wxLogMessage(wxString::Format("%d \t ViewHistoryTable : \t SetTable", GetTickCount() - p0));
	p0 = GetTickCount();

	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
			mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	}
	mTable->EnsureVisible(dv->GetItem(0));

	const auto str = wxString::Format("%d - %d", mOffset, mOffset + rt->size());
	mPageLabel->SetLabel(str);

	wxLogMessage(wxString::Format("%d \t ViewHistoryTable : \t SetBestColumnWidth", GetTickCount() - p0));
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistory::SetRowsOffset(const size_t& offset) //override;
{
	mOffset = offset;
	const auto str = wxString::Format("%d - %d", mOffset, mOffset + mLimit);
	mPageLabel->SetLabel(str);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistory::SetRowsLimit(const size_t& limit) //override;
{
	mLimit = limit;
	const auto str = wxString::Format("%d - %d", mOffset, mOffset + mLimit);
	mPageLabel->SetLabel(str);
}

//-----------------------------------------------------------------------------

void ViewHistory::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);

	sigUpdate();
}
//-----------------------------------------------------------------------------

void ViewHistory::OnCmd_Backward(wxCommandEvent& evt)
{
	sigPageBackward(); 
	OnCmd_Update();
}
//-----------------------------------------------------------------------------

void ViewHistory::OnCmd_Forward(wxCommandEvent& evt)
{
	sigPageForward();
	OnCmd_Update();
}
//-----------------------------------------------------------------------------

void ViewHistory::OnCmd_Filter(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

	auto& pane = mAuiMgr->GetPane("ViewFilterListPane");
	bool show = pane.IsShown();

	sigShowFilterList(!show);
	//OnCmd_Update();
}
//-----------------------------------------------------------------------------
void ViewHistory::OnCmd_PropList(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

	auto& pane = mAuiMgr->GetPane("ViewObjPropListPane");
	if (pane.IsOk())
	{
		bool show = !pane.IsShown();
		sigShowObjPropList(show);
	}
}
//-----------------------------------------------------------------------------

void ViewHistory::OnCmd_SelectHistoryItem(wxDataViewEvent& evt)
{
	if (!IsShowObjPropList())
		return;
	auto dv = dynamic_cast<wxDataViewModelMediator*>(mTable->GetModel());
	if (!dv)
		return;
	const wxString& str_id = dv->GetLogId(evt.GetItem());
	if (str_id.IsEmpty())
		return;
	
	sigSelectHistoryItem(str_id);
}
//-----------------------------------------------------------------------------
void ViewHistory::OnCmd_DClickTable(wxMouseEvent& evt)
{
	wxCommandEvent new_evt(wxEVT_COMMAND_MENU_SELECTED, wxID_PROPERTIES);
	mToolbar->ProcessCommand(new_evt);
}
//-----------------------------------------------------------------------------
void ViewHistory::OnCmd_ShowSetup(wxCommandEvent& evt)
{
	sigShowCfgWindow();
}
//-----------------------------------------------------------------------------
//IViewWindow virtual 
void ViewHistory::OnShow()//override 
{
	//OnCmd_Update(); 
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
		mToolbar->ToggleTool(wxID_FIND, show);
		wxWindowUpdateLocker lock(mPanel);
		pane.Show(show);
		mAuiMgr->Update();
	}
}
//-----------------------------------------------------------------------------
void ViewHistory::ShowObjPropList(bool show) 
{
	auto& pane = mAuiMgr->GetPane("ViewObjPropListPane");
	if (pane.IsOk())
	{
		pane.Show(show);
		if (show)
		{
			wxDataViewEvent evt(wxEVT_DATAVIEW_SELECTION_CHANGED, mTable->GetId());

			//if (!mTable->HasSelection())
			//	Select(0);

			evt.SetItem(mTable->GetSelection());
			OnCmd_SelectHistoryItem(evt);
		}
		mToolbar->ToggleTool(wxID_PROPERTIES, show);
		wxWindowUpdateLocker lock(mPanel);
		mAuiMgr->Update();
		
	}
}
//-----------------------------------------------------------------------------
bool ViewHistory::IsShowObjPropList()const
{
	bool shown = false;
	auto& pane = mAuiMgr->GetPane("ViewObjPropListPane");
	if (pane.IsOk())
		shown = pane.IsShown();
	return shown;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistory::SetCfg(const rec::PageHistory& cfg) //override;
{
	ShowObjPropList(cfg.mShowPropertyList);
	ShowFilterList(cfg.mShowFilterList);

	SetRowsOffset(cfg.mRowsOffset);
	SetRowsLimit(cfg.mRowsLimit);
	
	mColAutosize = cfg.mColAutosize;

	auto dv = dynamic_cast<wxDataViewModelMediator*>(mTable->GetModel());
	if (!dv)
		return;

	dv->mPathInPropperties = cfg.mPathInProperties;

	if (cfg.mStringPerRow != dv->mStringPerRow)
	{
		dv->mStringPerRow = cfg.mStringPerRow;
		int ch = mTable->GetCharHeight();
		mTable->SetRowHeight(ch * dv->mStringPerRow + 2);
	}
	

}