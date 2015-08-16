#include "_pch.h"
#include "DUserGroupEditor.h"

using namespace wh;
using namespace view;
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DUserGroupEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DUserGroupEditor::DUserGroupEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Выберите свойство");

	mGroupArray.reset(new MGroupArray);

	mGroupArrayView = new GroupTable(this);
	GetSizer()->Insert(0, mGroupArrayView, 1, wxALL | wxEXPAND, 0);



	this->Layout();
}
//---------------------------------------------------------------------------
void DUserGroupEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DUserGroupEditor::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(T_Model::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void DUserGroupEditor::OnChangeModel(const IModel* model,const T_Model::T_Data* data)
{
	auto userGroupModel = std::dynamic_pointer_cast<MUserGroup>(mModel);
	if (userGroupModel)
	{
		const rec::UserRole& userGroupData = userGroupModel->GetData();

		unsigned int i = mGroupArray->GetChildQty();
		while (i--)
		{
			std::shared_ptr<IModel> child = mGroupArray->GetChild(i);
			std::shared_ptr<MGroup> group = std::dynamic_pointer_cast<MGroup>(child);
			if (group)
			{
				const rec::Role& groupData = group->GetData();
				if (groupData.mID == userGroupData.mID)
					break;
			} 
				
		}
		wxDataViewItem item((void*)i);
		mGroupArrayView->Select(item);
	}



}
//---------------------------------------------------------------------------
void DUserGroupEditor::UpdateModel()const
{
	if (mModel)
	{
		std::vector<unsigned int> selected;
		mGroupArrayView->GetSelected(selected);
		if (!selected.empty())
		{
			auto childItem = mGroupArray->GetChild(selected[0]);
			auto groupItem = std::dynamic_pointer_cast<MGroup>(childItem);

			auto userGroup = mModel->GetData();
			auto group = groupItem->GetData();

			userGroup.mID = group.mID;
			userGroup.mLabel = group.mLabel;

			mModel->SetData(userGroup);
		}
	}
}
//---------------------------------------------------------------------------
int DUserGroupEditor::ShowModal()
{
	mGroupArray->Load();
	mGroupArrayView->SetModel(mGroupArray);
	return DlgBaseOkCancel::ShowModal();
}



