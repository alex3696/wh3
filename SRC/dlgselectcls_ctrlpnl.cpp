#include "_pch.h"
#include "dlgselectcls_ctrlpnl.h"

using namespace wh;

//---------------------------------------------------------------------------
CatDlg::CatDlg(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	:wxDialog(parent, id, title, pos, size, style, name)
	, mMainPanel(nullptr)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);


	m_sdbSizer = new wxStdDialogButtonSizer();
	m_btnOK = new wxButton(this, wxID_OK);//,"Сохранить и закрыть" );
	m_sdbSizer->AddButton(m_btnOK);
	m_btnCancel = new wxButton(this, wxID_CANCEL);//," Закрыть" );
	m_sdbSizer->AddButton(m_btnCancel);
	
	m_sdbSizer->Realize();

	mMainPanel = new view::VObjCatalogCtrl(this);
	mMainPanel->BuildToolbar(true);

	szrMain->Add(mMainPanel, 1, wxALL | wxEXPAND, 0);
	szrMain->Add(m_sdbSizer, 0, wxALL | wxEXPAND, 10);

	this->SetSizer(szrMain);

	mMainPanel->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &CatDlg::OnActivated, this);
	mMainPanel->Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &CatDlg::OnSelect, this);
	m_btnOK->Enable(false);
}
//---------------------------------------------------------------------------
void CatDlg::SetModel(std::shared_ptr<IModel> model)
{
	mCatalog = std::dynamic_pointer_cast<object_catalog::MObjCatalog>(model);
	mMainPanel->SetModel(model);
}
//---------------------------------------------------------------------------
bool CatDlg::GetSelectedCls(wh::rec::Cls& cls)
{
	const auto item = mMainPanel->GetSelectedItem();
	const auto typetem = dynamic_cast<const object_catalog::MTypeItem*> (item);
	if (typetem)
	{
		cls = typetem->GetData();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool CatDlg::GetSelectedObj(wh::rec::ObjInfo& obj)
{
	const auto item = mMainPanel->GetSelectedItem();
	const auto objItem = dynamic_cast<const object_catalog::MObjItem*> (item);
	
	if (!objItem)
		return false;

	auto mcls = objItem->GetCls();
	if (!mcls)
		return false;
	
	obj.mObj = objItem->GetData();
	obj.mCls = mcls->GetData();
	return true;
}
//---------------------------------------------------------------------------
void CatDlg::OnActivated(wxDataViewEvent& evt)
{
	m_btnOK->Enable(false);
	evt.Skip();
}
//---------------------------------------------------------------------------
void CatDlg::OnSelect(wxDataViewEvent& evt)
{
	m_btnOK->Enable(false);
	
	if (!mCatalog)
		return;
	if (!mCatalog->mTypeArray->GetChildQty())
		return;
	if (!mTargetValidator)
		return;
	
	auto selectedItem = evt.GetItem();
	if (selectedItem.IsOk())
	{
		namespace cat = wh::object_catalog;

		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto typeItem = dynamic_cast<cat::MTypeItem*> (modelInterface);
		if (typeItem)
		{
			const cat::MTypeItem::DataType& dt = typeItem->GetData();
			m_btnOK->Enable(mTargetValidator(&dt, nullptr));
		}
		else 
		{
			auto objItem = dynamic_cast<cat::MObjItem*> (modelInterface);
			auto objArray = dynamic_cast<cat::MObjArray*> (objItem->GetParent());
			if (objArray)
			{
				typeItem = dynamic_cast<cat::MTypeItem*> (objArray->GetParent());
				if (typeItem)
				{
					const auto& cls_date = typeItem->GetData();
					const auto& obj_date = objItem->GetData();
					m_btnOK->Enable(mTargetValidator(&cls_date, &obj_date));
				}
			}
		}
		
	}
	evt.Skip();
}
