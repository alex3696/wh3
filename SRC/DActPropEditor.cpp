#include "_pch.h"
#include "DActPropEditor.h"

using namespace wh;
using namespace view;
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DActPropEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DActPropEditor::DActPropEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Выберите свойство");

	mPropArray.reset(new MPropArray);

	mPropArrayView = new PropTable(this);
	GetSizer()->Insert(0, mPropArrayView, 1, wxALL | wxEXPAND, 0);



	this->Layout();
}
//---------------------------------------------------------------------------
void DActPropEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DActPropEditor::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(T_Model::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------
void DActPropEditor::OnChangeModel(const IModel* model, const T_Model::T_Data* data)
{
	auto propModel = std::dynamic_pointer_cast<MActProp>(mModel);
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
				if (propData.mId == actPropData.mId)
					break;
			}
				
		}
		wxDataViewItem item((void*)i);
		mPropArrayView->Select(item);
	}


	
	//if (mModel && mModel.get() == &model)
	//{
	//	const auto state = model.GetState();
	//	const auto& rec = mModel->GetData();
	//	SetData(rec);
	//}

}
//---------------------------------------------------------------------------
void DActPropEditor::UpdateModel()const
{
	if (mModel)
	{
		std::vector<unsigned int> selected;
		mPropArrayView->GetSelected(selected);
		if (!selected.empty())
		{
			auto childItem = mPropArray->GetChild(selected[0]);
			auto propItem = std::dynamic_pointer_cast<MPropChild>(childItem);
			
			rec::ActProp actProp = mModel->GetData();
			const rec::Prop& prop = propItem->GetData();

			actProp.mProp = prop;

			mModel->SetData(actProp);
		}
	}
}
//---------------------------------------------------------------------------
int DActPropEditor::ShowModal()
{
	mPropArray->Load();
	mPropArrayView->SetModel(mPropArray);
	return DlgBaseOkCancel::ShowModal();
}



