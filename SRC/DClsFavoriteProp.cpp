#include "_pch.h"
#include "DClsFavoriteProp.h"
#include "MClsActProp.h"
#include "MClsFavoriteActProp.h"

using namespace wh;
using namespace wh::view;



//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsFavoriteProp
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsFavoriteProp::DClsFavoriteProp(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	: wxDialog(parent, id, title, pos, size, style, name)
	//, mModel(nullptr)
{
	SetTitle("Редактирование информации о избранном свойстве");
	mAuiMgr.SetManagedWindow(this);

	mPropsPanel = new VClsFavoritePropCtrlPanel(this);
	

	wxPanel* btnPanel = new wxPanel(this);
	mBtnSizer = new wxStdDialogButtonSizer();
	btnPanel->SetSizer(mBtnSizer);
	mBtnOK = new wxButton(btnPanel, wxID_OK);//,"Сохранить и закрыть" );
	mBtnSizer->AddButton(mBtnOK);
	mBtnCancel = new wxButton(btnPanel, wxID_CANCEL);//," Закрыть" );
	mBtnSizer->AddButton(mBtnCancel);

	mBtnSizer->Realize();
	btnPanel->Layout();

	mAuiMgr.AddPane(btnPanel, wxAuiPaneInfo().Bottom()
		.Resizable(false)
		.CaptionVisible(false)
		.PaneBorder(false)
		.MinSize(-1, mBtnOK->GetSize().GetHeight()*1.4)
		);

	wxAuiDockArt* frame = mAuiMgr.GetArtProvider();
	frame->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	frame->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
	frame->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);

	wxAuiPaneInfo pnlInfo;
	pnlInfo.CloseButton(false).Resizable().PaneBorder(false);

	mAuiMgr.AddPane(mPropsPanel, wxAuiPaneInfo(pnlInfo)
		.Center()
		//.Resizable()
		.Caption("Дополнительные свойства")
		.Icon(m_ResMgr->m_ico_classprop24));


	this->SetSize(600, 500);

	mAuiMgr.Update();

}
//-----------------------------------------------------------------------------
DClsFavoriteProp::~DClsFavoriteProp()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void DClsFavoriteProp::SetModel(std::shared_ptr<IModel>& newModel)
{
	auto clsPropModel = std::dynamic_pointer_cast<MClsFavorite>(newModel);

	if (clsPropModel)
	{
		const auto state = clsPropModel->GetState();
		mPropsPanel->SetModel(clsPropModel->GetPropArray());
	}

}//SetModel
//---------------------------------------------------------------------------
void DClsFavoriteProp::UpdateModel()const
{
	//??mPropsPanel->UpdateModel();
}
//---------------------------------------------------------------------------
int DClsFavoriteProp::ShowModal()
{
	return wxDialog::ShowModal();
}