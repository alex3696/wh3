#include "_pch.h"
#include "DClsFavoriteActPropEditor.h"
#include "MClsFavoriteActProp.h"


using namespace wh;
using namespace view;
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsFavoriteActPropEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsFavoriteActPropEditor::DClsFavoriteActPropEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("¬ыберите свойство");

	mPropArray.reset(new MClsAllActPropArray);

	mPropArrayView = new PropTable(this);
	GetSizer()->Insert(0, mPropArrayView, 1, wxALL | wxEXPAND, 0);



	this->Layout();
}
//---------------------------------------------------------------------------
DClsFavoriteActPropEditor::~DClsFavoriteActPropEditor()
{
	/*
	if (mModel)
	{
		auto propArray = dynamic_cast<MClsActPropArray*>(mModel->GetParent());
		auto clsModel = dynamic_cast<MClsFavorite*>(propArray->GetParent());


		unsigned int pos;
		if (clsModel->GetItemPosition(mPropArray, pos))
		{
			clsModel->DelChild(pos);// уже удалено в презентере
		}

	}
	*/

}
//---------------------------------------------------------------------------
void DClsFavoriteActPropEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<MClsActProp>(newModel);
		if (mModel)
		{
			auto propArray = dynamic_cast<MClsActPropArray*>(mModel->GetParent());
			auto clsModel = dynamic_cast<MClsFavorite*>(propArray->GetParent());
			
			clsModel->AddChild(std::dynamic_pointer_cast<IModel>(mPropArray));
			
			auto funcOnChange = std::bind(&DClsFavoriteActPropEditor::OnChangeModel,
				this, std::placeholders::_1);
			mChangeConnection = mModel->ConnectChangeSlot(funcOnChange);
			OnChangeModel(*mModel.get());
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void DClsFavoriteActPropEditor::OnChangeModel(const IModel& model)
{
	/*
	auto propModel = std::dynamic_pointer_cast<MClsActProp>(mModel);
	if (propModel)
	{
		auto& actPropData = propModel->GetData();

		unsigned int i = mPropArray->GetChildQty();
		while (i--)
		{
			std::shared_ptr<IModel> child = mPropArray->GetChild(i);
			std::shared_ptr<MPropChild> prop = std::dynamic_pointer_cast<MPropChild>(child);
			if (prop)
			{
				const rec::Prop& propData = prop->GetData();
				if (propData.mID == actPropData.mID)
					break;
			}

		}
		wxDataViewItem item((void*)i);
		mPropArrayView->Select(item);
	}
*/

}
//---------------------------------------------------------------------------
void DClsFavoriteActPropEditor::UpdateModel()const
{
	if (mModel)
	{
		std::vector<unsigned int> selected;
		mPropArrayView->GetSelected(selected);
		if (!selected.empty())
		{
			auto childItem = mPropArray->GetChild(selected[0]);
			auto propItem = std::dynamic_pointer_cast<MClsActProp>(childItem);

			const auto& clsActPropData = propItem->GetData();

			mModel->SetData(clsActPropData);
		}
	}
}
//---------------------------------------------------------------------------
int DClsFavoriteActPropEditor::ShowModal()
{
	mPropArray->Load();
	mPropArrayView->SetModel(mPropArray);
	return DlgBaseOkCancel::ShowModal();
}



