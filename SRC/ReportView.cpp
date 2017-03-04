#include "_pch.h"
#include "ReportView.h"
#include "globaldata.h"

using namespace wh;

class ReportDv
	: public wxDataViewIndexListModel
{
	rec::ReportTable mReportTable;
public:
	ReportDv(){}
	//virtual bool  GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const override	{}
	//virtual bool  IsEnabledByRow(unsigned int row, unsigned int col) const override {}

	virtual unsigned int  GetCount() const override
	{
		return mReportTable.mRowList.size();
	}

	virtual void  GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const override
	{
		if (mReportTable.mRowList.size() <= row)
			return;
		variant = mReportTable.mRowList.at(row).at(col); 
	}

	virtual bool  SetValueByRow(const wxVariant &variant, unsigned int row, unsigned int col)
	{
		return false;
	}

	virtual unsigned int	GetColumnCount() const override
	{
		return mReportTable.mColNames.size();
	}
	virtual wxString		GetColumnType(unsigned int col) const override
	{
		return "string";
	}

	void SetReportList(const rec::ReportTable& rt)
	{
		mReportTable = rt;
		Reset(rt.mRowList.size());
	}


};




//-----------------------------------------------------------------------------
ReportView::ReportView(std::shared_ptr<IViewWindow> parent)
	:IReportView()
{
	auto panel = new wxAuiPanel(parent->GetWnd());
	mPanel = panel;
	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
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

	mNote = new wxStaticText(mPanel, wxID_ANY, "Описание \n Описание");
	mNote->Wrap(-1);
	//mNote->SetBackgroundColour(*wxRED);
	panel->mAuiMgr.AddPane(mNote, wxAuiPaneInfo().
		Name(wxT("NoteText")).Caption("Описание")
		//.CaptionVisible(false)
		.ToolbarPane()
		.Top()
		//.Left()
		//.Fixed()
		//.Resizable(true)
		//.Dockable(false)
		.PaneBorder(false)
		.Gripper(false)
		.CloseButton(false)
		);



	mTable = BuildReportTable(mPanel);
	panel->mAuiMgr.AddPane(mTable, wxAuiPaneInfo().
		Name(wxT("ReportTable")).CenterPane().PaneBorder(false));

	mPG = new wxPropertyGrid(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_SPLITTER_AUTO_CENTER);
	panel->mAuiMgr.AddPane(mPG, wxAuiPaneInfo().
		Name(wxT("ReportPG")).Caption("Параметры")
		.Left().PaneBorder(false).MinSize(250,100).CloseButton(false));
	mPG->CenterSplitter(true); 

	//mPanel->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&)
	//{
	//	sigClose();
	//	mPanel->Destroy();
	//	mPanel = nullptr;
	//});

	panel->mAuiMgr.Update();
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Execute, this, wxID_EXECUTE);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Export, this, wxID_CONVERT);

}
//-----------------------------------------------------------------------------
ReportView::~ReportView()
{

}

//-----------------------------------------------------------------------------
wxWindow* ReportView::GetWnd()const
{
	return mPanel;
}
//-----------------------------------------------------------------------------

wxAuiToolBar* ReportView::BuildToolBar(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT /*| wxAUI_TB_OVERFLOW*/);
	

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24);
	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24);

	tool_bar->AddTool(wxID_CONVERT, "Экспорт в Excel", mgr->m_ico_export_excel24);

	tool_bar->Realize();

	return tool_bar;


}
//-----------------------------------------------------------------------------

wxDataViewCtrl* ReportView::BuildReportTable(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(mPanel, wxID_ANY,wxDefaultPosition,wxDefaultSize
		,wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);
	auto dv_model = new ReportDv();
	table->AssociateModel(dv_model);
	dv_model->DecRef();
	return table;
}
//-----------------------------------------------------------------------------
void ReportView::SetReportTable(const rec::ReportTable& rt)
{
	auto dv = dynamic_cast<ReportDv*>(mTable->GetModel());
	
	mTable->ClearColumns();

	for (size_t i = 0; i < rt.mColNames.size(); i++)
		auto col_id = mTable->AppendTextColumn(rt.mColNames[i], i, wxDATAVIEW_CELL_INERT
			, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);

	dv->SetReportList(rt);

	for (size_t i = 0; i < mTable->GetColumnCount(); i++)
		mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));

}
//-----------------------------------------------------------------------------
void ReportView::SetFilterTable(const rec::ReportFilterTable& ft)
{
	mPG->Clear();
	for (const auto& p : ft)
	{
		if (0 == p.mType.CmpNoCase("STRING"))
			mPG->Append(new wxStringProperty(p.mName,wxPG_LABEL,p.mDefault));
		else if (0 == p.mType.CmpNoCase("DATE"))
		{
			auto pgp = new wxDateProperty(p.mName);
			if (p.mDefault.IsEmpty())
				pgp->SetAttribute(wxPG_DATE_PICKER_STYLE, (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY |	wxDP_ALLOWNONE));
			else
				pgp->SetAttribute(wxPG_DATE_PICKER_STYLE, (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY ));
			wxString format = wxLocale::GetOSInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
			pgp->SetAttribute(wxPG_DATE_FORMAT, format);

			wxDateTime dt;
			if (dt.ParseDate(p.mDefault))
				pgp->SetDateValue(dt);
			mPG->Append(pgp);
		}
			
	}
	mPG->CenterSplitter(true);
	

}
//-----------------------------------------------------------------------------
void ReportView::SetNote(const wxString& note )
{
	mNote->SetLabel(note);

	auto bs = mNote->GetBestSize();
	mNote->SetSize(bs);

	wxAuiPaneInfo&  pi = mPanel->mAuiMgr.GetPane(mNote);
	if (pi.IsOk())
	{
		pi.BestSize(bs);
	}
		
	mPanel->mAuiMgr.Update();
}

//-----------------------------------------------------------------------------

void ReportView::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	
	sigUpdate();
}
//-----------------------------------------------------------------------------
void ReportView::OnCmd_Execute(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	mPG->CommitChangesFromEditor();

	std::vector<wxString> filter;
	auto pit = mPG->GetIterator(wxPG_ITERATE_ALL, nullptr);
	while (!pit.AtEnd())
	{
		auto pgp = pit.GetProperty();
		filter.emplace_back(pgp->GetValueAsString());
		pit.Next();
	}
	sigExecute(filter);
}
//-----------------------------------------------------------------------------

void ReportView::OnCmd_Export(wxCommandEvent& evt)
{
	sigExport();
}
//-----------------------------------------------------------------------------

