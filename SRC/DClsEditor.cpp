#include "_pch.h"
#include "DClsEditor.h"

#include "MClsProp.h"
#include "MClsAct.h"
#include "MClsMove.h"

using namespace wh;
using namespace wh::view;



//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsEditor::DClsEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	: wxDialog(parent, id, title, pos, size, style, name)
	//, mModel(nullptr)
{
	SetTitle("Редактирование класса");
	mAuiMgr.SetManagedWindow(this);

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

	mNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition,wxDefaultSize,
		wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS 
		|  wxNO_BORDER
		);
	
	mAuiMgr.AddPane(mNotebook, wxAuiPaneInfo().
		Name(wxT("NotebookPane")).Caption(wxT("Notebook")).
		CenterPane()//.Layer(1).Position(1)
		.CloseButton(false).MaximizeButton(false).PaneBorder(false));

	wxImageList* image_list = new wxImageList(24, 24);
	image_list->Add(m_ResMgr->m_ico_type24);
	image_list->Add(m_ResMgr->m_ico_classprop24);
	image_list->Add(m_ResMgr->m_ico_rule24);
	image_list->Add(m_ResMgr->m_ico_act24);
	image_list->Add(m_ResMgr->m_ico_obj24);
	image_list->Add(m_ResMgr->m_ico_move24);


	mClsPanel = new VClsDataEditorPanel(mNotebook);
	mClsPropPanel = new VClsPropPanel(mNotebook);
	mClsActPanel = new VClsActPanel(mNotebook);
	mClsMovePanel = new VClsMovePanel(mNotebook);

	mNotebook->AssignImageList(image_list);

	mNotebook->AddPage(mClsPanel, "Основные", true, 0);
	//mNotebook->AddPage(mClsPropPanel, "Свойства", false, 1);
	//mNotebook->AddPage(mClsActPanel, "Действия", false, 3);
	//mNotebook->AddPage(mClsMovePanel, "Перемещения", false, 5);
	//mNotebook->AddPage(mClsObjNumPanel, "Объекты", false, 4);
	//mNotebook->AddPage(mClsObjQtyPanel, "Объекты", false, 4);

	this->SetSize(600, 500);

	mAuiMgr.Update();

}
//-----------------------------------------------------------------------------
DClsEditor::~DClsEditor()
{
	mChangeConnection.disconnect();
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void DClsEditor::SetModel(std::shared_ptr<IModel>& newModel)
{

	if (newModel != mClsNode)
	{
		mChangeConnection.disconnect();
		mClsNode = std::dynamic_pointer_cast<object_catalog::MTypeItem>(newModel);
		if (mClsNode)
		{
			mClsPanel->SetModel(newModel);

			//mClsNode->Load();
			auto clsState = mClsNode->GetState();
			auto cls = mClsNode->GetData();

			if (msNull != clsState && msCreated != clsState && msDeleted != clsState)
			{
				mClsNode->GetClsPropArray()->Load();
				if (cls.IsNumberic())
				{
					mClsNode->GetClsActArray()->Load();
					mClsNode->GetClsMoveArray()->Load();
				}
				else if (cls.IsQuantity())
				{
					mClsNode->GetClsActArray()->Load();
					mClsNode->GetClsMoveArray()->Load();
				}
			}

			
			mClsPropPanel->SetModel(
				std::dynamic_pointer_cast<IModel>(mClsNode->GetClsPropArray()) );
			mClsActPanel->SetModel(
				std::dynamic_pointer_cast<IModel>(mClsNode->GetClsActArray()));
			mClsMovePanel->SetModel(
				std::dynamic_pointer_cast<IModel>(mClsNode->GetClsMoveArray()));
			
			auto funcOnChange = std::bind(&DClsEditor::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mClsNode->DoConnect(moAfterUpdate, funcOnChange);
			OnChangeModel(mClsNode.get(), nullptr);

		}//if (mModel)
	}


}//SetModel
//---------------------------------------------------------------------------
void DClsEditor::UpdateModel()const
{
	mClsPanel->UpdateModel();
	//mUserPanel->UpdateModel();
	//??mGroupsPanel->UpdateModel();
}
//---------------------------------------------------------------------------
int DClsEditor::ShowModal()
{
	return wxDialog::ShowModal();
}
//---------------------------------------------------------------------------
void DClsEditor::OnChangeModel(const IModel* model, 
	const object_catalog::MTypeItem::T_Data* data)
{
	if (mClsNode)
	{
		const auto& cls = mClsNode->GetData();

		wxBusyCursor		busyCursor;
		wxWindowUpdateLocker	wndDisabler(mNotebook);

		while (mNotebook->GetPageCount() > 1)
			mNotebook->RemovePage(1);

		mClsPropPanel->Show(false);
		mClsActPanel->Show(false);
		mClsMovePanel->Show(false);

		if (cls.mType.IsNull())
			return;
		switch (cls.GetClsType() )
		{
		case ctSingle:
			mClsPropPanel->Show(true);
			mClsActPanel->Show(true);
			mClsMovePanel->Show(true);

			mNotebook->AddPage(mClsPropPanel, "Свойства", false, 1);
			mNotebook->AddPage(mClsActPanel, "Действия", false, 3);
			mNotebook->AddPage(mClsMovePanel, "Перемещения", false, 5);
			break;
		case ctQtyByFloat: case ctQtyByOne:
			mClsPropPanel->Show(true);
			mClsActPanel->Show(true);
			mClsMovePanel->Show(true);

			mNotebook->AddPage(mClsPropPanel, "Свойства", false, 1);
			mNotebook->AddPage(mClsActPanel, "Действия", false, 3);
			mNotebook->AddPage(mClsMovePanel, "Перемещения", false, 5);
			break;
		case ctAbstract:
		default:break;
		}//switch


	}
}