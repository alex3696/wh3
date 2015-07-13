#include "_pch.h"
#include "dlg_mkobj_view_Frame.h"
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
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	auto mgr = ResMgr::GetInstance();
	this->SetIcon(wxIcon(mgr->m_icoObj24));
	mgr->FreeInst();
	
	mPropGrid = new wxPropertyGrid(this);
	szrMain->Add(mPropGrid, 1, wxALL | wxEXPAND, 1);

	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	mbtnOK = new wxButton(this, wxID_OK, "��������� � �������");
	mbtnCancel = new wxButton(this, wxID_CANCEL, "������");
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	msdbSizer->Add(mbtnOK, 0, wxALL, 5);
	szrMain->Add(msdbSizer, 0, wxEXPAND, 10);

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Frame::OnOk, this, wxID_OK);

	this->SetSizer(szrMain);
	this->Layout();


	auto pg_mainInfo = new wxPropertyCategory("�������� ��������");
	mPropGrid->Append(pg_mainInfo);

	pg_mainInfo->AppendChild(new wxStringProperty(L"���", wxPG_LABEL));
	mPGQty = new wxFloatProperty(L"����������", wxPG_LABEL);
	pg_mainInfo->AppendChild(mPGQty);
	pg_mainInfo->AppendChild(new wxStringProperty(L"ID", wxPG_LABEL))->Enable(false);

	auto pg_pathInfo = new wxPropertyCategory("��������������");
	mPropGrid->Append(pg_pathInfo);



	std::function<bool(wxPGProperty*)> selectProp =
		std::bind(&Frame::OnSelectPath, this, std::placeholders::_1);
	mPGPath = new BtnProperty("����");
	mPGPath->SetOnClickButonFunc(selectProp);
	pg_pathInfo->AppendChild(mPGPath);
	
	mPGPid = new wxStringProperty(L"PID", wxPG_LABEL);
	pg_pathInfo->AppendChild(mPGPid)->Enable(false);
	
	
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
	if (!mObj || mObj.get() != &model)
		return;
	auto obj_array = mObj->GetParent();
	if (!obj_array)
		return;
	auto cls_model = dynamic_cast<MTypeItem*>(obj_array->GetParent());
	if (!cls_model)
		return;
	auto cls_array = cls_model->GetParent();
	if (!cls_array)
		return;
	auto catalog = dynamic_cast<MObjCatalog*>(cls_array->GetParent());
	if (!catalog)
		return;


	const auto& cls_data = cls_model->GetData();
	const auto& obj_data = mObj->GetData();
	const auto  obj_state = model.GetState();
		
	SetData(obj_data);




	ClsType ct;
	if (cls_data.GetClsType(ct))
		switch (ct)
		{
		case ctSingle: 
			mPGQty->SetValueFromString("1"); 
			mPGQty->Enable(false);
			break;
		case ctQtyByOne:
		case ctQtyByFloat:
			mPGQty->SetValueFromString(obj_data.mQty);
			mPGQty->Enable(true);
		default:break;
		}
		
	switch (obj_state)
	{
	default:		SetTitle("**error**");			break;
	case msCreated: 
		SetTitle("�������� �������");
		
		if (catalog->mCfg->GetData().mObjCatalog)
		{
			mPGPath->SetValueFromString(mObj->GetPathString());
			mPGPid->SetValueFromString(catalog->GetData().mObj.mID);
		}
		else
		{
			rec::Obj tmp_obj_data;
			tmp_obj_data.mPID = cls_data.mDefaultPid;
			auto obj_model = std::make_shared<MObjItem>();
			obj_model->SetData(tmp_obj_data);

			auto opath = std::make_shared<object_catalog::model::ObjPath>();

			obj_model->AddChild( std::dynamic_pointer_cast<IModel>(opath));
			opath->Load();

			mPGPath->SetValueFromString(opath->AsString());
			mPGPid->SetValueFromString(cls_data.mDefaultPid);
		}
		
		mPGPath->Enable(true);
		break;
	case msExist:
	case msUpdated:	
		SetTitle("�������������� �������");
		mPGPath->SetValueFromString(mObj->GetPathString());
		mPGPid->SetValueFromString(obj_data.mPID);
		mPGPath->Enable(false);

			
		break;
	}
}
//---------------------------------------------------------------------------
void Frame::GetData(object_catalog::MObjItem::T_Data& data) const
{
	mPropGrid->CommitChangesFromEditor();

	// �������� �������� ���������� �� ������
	data.mLabel = mPropGrid->GetPropertyByLabel("���")->GetValueAsString();
	data.mQty = mPGQty->GetValueAsString();
	//data.mObj.??? = mPropGrid->GetPropertyByLabel("����")->GetValueAsString();
	data.mID = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
	data.mPID = mPropGrid->GetPropertyByLabel("PID")->GetValueAsString();
	

}
//---------------------------------------------------------------------------
void Frame::SetData(const object_catalog::MObjItem::T_Data& data)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"���")->SetValueFromString(data.mLabel);
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(data.mID);
	//mPropGrid->GetPropertyByLabel(L"PID")->SetValueFromString(data.mPID);
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
	mChangeConnection.disconnect();// ��� ������ ����� ��������� ��������� �����

	auto data = mObj->GetData();
	GetData(data);
	mObj->SetData(data);
	mObj->Save();
	EndModal(wxID_OK);
}
