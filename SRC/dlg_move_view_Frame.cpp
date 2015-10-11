#include "_pch.h"
#include "dlg_move_view_Frame.h"


using namespace wh;
using namespace wh::dlg_move::view;

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
	this->SetTitle("Перемещение");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	
	mLblMovableObj = new wxStaticText(this, wxID_ANY, "[класс]объект/...[класс]объект(ед.изм)");
	szrMain->Add(mLblMovableObj, 0, wxALL, 5);

	mqtySpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxSP_ARROW_KEYS, 1, MAXINT, 1);
	mqtyCtrl = new wxTextCtrl(this, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, 0,
		wxTextValidator(wxFILTER_NUMERIC));
	szrMain->Add(mqtySpin, 0, wxALL | wxEXPAND, 0);
	szrMain->Add(mqtyCtrl, 0, wxALL | wxEXPAND, 0);

	mLblDstObj = new wxStaticText(this, wxID_ANY, "В");
	szrMain->Add(mLblDstObj, 0, wxALL, 5);

	mCtrlPanel = new CtrlPanel(this);
	szrMain->Add(mCtrlPanel, 1, wxALL | wxEXPAND, 0);

	msdbSizer = new wxStdDialogButtonSizer();
	mbtnOK = new wxButton(this, wxID_OK);//,"Сохранить и закрыть" );
	msdbSizer->AddButton(mbtnOK);
	mbtnCancel = new wxButton(this, wxID_CANCEL);//," Закрыть" );
	msdbSizer->AddButton(mbtnCancel);
	msdbSizer->Realize();
	szrMain->Add(msdbSizer, 0, wxALL | wxEXPAND, 10);



	this->SetSizer(szrMain);
	this->Layout();

	Bind(wxEVT_CLOSE_WINDOW, &Frame::OnClose, this);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnCancel, this, wxID_CANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnOk, this, wxID_OK);

	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &Frame::OnActivated, this);
}
//-----------------------------------------------------------------------------
void Frame::SetModel(std::shared_ptr<IModel>& model)
{
	auto movable = std::dynamic_pointer_cast<model::MovableObj>(model);
	SetModel(movable);
}
//-----------------------------------------------------------------------------
void Frame::SetModel(std::shared_ptr<dlg_move::model::MovableObj>& model)
{
	mMovable = model;
	if (mMovable)
	{
		mCtrlPanel->SetModel(model);

		const wxString movLabel = wxString::Format("[%s]%s"
			, mMovable->GetData().mCls.mLabel.toStr()
			,mMovable->GetData().mObj.mLabel
			//,mMovable->GetData().mCls.mMeasure
			);

		mLblMovableObj->SetLabel(movLabel);

		ClsType clstype;
		if (mMovable->GetData().mCls.GetClsType(clstype))
		{
			switch (clstype)
			{
			case wh::ctQtyByOne:
				mqtySpin->Show();
				mqtyCtrl->Hide();
				break;
			case wh::ctQtyByFloat:
				mqtySpin->Hide();
				mqtyCtrl->Show();
				break;
			default: 
				mqtySpin->Hide();
				mqtyCtrl->Hide();
				break;
			}//switch
		}

	}//if (mMovable)
}
//-----------------------------------------------------------------------------

void Frame::OnClose(wxCloseEvent& evt)
{
	OnCancel();
}
//-----------------------------------------------------------------------------

void Frame::OnCancel(wxCommandEvent& evt)
{
	mMovable->Unlock();
	EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------
void Frame::OnOk(wxCommandEvent& evt)
{
	auto selected = mCtrlPanel->GetSelected();
	if (selected)
	{
		ClsType clstype;
		if (mMovable->GetData().mCls.GetClsType(clstype))
		{
			wxString qty;
			bool isQtyOk = false;
			unsigned long long_tmp;
			double double_tmp;

			switch (clstype)
			{
			case wh::ctSingle:
				isQtyOk = true;
				qty = "1";
				break;
			case wh::ctQtyByOne:
				qty = wxString::Format("%d",mqtySpin->GetValue());
				if (qty.ToULong(&long_tmp))
					isQtyOk = true;
				break;
			case wh::ctQtyByFloat:
				qty = mqtyCtrl->GetValue();
				if (qty.ToCDouble(&double_tmp))
					isQtyOk = true;
				break;
			default: //wh::ctAbstract:
				break;
			}

			if (isQtyOk)
			{
				mMovable->Move(selected, qty);
				EndModal(wxID_OK);
			}
			
			
		}// if (mMovable->GetData().mCls.GetClsType(clstype))
	}// if (selected)
}
//---------------------------------------------------------------------------
void Frame::OnActivated(wxDataViewEvent& evt)
{
	OnOk();
}
