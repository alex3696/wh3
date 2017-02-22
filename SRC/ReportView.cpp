#include "_pch.h"
#include "ReportView.h"
#include "globaldata.h"

using namespace wh;

class ReportListDv
	: public wxDataViewIndexListModel
{
	const rec::ReportList* mReportList = nullptr;
public:
	ReportListDv(){}
	//virtual bool  GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const override	{}
	//virtual bool  IsEnabledByRow(unsigned int row, unsigned int col) const override {}

	virtual unsigned int  GetCount() const override
	{
		return mReportList ? mReportList->size() : 0;
	}

	virtual void  GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const override
	{
		if (!mReportList || mReportList->size() <= row)
			return;

		switch (col)
		{
		case 1: variant = mReportList->at(row).mId; break;
		case 2: variant = mReportList->at(row).mTitle; break;
		case 3: variant = mReportList->at(row).mNote; break;
		default:break;
		}
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
		return "string";
	}

	void SetReportList(const rec::ReportList& rl)
	{
		mReportList = &rl;
		Reset(rl.size());
	}


};

//-----------------------------------------------------------------------------
ReportView::ReportView(std::shared_ptr<wxWindow*> wnd)
	:IReportView()
{
	mPanel = new wxPanel(*wnd);
	mPanel->SetBackgroundColour(*wxRED);

	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Export, this, wxID_DUPLICATE);

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	auto tool_bar = BuildToolBar(mPanel);
	szrMain->Add(tool_bar, 0, wxALL | wxEXPAND, 0);

	mTable = BuildReportTable(mPanel);
	szrMain->Add(mTable, 1, wxALL | wxEXPAND, 0);

	mPanel->SetSizer(szrMain);
	mPanel->Layout();
}
//-----------------------------------------------------------------------------
wxWindow* ReportView::GetWnd()
{
	return mPanel;
}
//-----------------------------------------------------------------------------

wxAuiToolBar* ReportView::BuildToolBar(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT | wxAUI_TB_OVERFLOW);

	tool_bar->AddTool(wxID_REFRESH, "ќбновить", mgr->m_ico_refresh24);

	tool_bar->AddTool(wxID_ADD, "Ёкспорт в Excel", mgr->m_ico_back24);
	tool_bar->Realize();

	return tool_bar;


}
//-----------------------------------------------------------------------------

wxDataViewCtrl* ReportView::BuildReportTable(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(mPanel, wxID_ANY);

	auto dv_model = new ReportListDv();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	auto col_id = table->AppendTextColumn("ID", 1, wxDATAVIEW_CELL_INERT, 50);
	col_id->SetHidden(true);
	auto col_name = table->AppendTextColumn("»м€", 2, wxDATAVIEW_CELL_INERT, 200,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	//col_name->SetSortOrder(1);
	table->AppendTextColumn("ќписание", 3, wxDATAVIEW_CELL_INERT);


	int ch = table->GetCharHeight();
	table->SetRowHeight(ch * 2 + 2);

	return table;
}
//-----------------------------------------------------------------------------
void ReportView::SetReportTable(const rec::ReportTable& rt)
{
	//auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	//dv->SetReportList(rl);

}
//-----------------------------------------------------------------------------

void ReportView::OnCmd_Update(wxCommandEvent& evt)
{
	sigSetParam();
	sigUpdate();
}
//-----------------------------------------------------------------------------
void ReportView::OnCmd_Export(wxCommandEvent& evt)
{
	sigExport();
}
