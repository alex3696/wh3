#include "_pch.h"
#include "dlg_act_view_ActListDvModel.h"


using namespace wh;
using namespace wh::dlg_act::view;


ActListDvModel::ActListDvModel()
	:wxDataViewModel()
{
}
//---------------------------------------------------------------------------
ActListDvModel::~ActListDvModel()
{

}
//---------------------------------------------------------------------------
unsigned int ActListDvModel::GetColumnCount() const
{
	return 3;
}
//---------------------------------------------------------------------------
wxString ActListDvModel::GetColumnType(unsigned int col) const
{
	//return !col ? "wxDataViewIconText" : "string";
	return "string";
}
//---------------------------------------------------------------------------
bool ActListDvModel::IsContainer(const wxDataViewItem &dataViewItem)const
{
	return false;
}
//---------------------------------------------------------------------------

void ActListDvModel::GetValue(wxVariant &variant, 
	const wxDataViewItem &dataViewItem, unsigned int col) const
{
	if (!dataViewItem.IsOk())
		return;

	auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
	auto actItem = dynamic_cast<model::Act*> (modelInterface);

	if (actItem)
	{
		const auto& actData = actItem->GetData();
		switch (col)
		{
			case 1:		variant = actData.mLabel ;	break;
			case 2:		variant = actData.mComment;	break;
			case 3:		variant = actData.mID;		break;
			default:	break;
		}
	}
	
}
//---------------------------------------------------------------------------
bool ActListDvModel::GetAttr(const wxDataViewItem &dataViewItem, unsigned int col, wxDataViewItemAttr &attr) const
{
	if (dataViewItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
		auto actItem = dynamic_cast<model::Act*> (modelInterface);
		if (actItem)
		{
			const auto& actData = actItem->GetData();
			attr.SetBackgroundColour(actData.mColor);
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------

bool ActListDvModel::SetValue(const wxVariant &variant, const wxDataViewItem &item,
	unsigned int col)
{
	return false;
}
//---------------------------------------------------------------------------

wxDataViewItem ActListDvModel::GetParent(const wxDataViewItem &dataViewItem) const
{
	return wxDataViewItem(NULL);// the invisible root node has no parent
}
//---------------------------------------------------------------------------
unsigned int ActListDvModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
{
	if (!mActArray)
		return 0;

	unsigned int children_count(0);

	if (!parent.IsOk()) 
	{
		children_count = mActArray->GetChildQty();
		for (unsigned int i = 0; i < children_count; ++i)
		{
			auto typeItem = mActArray->GetChild(i);
			array.Add(wxDataViewItem(typeItem.get()));
		}
	}
	return children_count;
}

//---------------------------------------------------------------------------
void ActListDvModel::SetModel(std::shared_ptr<dlg_act::model::ActArray> model)
{
	mConnAppend.disconnect();
	mConnRemove.disconnect();
	mConnChange.disconnect();

	mActArray = model;

	if (!mActArray)
		return;

	namespace sph = std::placeholders;

	mConnAppend = mActArray->ConnAfterInsert(
		std::bind(&ActListDvModel::OnAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnRemove = mActArray->ConnectBeforeRemove(
		std::bind(&ActListDvModel::OnBeforeRemove, this, sph::_1, sph::_2));
	mConnChange = mActArray->ConnectChangeSlot(
		std::bind(&ActListDvModel::OnChange, this, sph::_1, sph::_2));

}
//---------------------------------------------------------------------------
void ActListDvModel::ClearModel()
{
	wxDataViewItemArray itemArray;
	GetChildren(wxDataViewItem(NULL), itemArray);
	this->ItemsDeleted(wxDataViewItem(NULL), itemArray);
}


//---------------------------------------------------------------------------
void ActListDvModel::OnAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore)
{
	for (const auto& curr : newItems)
	{
		wxDataViewItem actItem(curr.get());
		ItemAdded(wxDataViewItem(NULL), actItem);
	}
}//OnAppend
//---------------------------------------------------------------------------
void ActListDvModel::OnBeforeRemove(const IModel& vec,
	const std::vector<SptrIModel>& remVec)
{
	wxDataViewItemArray itemArray;
	for (const auto& remItem : remVec)
	{
		itemArray.Add(wxDataViewItem(remItem.get()));
	}
	ItemsDeleted(wxDataViewItem(NULL), itemArray);
}//OnRemove
//---------------------------------------------------------------------------
void ActListDvModel::OnChange(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = mActArray->GetChild(i);
		itemArray.Add(wxDataViewItem(item.get()));
	}
	ItemsChanged(itemArray);

}//OnChange



