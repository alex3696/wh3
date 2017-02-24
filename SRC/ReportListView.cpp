#include "_pch.h"
#include "ReportListView.h"

#include "globaldata.h"
#include "config.h"

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
		return mReportList? mReportList->size() : 0;
	}

	virtual void  GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const override
	{
		if (!mReportList || mReportList->size() <= row)
			return;

		switch (col)
		{
		case 1: variant = mReportList->at(row)->mId; break;
		case 2: variant = mReportList->at(row)->mTitle; break;
		case 3: variant = mReportList->at(row)->mNote; break;
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
		Reset(mReportList->size());
	}

	const wxString& GetRepId(const wxDataViewItem& item)const
	{
		if (!mReportList)
			return wxEmptyString2;
		auto row = GetRow(item);
		return mReportList->at(row)->mId;
	}

	wxDataViewItem GetDataViewItem(const wxString& rep_id)const
	{
		if (!mReportList)
			return wxDataViewItem();
		
		const auto& idxRepId = mReportList->get<1>();
		const auto& itRepId = idxRepId.find(rep_id);
		if (idxRepId.end() != itRepId)
		{
			const auto rndIt = mReportList->project<0>(itRepId);
			auto pos = std::distance(mReportList->begin(), rndIt);
			return GetItem(pos);
		}
		return wxDataViewItem();
	}

	wxDataViewItem RmItem(const std::shared_ptr<const rec::ReportItem>& ri)
	{
		auto dvitem = GetDataViewItem(ri->mId);
		if (dvitem.IsOk())
		{
			auto row = GetRow(dvitem);
			RowDeleted(row);
			if (1 == mReportList->size())
			{
				dvitem = wxDataViewItem();
			}
			else
			{
				while (row>0 && row >= mReportList->size() - 1)
					row--;
				dvitem = GetItem(row);
			}
		}
		return dvitem;
	}
	void ChItem(const std::shared_ptr<const rec::ReportItem>& ri)
	{
		auto dvitem = GetDataViewItem(ri->mId);
		if (dvitem)
			RowChanged(GetRow(dvitem));
	}


	wxDataViewItem DoMkItem(const std::shared_ptr<const rec::ReportItem>& ri)
	{
		this->RowAppended();
		return GetDataViewItem(ri->mId);
	}

};

//-----------------------------------------------------------------------------
ReportListView::ReportListView(std::shared_ptr<IViewNotebook> wnd)
	:IReportListView()
{
	mPanel = new wxPanel(wnd->GetWnd());
	
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	
	auto tool_bar = BuildToolBar(mPanel);
	szrMain->Add(tool_bar, 0, wxALL | wxEXPAND, 0);
		
	mTable = BuildReportList(mPanel);
	szrMain->Add(mTable, 1, wxALL | wxEXPAND, 0);

	mPanel->SetSizer(szrMain);
	mPanel->Layout();

	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportListView::OnCmd_UpdateList, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportListView::OnCmd_ExecReport, this, wxID_EXECUTE);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportListView::OnCmd_MkReport, this, wxID_ADD);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportListView::OnCmd_RmReport, this, wxID_REMOVE);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ReportListView::OnCmd_ChReport, this, wxID_EDIT);

	mTable->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &ReportListView::OnActivated, this);
}
//-----------------------------------------------------------------------------
wxWindow* ReportListView::GetWnd() const
{ 
	return mPanel; 
}
//-----------------------------------------------------------------------------

wxAuiToolBar* ReportListView::BuildToolBar(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT | wxAUI_TB_OVERFLOW);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24);
	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24);

	
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)currBaseGroup >= (int)bgTypeDesigner)
	{
		tool_bar->AddSeparator();
		tool_bar->AddTool(wxID_ADD, "Создать", mgr->m_ico_create24);
		tool_bar->AddTool(wxID_EDIT, "Редактировать", mgr->m_ico_edit24);
		tool_bar->AddTool(wxID_REMOVE, "Удалить", mgr->m_ico_delete24);
	}

	tool_bar->Realize();

	return tool_bar;

	
}
//-----------------------------------------------------------------------------

wxDataViewCtrl* ReportListView::BuildReportList(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(mPanel, wxID_ANY);
	
	auto dv_model = new ReportListDv();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	auto col_id = table->AppendTextColumn("ID", 1, wxDATAVIEW_CELL_INERT, 50);
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)currBaseGroup < (int)bgAdmin)
		col_id->SetHidden(true);
	auto col_name = table->AppendTextColumn("Имя", 2, wxDATAVIEW_CELL_INERT, 200,
		wxALIGN_NOT, /*wxDATAVIEW_COL_SORTABLE |*/ wxDATAVIEW_COL_RESIZABLE);
	//col_name->SetSortOrder(1);
	table->AppendTextColumn("Описание", 3, wxDATAVIEW_CELL_INERT);


	int ch = table->GetCharHeight();
	table->SetRowHeight(ch * 2 + 2);

	return table;
}
//-----------------------------------------------------------------------------

void ReportListView::SetReportList(const rec::ReportList& rl)
{
	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	auto sel_item = mTable->GetSelection();
	wxString rep_id;
	if (sel_item.IsOk())
		rep_id = dv->GetRepId(sel_item);

	dv->SetReportList(rl);

	sel_item = dv->GetDataViewItem(rep_id);

	if(sel_item.IsOk())
		mTable->Select(sel_item);
}
//---------------------------------------------------------------------------
void ReportListView::OnMkReport(const std::shared_ptr<const rec::ReportItem>& ri)
{
	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	wxDataViewItem sel_item = dv->DoMkItem(ri);
	if (sel_item.IsOk())
		mTable->Select(sel_item);
}
//---------------------------------------------------------------------------
void ReportListView::OnRmReport(const std::shared_ptr<const rec::ReportItem>& ri)
{
	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	//sigUpdateList();
	wxDataViewItem sel_item = dv->RmItem(ri);
	if (sel_item.IsOk())
		mTable->Select(sel_item);
	
}
//---------------------------------------------------------------------------
void ReportListView::OnChReport(const std::shared_ptr<const rec::ReportItem>& ri, const wxString& old_rep_id)
{
	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	dv->ChItem(ri);
	//sigUpdateList();
}
//-----------------------------------------------------------------------------

void ReportListView::OnCmd_UpdateList(wxCommandEvent& evt)
{
	sigUpdateList();
}
//-----------------------------------------------------------------------------
void ReportListView::OnCmd_ExecReport(wxCommandEvent& evt)
{
	auto sel_item = mTable->GetSelection();
	if (!sel_item.IsOk())
		return;

	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	sigExecReport(dv->GetRepId(sel_item));
}
//-----------------------------------------------------------------------------
void ReportListView::OnCmd_MkReport(wxCommandEvent& evt)
{
	sigMkReport();
}
//-----------------------------------------------------------------------------
void ReportListView::OnCmd_RmReport(wxCommandEvent& evt)
{
	auto sel_item = mTable->GetSelection();
	if (!sel_item.IsOk())
		return;
	
	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	sigRmReport(dv->GetRepId(sel_item));
}
//-----------------------------------------------------------------------------
void ReportListView::OnCmd_ChReport(wxCommandEvent& evt)
{
	auto sel_item = mTable->GetSelection();
	if (!sel_item.IsOk())
		return;

	auto dv = dynamic_cast<ReportListDv*>(mTable->GetModel());
	sigChReport(dv->GetRepId(sel_item));

}
//---------------------------------------------------------------------------
void ReportListView::OnActivated(wxDataViewEvent& evt)
{
	OnCmd_ExecReport(wxCommandEvent());
}