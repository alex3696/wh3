#include "_pch.h"
#include "dlg_act_view_Frame.h"


using namespace wh;
using namespace wh::dlg_act::view;

//---------------------------------------------------------------------------
Frame::Frame(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	//: wh::view::DlgBaseOkCancel(parent, id, title, pos, size, style, name)
	: wxDialog(parent, id, title, pos, size, style, name)
{
	this->SetTitle("Выполнение действия");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBitmap* actBmp = new wxStaticBitmap(this, wxID_ANY,
		wxBitmap("..\\..\\RESOURCES\\act.ico", wxBITMAP_TYPE_ANY),
		wxDefaultPosition, wxDefaultSize, 0);
	titleSizer->Add(actBmp, 0, wxALL, 5);
	mCaptionTxt = new wxStaticText(this, wxID_ANY, "Выберите действие", 
		wxDefaultPosition, wxDefaultSize, 0);
	mCaptionTxt->Wrap(-1);
	titleSizer->Add(mCaptionTxt, 1, wxALL | wxEXPAND, 5);
	szrMain->Add(titleSizer, 0, wxEXPAND, 5);

	mPnlActArray = new PnlActArray(this);
	szrMain->Add(mPnlActArray, 1, wxALL | wxEXPAND, 5);

	mPnlPropGrid = new PnlPropGrid(this);
	szrMain->Add(mPnlPropGrid, 1, wxALL | wxEXPAND, 1);
	
	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	auto mbtnCancel = new wxButton(this, wxID_CANCEL);//," Закрыть" );
	mbtnNextOK = new wxButton(this, wxID_FORWARD,"Далее");
	mbtnPrevios = new wxButton(this, wxID_BACKWARD,"Назад");
	msdbSizer->Add(mbtnPrevios, 0, wxALL, 5);
	msdbSizer->Add(mbtnNextOK, 0, wxALL, 5);
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	szrMain->Add(msdbSizer, 0,  wxEXPAND, 10);
	

	this->SetSizer(szrMain);
	this->Layout();

	Bind(wxEVT_CLOSE_WINDOW, &Frame::OnClose, this);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnCancel, this, wxID_CANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnBackward, this, wxID_BACKWARD);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnForward, this, wxID_FORWARD);
	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &Frame::OnActivated, this);

	mbtnPrevios->Hide();
	mPnlPropGrid->Hide();
}

//-----------------------------------------------------------------------------
void Frame::SetModel(std::shared_ptr<dlg_act::model::Obj>& model)
{
	mActObj = model;
	if (mActObj)
	{
		mPnlActArray->SetModel(mActObj->mActArray);
		mActObj->mActArray->Load();

		if(!mActObj->mActArray->GetChildQty())
			mbtnNextOK->Enable(false);
	}//if (mActObj)
}
//-----------------------------------------------------------------------------

void Frame::OnClose(wxCloseEvent& evt)
{
	OnCancel();
}
//-----------------------------------------------------------------------------

void Frame::OnCancel(wxCommandEvent& evt)
{
	try
	{
		mActObj->Unlock();
		EndModal(wxID_CANCEL);
	}
	catch (...)
	{
		wxLogError("error mActObj->Unlock() ");
	}
	EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------

void Frame::OnBackward(wxCommandEvent& evt)
{
	if (mPnlPropGrid->IsShown())
	{
		mPnlActArray->Show();
		mPnlPropGrid->Hide();
		mbtnPrevios->Hide();
		mbtnNextOK->SetLabel("Далее");
		mCaptionTxt->SetLabel("Выберите действие");
		mSelectedAct.reset();
		this->Layout();
	}
	

}
//-----------------------------------------------------------------------------
void Frame::OnForward(wxCommandEvent& evt)
{
	if (mPnlActArray->IsShown())
	{
		mPnlActArray->Hide();
		mPnlPropGrid->Show();
		mbtnPrevios->Show();
		mbtnNextOK->SetLabel("Готово");
		this->Layout();

		std::vector<unsigned int> selected;
		mPnlActArray->GetSelected(selected);
		if (selected.size())
		{
			unsigned int pos = selected[0];
			if (mActObj && mActObj->mActArray && mActObj->mActArray->GetChildQty()>pos)
			{
				mSelectedAct = std::dynamic_pointer_cast<model::Act>
					(mActObj->mActArray->GetChild(pos));
				if (mSelectedAct)
				{
					mSelectedAct->Load();
					const wxString str =
						wxString::Format("Введите данные для выполнения '%s' c '%s %s'"
						, mSelectedAct->GetData().mLabel
						, mActObj->GetData().mCls.mLabel.toStr()
						, mActObj->GetData().mObj.mLabel.toStr() );
					mCaptionTxt->SetLabel(str);
					mPnlPropGrid->SetModel(mSelectedAct->mPropArray);
				}
			}
		}

	}
	else if (mPnlPropGrid->IsShown())
	{
		auto act_model = dynamic_cast<model::Act*>(mPnlPropGrid->GetModel()->GetParent());

		bool hasEmptyFields = mPnlPropGrid->UpdateModel();

		int modalResult = wxID_YES;
		if (hasEmptyFields)
		{
			wxMessageDialog dialog(this,
				"Не все поля заполнены! Вы уверены, в том что надо сохранить?",
				"Confirm\n",
				wxCENTER |
				wxNO_DEFAULT | wxYES_NO | wxICON_QUESTION);
			modalResult = dialog.ShowModal();
		}

		if (wxID_YES == modalResult)
		{
			try
			{
				act_model->DoAct();
				EndModal(wxID_OK);
			}
			catch (...)
			{
				wxLogError("error act_model->DoAct()");
			}
		}

	}
}
//-----------------------------------------------------------------------------
void Frame::OnActivated(wxDataViewEvent &evt)
{
	OnForward();
}