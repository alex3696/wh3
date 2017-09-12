#include "_pch.h"
#include "ViewUndo.h"
#include "globaldata.h"
#include "whPGFileLinkProperty.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewUndoWindow::ViewUndoWindow(wxWindow* parent)
{
	mPanel = new wxDialog(parent, wxID_ANY, "Отмена действия"
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* szr_info = new wxBoxSizer(wxHORIZONTAL);
	auto ctrl_bmp = new wxStaticBitmap(mPanel, wxID_ANY, wxArtProvider::GetBitmap(wxART_WARNING, wxART_CMN_DIALOG), wxDefaultPosition, wxDefaultSize, 0);
	szr_info->Add(ctrl_bmp, 0, wxALL, 15);

	//auto ctrl_txt = new wxTextCtrl(mPanel, wxID_ANY
	auto ctrl_txt = new wxStaticText(mPanel, wxID_ANY
		, "Откат действия приведёт к безвозвратному удалению указанных данных."
		" Откатить можно только 'свои' действия за последние 7 суток, при условии"
		", что после вас никто не выполнял другие действия."
		);
		//, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_NO_VSCROLL | wxTE_READONLY | wxTE_WORDWRAP);
	ctrl_txt->Wrap(500);
	szr_info->Add(ctrl_txt, 1, wxALL | wxEXPAND, 5);
	mainSz->Add(szr_info, 0, wxEXPAND, 5);



	int style = wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION;
	int extraStyle = wxPG_EX_HELP_AS_TOOLTIPS;
	mPG = new wxPropertyGrid(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	//mPG->CenterSplitter(true);
	mPG->SetExtraStyle(extraStyle);
	mainSz->Add(mPG, 1, wxALL | wxEXPAND, 0);

	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	auto mbtnOK = new wxButton(mPanel, wxID_OK, "Выполнить");
	auto mbtnCancel = new wxButton(mPanel, wxID_CANCEL, "Закрыть");
	msdbSizer->Add(mbtnOK, 0, wxALL, 5);
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	mainSz->Add(msdbSizer, 0, wxEXPAND, 10);

	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewUndoWindow::OnCmd_ExecuteUndo, this, wxID_OK);
	

	mPG->SetMinSize(wxSize(500, mPG->GetRowHeight() * 10));

	mPG->FitColumns();
	mainSz->SetSizeHints(mPanel);
	mPanel->SetSizer(mainSz);
	mPanel->Layout();
	mPanel->Centre(wxBOTH);
	
}
//-----------------------------------------------------------------------------
ViewUndoWindow::ViewUndoWindow(std::shared_ptr<IViewWindow> parent)
	: ViewUndoWindow(parent->GetWnd())
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewUndoWindow::OnShow() //override
{
	mPanel->ShowModal();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewUndoWindow::OnUpdateTitle(const wxString& str, const wxIcon& ico)//override;
{
	mPanel->SetTitle(str);
	mPanel->SetIcon(ico);
}

//-----------------------------------------------------------------------------
//virtual 
void ViewUndoWindow::SetHistoryRecord(const ModelHistoryRecord& rec) //override;
{
	mPG->Clear();
	
	auto dt = new wxStringProperty("Дата Время", wxPG_LABEL, rec.GetDate());
	auto usr = new wxStringProperty("Пользователь", wxPG_LABEL, rec.GetUser());
	auto log_id = new wxStringProperty("#", wxPG_LABEL, rec.GetLogId());
	dt->SetFlagRecursively(wxPG_PROP_READONLY, true);
	usr->SetFlagRecursively(wxPG_PROP_READONLY, true);
	log_id->SetFlagRecursively(wxPG_PROP_READONLY, true);
	mPG->Append(dt);
	mPG->Append(usr);
	mPG->Append(log_id);

	auto cls = new wxStringProperty("Тип", wxPG_LABEL, rec.GetObj().GetCls().GetTitle() );
	auto obj = new wxStringProperty("Объект", wxPG_LABEL, rec.GetObj().GetTitle());
	cls->SetFlagRecursively(wxPG_PROP_READONLY, true);
	obj->SetFlagRecursively(wxPG_PROP_READONLY, true);
	mPG->Append(cls);
	mPG->Append(obj);

	if (rec.GetAct().GetId() == "0" || rec.GetAct().GetId().IsEmpty()) // move
	{
		
		auto cat = new wxPropertyCategory("Перемещение");
		auto src = new wxLongStringProperty("Источник", wxPG_LABEL, rec.GetPath().AsString()); 
		auto dst = new wxLongStringProperty("Приёмник", wxPG_LABEL, rec.GetDstPath().AsString());
		auto qty = new wxLongStringProperty("Количество", wxPG_LABEL, rec.GetQty());
		mPG->Append(cat);
		mPG->AppendIn(cat, dst);
		mPG->AppendIn(cat, src);
		mPG->AppendIn(cat, qty);
		cat->SetFlagRecursively(wxPG_PROP_READONLY, true);

		
	}
	else //if (rec.GetAct().GetId().IsEmpty()) // act
	{
		const auto& act = rec.GetAct();
		auto cat = new wxPropertyCategory(act.GetTitle());
		mPG->Append(cat);

		for (const auto& curr : rec.GetActProperties())
		{
			wxPGProperty* pgp = nullptr;
			const wxString& pgp_title = curr->GetProp().GetTitle();
			const wxString  pgp_name = wxString::Format("ObjProp_%s", curr->GetProp().GetId());

			switch (curr->GetProp().GetType())
			{
			case ftText:	pgp = new wxLongStringProperty(pgp_title, pgp_name); break;
			case ftName:	pgp = new wxStringProperty(pgp_title, pgp_name); break;
			case ftLong:
				pgp = new wxStringProperty(pgp_title, pgp_name);
				pgp->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
				break;
			case ftDouble:	pgp = new wxFloatProperty(pgp_title, pgp_name);  break;
			case ftDate:	pgp = new wxStringProperty(pgp_title, pgp_name);  break;
			case ftLink:	pgp = new whPGFileLinkProperty(pgp_title, pgp_name);  break;
			case ftFile:	pgp = new wxStringProperty(pgp_title, pgp_name);  break;
			case ftJSON:	pgp = new wxLongStringProperty(pgp_title, pgp_name);  break;
			default:break;
			}
			pgp->SetValueFromString(curr->GetValue());
			mPG->AppendIn(cat, pgp);

		}//for

		cat->SetFlagRecursively(wxPG_PROP_READONLY, true);
		wxColour clr = act.GetColour().IsEmpty() ? *wxWHITE : act.GetColour();
		cat->SetBackgroundColour(clr);


	}
	mPG->FitColumns();

	
}
//-----------------------------------------------------------------------------
void ViewUndoWindow::OnCmd_Load(wxCommandEvent& evt)
{
	sigLoad();
}
//-----------------------------------------------------------------------------
void ViewUndoWindow::OnCmd_ExecuteUndo(wxCommandEvent& evt)
{
	sigExecuteUndo();
	mPanel->EndModal(wxID_OK);
}

