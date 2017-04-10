#include "_pch.h"
#include "ViewHistory.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"

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
		switch (col)
		{
		case 3:
		{
			const auto& act = mTD->GetActRec(row);
			wxColour clr(act.mColour);
			if (!act.mId.IsEmpty() && *wxWHITE != clr)
			{
				attr.SetBackgroundColour(clr);
				return true;
			}
			break;
		}
		default:break;
		}
		return false;
	}

	virtual void  GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const override
	{
		if (!mTD || mTD->size() <= row)
			return;
		wxString		str;
		const wxIcon*	ico = &wxNullIcon;
		wxIcon			icoHolder;

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

				str = wxString::Format("%s\n%s\n\n%s"
					, dt.Format(format_d)
					, dt.Format(format_t)
					, mTD->GetUser(row));
			}
			break;
		case 2: str = wxString::Format("%s\n%s"
			, mTD->GetCTiltle(row), mTD->GetOTiltle(row)
			);
			break;
		case 3:
		{
			const auto& act = mTD->GetActRec(row);
			if (act.mId.IsEmpty())
			{
				str = wxString::Format("Перемещение: %s (%s)\n%s\n%s\n"
					, mTD->GetQty(row), mTD->GetCMeasure(row)
					, mTD->GetDstPath(row), mTD->GetSrcPath(row)
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
						,p->mProp->mTitle, p->mVal);
				}
				
				str = wxString::Format("%s:\n%sМестонахождение: %s"
					, act.mTitle
					, prop_str
					, mTD->GetSrcPath(row));
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
		//.PaneBorder(false)
		.Gripper(false)
		);

	mTable = BuildTable(mPanel);
	panel->mAuiMgr.AddPane(mTable, wxAuiPaneInfo().
		Name(wxT("ReportTable")).CenterPane()
		//.PaneBorder(false)
		);

	panel->mAuiMgr.Update();
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Backward, this, wxID_BACKWARD);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Forward, this, wxID_FORWARD);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewHistory::OnCmd_Filter, this, wxID_SETUP);

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
	tool_bar->AddTool(wxID_SETUP, "Фильтр", mgr->m_ico_filter24);
	tool_bar->AddTool(wxID_SETUP, "Настройки", wxArtProvider::GetIcon(wxART_HELP_PAGE, wxART_TOOLBAR));



	tool_bar->Realize();

	return tool_bar;


}
//-----------------------------------------------------------------------------

wxDataViewCtrl* ViewHistory::BuildTable(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);
	auto dv_model = new wxDataViewModelMediator();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	int ch = table->GetCharHeight();
	table->SetRowHeight(ch * 5 + 2);

	auto renderer1 = new wxDataViewIconMLTextRenderer();
	renderer1->SetAlignment(wxALIGN_TOP);
	auto attr = renderer1->GetAttr();
	attr.SetColour(*wxBLACK);
	renderer1->SetAttr(attr);

	auto renderer2 = new wxDataViewIconMLTextRenderer();
	renderer2->SetAlignment(wxALIGN_TOP);
	auto renderer3 = new wxDataViewIconMLTextRenderer();
	renderer3->SetAlignment(wxALIGN_TOP);

	auto col1 = new wxDataViewColumn("Время Пользователь"
		, renderer1, 1, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col1);
	auto col2 = new wxDataViewColumn("Тип Объект"
		, renderer2, 2, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col2);
	auto col3 = new wxDataViewColumn("Действие Свойства Местоположение"
		, renderer3, 3, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col3);
	
	//table->SetCanFocus(false);

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

	//for (size_t i = 0; i < mTable->GetColumnCount(); i++)
	//	mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	
	mTable->EnsureVisible(dv->GetItem(0));
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
	sigFilter();
	OnCmd_Update();
}
//-----------------------------------------------------------------------------
//IViewWindow virtual 
void ViewHistory::OnShow()//override 
{
	//OnCmd_Update(); 
}
