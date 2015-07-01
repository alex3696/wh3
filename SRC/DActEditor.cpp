#include "_pch.h"
#include "DActEditor.h"

using namespace wh;
using namespace wh::view;



//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DActEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DActEditor::DActEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	: wxDialog(parent, id, title, pos, size, style, name)
	//, mModel(nullptr)
{
	SetTitle("Редактирование информации о действии");
	mAuiMgr.SetManagedWindow(this);

	mActPanel = new VActPanel(this);;
	mPropsPanel = new VActPropCtrlPanel(this);


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

	mAuiMgr.AddPane(mActPanel, wxAuiPaneInfo(pnlInfo)
		.Top()
		.Caption("Основные сведения")
		.Icon(m_ResMgr->m_ico_act24));


	mAuiMgr.AddPane(mPropsPanel, wxAuiPaneInfo(pnlInfo)
		.Center()
		//.Resizable()
		.Caption("Дополнительные свойства")
		.Icon(m_ResMgr->m_ico_classprop24));


	this->SetSize(600, 500);

	mAuiMgr.Update();

}
//-----------------------------------------------------------------------------
DActEditor::~DActEditor()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void DActEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	auto actModel = std::dynamic_pointer_cast<MAct>(newModel);

	if (actModel)
	{
		const auto actState = actModel->GetState();

		switch (actState)
		{
		default:
			newModel->Load();
			newModel->LoadChilds();
			break;
		case msCreated :
			break;
		}

		mActPanel->SetModel(newModel);
		mPropsPanel->SetModel(actModel->GetPropArray());
	}

	
}//SetModel
//---------------------------------------------------------------------------
void DActEditor::UpdateModel()const
{
	mActPanel->UpdateModel();
	//??mPropsPanel->UpdateModel();
}
//---------------------------------------------------------------------------
int DActEditor::ShowModal()
{
	return wxDialog::ShowModal();
}