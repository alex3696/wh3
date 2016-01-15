#include "_pch.h"
#include "dlg_favprop_dvmodel.h"


using namespace wh;
using namespace dlg::favprop::view;


DvModel::DvModel()
	:wxDataViewIndexListModel()
{
}
//---------------------------------------------------------------------------
DvModel::~DvModel()
{

}
//---------------------------------------------------------------------------
unsigned int DvModel::GetColumnCount() const
{
	return 4;
}
//---------------------------------------------------------------------------
wxString DvModel::GetColumnType(unsigned int col) const
{
	return !col ? "wxDataViewIconText" : "string";
}
//---------------------------------------------------------------------------
void DvModel::GetValueByRow(wxVariant &variant,
	unsigned int row, unsigned int col)const
{
	auto prop_model = std::dynamic_pointer_cast<model::FavPropItem>
		(mPropArray->GetChild(row));

	const auto& prop_data = prop_model->GetData();


	switch (col)
	{
	case 1:		variant = prop_data.mSelected;	break;
	case 2:		variant << wxDataViewIconText(prop_data.mLabel.toStr(), 
							ResMgr::GetInstance()->m_ico_classprop24);
		break;
	case 3:		variant = ToText(prop_data.mType);	break;
	case 4:		variant = prop_data.mId.toStr();	break;
	default:break;
	}
}
//---------------------------------------------------------------------------
bool DvModel::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr)const

{
	if (mPropArray)
	{
		const ModelState state = mPropArray->GetChild(row)->GetState();
		switch (state)
		{
			//msNull
		default:  break;
		case msCreated:
			attr.SetBold(true);
			attr.SetColour(*wxBLUE);
			break;
		case msExist:
			attr.SetBold(false);
			attr.SetColour(*wxBLACK);
			break;
		case msUpdated:
			attr.SetBold(true);
			attr.SetColour(wxColour(128, 64, 0));
			break;
		case msDeleted:
			attr.SetBold(true);
			attr.SetColour(*wxRED);
			break;
		}//switch
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------

bool DvModel::SetValueByRow(const wxVariant &variant,
	unsigned int row, unsigned int col)
{
	if (col != 1)
		return false;

	auto prop_model = std::dynamic_pointer_cast<model::FavPropItem>
		(mPropArray->GetChild(row));

	auto data = prop_model->GetData();
	const auto& stored = prop_model->GetStored();

	data.mSelected = variant.GetBool();
	if (stored.mSelected == data.mSelected)
		prop_model->SetData(data, true);
	else
		prop_model->SetData(data);
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void DvModel::SetModel(std::shared_ptr<IModel> model)
{
	mConnPropAppend.disconnect();
	mConnPropRemove.disconnect();
	mConnPropChange.disconnect();

	mPropArray = std::dynamic_pointer_cast<model::FavPropArray>(model);

	if (!mPropArray)
		return;

	namespace sph = std::placeholders;

	mConnPropAppend = mPropArray->ConnAfterInsert(
		std::bind(&DvModel::OnPropAppend, this, sph::_1, sph::_2, sph::_3));
	mConnPropRemove = mPropArray->ConnectBeforeRemove(
		std::bind(&DvModel::OnPropBeforeRemove, this, sph::_1, sph::_2));
	mConnPropChange = mPropArray->ConnectChangeSlot(
		std::bind(&DvModel::OnPropChange, this, sph::_1, sph::_2));

	Reset(mPropArray->GetChildQty());
}
//---------------------------------------------------------------------------
void DvModel::ClearModel()
{
	wxDataViewItemArray itemArray;
	GetChildren(wxDataViewItem(NULL), itemArray);
	this->ItemsDeleted(wxDataViewItem(NULL), itemArray);
}


//---------------------------------------------------------------------------
void DvModel::OnPropAppend(const IModel& vec
	, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore)

{
	if (!mPropArray)
		return;
	
	this->Reset(mPropArray->GetChildQty());
	/*
	namespace sph = std::placeholders;

	wxDataViewItemArray typeItemArray;
	for (const unsigned int& i : itemVec)
	{
		auto prop_model = std::dynamic_pointer_cast<model::FavPropItem>
			(mPropArray->GetChild(i));

		wxDataViewItem dv_item(prop_model.get());
		typeItemArray.Add(dv_item);
	}
	ItemsAdded(wxDataViewItem(NULL), typeItemArray);
	*/
}//OnAppend

//---------------------------------------------------------------------------
void DvModel::OnPropBeforeRemove(const IModel& vec,
	const std::vector<SptrIModel>& remVec)
{
	if (!mPropArray)
		return;
	auto newSize = vec.size() - remVec.size();
	this->Reset(newSize);

}//OnRemove
//---------------------------------------------------------------------------
void DvModel::OnPropChange(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	if (!mPropArray)
		return;

	/*
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto prop_model = std::dynamic_pointer_cast<model::FavPropItem>
			(mPropArray->GetChild(i));
		itemArray.Add(wxDataViewItem(prop_model.get()));
	}
	ItemsChanged(itemArray);
	*/
}//OnChange
//---------------------------------------------------------------------------

//bool DvModel::IsEnabledByRow(unsigned int row, unsigned int col) const
//{
//	return true;
//}
