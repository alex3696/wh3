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
	mPanel = new wxPanel(parent->GetWnd());

	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Execute, this, wxID_EXECUTE);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportView::OnCmd_Export, this, wxID_CONVERT);

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	auto tool_bar = BuildToolBar(mPanel);
	szrMain->Add(tool_bar, 0, wxALL | wxEXPAND, 0);

	mTable = BuildReportTable(mPanel);
	szrMain->Add(mTable, 1, wxALL | wxEXPAND, 0);

	mPanel->SetSizer(szrMain);
	mPanel->Layout();
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
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT | wxAUI_TB_OVERFLOW);

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

	for (size_t i = 0; i < rt.mColNames.size() ; i++)
		auto col_id = mTable->AppendTextColumn(rt.mColNames[i], i, wxDATAVIEW_CELL_INERT);
	
	dv->SetReportList(rt);

}
//-----------------------------------------------------------------------------
void ReportView::SetFilterTable(const rec::ReportFilterTable& ft)
{


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
	sigExecute();
}
//-----------------------------------------------------------------------------

void ReportView::OnCmd_Export(wxCommandEvent& evt)
{
	sigExport();
}
