#include "_pch.h"
#include "dlg_mkobj_view_Frame.h"
#include "BtnProperty.h"
#include "MObjCatalog.h"


using namespace wh;
using namespace wh::object_catalog::view;

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
	this->SetTitle("Создание объекта");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	auto mgr = ResMgr::GetInstance();
	this->SetIcon(wxIcon(mgr->m_ico_classprop16));
	mgr->FreeInst();
	
	mPropGrid = new wxPropertyGrid(this);
	szrMain->Add(mPropGrid, 1, wxALL | wxEXPAND, 1);

	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	mbtnOK = new wxButton(this, wxID_OK, "Сохранить и закрыть");
	mbtnCancel = new wxButton(this, wxID_CANCEL, "Отмена");
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	msdbSizer->Add(mbtnOK, 0, wxALL, 5);
	szrMain->Add(msdbSizer, 0, wxEXPAND, 10);

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnOk, this, wxID_OK);

	this->SetSizer(szrMain);
	this->Layout();


	auto pg_mainInfo = new wxPropertyCategory("Основные сведения");
	mPropGrid->Append(pg_mainInfo);

	pg_mainInfo->AppendChild(new wxStringProperty(L"Имя", wxPG_LABEL));
	pg_mainInfo->AppendChild(new wxStringProperty(L"Количество", wxPG_LABEL));
	pg_mainInfo->AppendChild(new wxStringProperty(L"ID", wxPG_LABEL))->Enable(false);

	auto pg_pathInfo = new wxPropertyCategory("Местоположение");
	mPropGrid->Append(pg_pathInfo);



	std::function<bool(wxPGProperty*)> selectProp =
		std::bind(&Frame::OnSelectPath, this, std::placeholders::_1);
	BtnProperty* btnProp = new BtnProperty("Путь");
	btnProp->SetOnClickButonFunc(selectProp);
	pg_pathInfo->AppendChild(btnProp);
	
	pg_pathInfo->AppendChild(new wxStringProperty(L"PID", wxPG_LABEL))->Enable(false);
	
	
}
//---------------------------------------------------------------------------
void Frame::SetModel(std::shared_ptr<object_catalog::MObjItem>& newModel)
{
	if (mObj != newModel)
	{
		mChangeConnection.disconnect();
		mObj = newModel;
		if (mObj)
		{
			auto funcOnChange = std::bind(&Frame::OnChangeModel,
				this, std::placeholders::_1);
			mChangeConnection = mObj->ConnectChangeDataSlot(funcOnChange);
			OnChangeModel(*mObj.get());
		}//if (mModel)
	}//if

}
//---------------------------------------------------------------------------
void Frame::OnChangeModel(const IModel& model)
{
	if (mObj && mObj.get() == &model)
	{
		const auto state = model.GetState();
		const auto& data = mObj->GetData();
		SetData(data);
	}
}
//---------------------------------------------------------------------------
void Frame::GetData(object_catalog::MObjItem::T_Data& data) const
{
	mPropGrid->CommitChangesFromEditor();

	// значение свойства получается из диалоа
	data.mLabel = mPropGrid->GetPropertyByLabel("Имя")->GetValueAsString();
	data.mQty = mPropGrid->GetPropertyByLabel("Количество")->GetValueAsString();
	//data.mObj.??? = mPropGrid->GetPropertyByLabel("Путь")->GetValueAsString();
	data.mID = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
	data.mPID = mPropGrid->GetPropertyByLabel("PID")->GetValueAsString();
	

}
//---------------------------------------------------------------------------
void Frame::SetData(const object_catalog::MObjItem::T_Data& data)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(data.mLabel);
	mPropGrid->GetPropertyByLabel(L"Количество")->SetValueFromString(data.mQty);
	
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(data.mID);
	mPropGrid->GetPropertyByLabel(L"PID")->SetValueFromString(data.mPID);

	auto catalog = dynamic_cast<object_catalog::MObjCatalog*>(mObj->GetParent());
	if (catalog)
	{
		mPropGrid->GetPropertyByLabel(L"Путь")->SetValueFromString(catalog->mPath->GetPathStr());
	}

}
//---------------------------------------------------------------------------
bool Frame::OnSelectPath(wxPGProperty* prop)
{

	return false;
}
//---------------------------------------------------------------------------
/*
void Frame::OnClose(wxCloseEvent& evt)
{

}
//---------------------------------------------------------------------------
void Frame::OnCancel(wxCommandEvent& evt )
{

}
*/
//---------------------------------------------------------------------------
void Frame::OnOk(wxCommandEvent& evt )
{
	mObj->Save();
}
