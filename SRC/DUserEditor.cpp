#include "_pch.h"
#include "DUserEditor.h"

using namespace wh;
using namespace wh::view;



//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DUserEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DUserEditor::DUserEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	: wxDialog(parent, id, title, pos, size, style, name)
	//, mModel(nullptr)
{
	SetTitle("Редактирование информации о пользователе");
	mAuiMgr.SetManagedWindow(this);

	mUserPanel = new VUserPanel(this);;
	mGroupsPanel = new VGroupsCtrlPanel(this);


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

	mAuiMgr.AddPane(mUserPanel, wxAuiPaneInfo(pnlInfo)
		.Top()
		.Caption("Основные сведения")
		.Icon(m_ResMgr->m_ico_user24));


	mAuiMgr.AddPane(mGroupsPanel, wxAuiPaneInfo(pnlInfo)
		.Center()
		//.Resizable()
		.Caption("Группы")
		.Icon(m_ResMgr->m_ico_usergroup24));


	this->SetSize(600, 500);

	mAuiMgr.Update();

}
//-----------------------------------------------------------------------------
DUserEditor::~DUserEditor()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void DUserEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	auto userModel = std::dynamic_pointer_cast<MUser2>(newModel);

	if (userModel)
	{
		const auto userState = userModel->GetState();

		switch (userState)
		{
		default:
			newModel->Load();
			newModel->LoadChilds();
			break;
		case msCreated:
			break;
		}

		mUserPanel->SetModel(newModel);
		mGroupsPanel->SetModel(userModel->GetGroupArray());
	}


}//SetModel
//---------------------------------------------------------------------------
void DUserEditor::UpdateModel()const
{
	mUserPanel->UpdateModel();
	//??mGroupsPanel->UpdateModel();
}
//---------------------------------------------------------------------------
int DUserEditor::ShowModal()
{
	return wxDialog::ShowModal();
}