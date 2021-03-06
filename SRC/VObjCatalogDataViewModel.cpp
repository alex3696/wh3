#include "_pch.h"
#include "VObjCatalogDataViewModel.h"
#include "MObjItem.h"
#include "MTypeNode.h"


using namespace wh;
using namespace view;


VObjCatalogDataViewModel::VObjCatalogDataViewModel()
	:wxDataViewModel()
{
}
//---------------------------------------------------------------------------
VObjCatalogDataViewModel::~VObjCatalogDataViewModel()
{

}
//---------------------------------------------------------------------------
unsigned int VObjCatalogDataViewModel::GetColumnCount() const
{
	return 4;
}
//---------------------------------------------------------------------------
wxString VObjCatalogDataViewModel::GetColumnType(unsigned int col) const
{
	return (col==0) ? "wxDataViewIconText" : "string";
}
//---------------------------------------------------------------------------
bool VObjCatalogDataViewModel::IsContainer(const wxDataViewItem &dataViewItem)const
{
	if (!dataViewItem.IsOk())
		return true;

	auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
	auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);

	if (!typeItem)
		return false;

	auto typeArray = typeItem->GetParent();
	if (!typeArray)
		throw;
	auto catalog = dynamic_cast<object_catalog::MObjCatalog*>(typeArray->GetParent());
	if (!catalog)
		throw;
	
	const auto& cls_data = typeItem->GetData();

	bool is_container = (catalog->IsObjEnabled()) ?
		!cls_data.IsAbstract()
		: false;
	return is_container;
}
//---------------------------------------------------------------------------

void VObjCatalogDataViewModel::GetValue(wxVariant &variant, const wxDataViewItem &dataViewItem, unsigned int col) const
{
	wxString val;
	const wxIcon*  ico(&wxNullIcon);
	
	
	auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
	auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
	auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
	
	if (typeItem)
	{
		const auto& cls = typeItem->GetData();
		
		if (!cls.mType.IsNull() )
		{
			//const wxIcon*	clsIcon(&wxNullIcon);
			//wxString		clsLabel;
			
			auto mgr = ResMgr::GetInstance();
			switch (cls.GetClsType() )
			{
			
			default: //ClsKind::Abstract
				if (0 == col)
				{
					ico = &mgr->m_ico_type_abstract24;
					val = wxString::Format("%s", cls.mLabel.toStr());
				}
				break;
			case ClsKind::QtyByFloat:
			case ClsKind::QtyByOne:ico = &mgr->m_ico_type_qty24;
							val = wxString::Format("%s - %s (%s)",
							//cls.mClsLabel, cls.mQty, cls.mMeasure);
							cls.mLabel.toStr(), typeItem->GetQty(), cls.mMeasure.toStr());
							break;
			case ClsKind::Single:	ico = &mgr->m_ico_type_num24;
							val = wxString::Format("%s - %s (%s)",
							//cls.mLabel, cls.mQty, cls.mMeasure);
							cls.mLabel.toStr(), typeItem->GetQty(), cls.mMeasure.toStr());
							break;
			} // switch (clsType)
		} // if (cls.GetClsType(clsType))
	}
	else if (objItem)
	{
		const auto& obj = objItem->GetData();
		
		auto objArray = objItem->GetParent();
		if (objArray)
			typeItem = dynamic_cast<object_catalog::MTypeItem*> (objArray->GetParent());

		switch (col)
		{
		case 0:		val = obj.mLabel;	break;
		case 1:		val = obj.mQty + " (" + typeItem->GetData().mMeasure.toStr() + ")"; break;
		case 2:		val = obj.mId;				break;
		case 3:		val = obj.mParent.mId;		break;
		case 4:		val = obj.mLastMoveLogId;	break;
		case 5:		val = objItem->mPath;		break;
		default:	if (obj.mProp.size() > (col - 6) )
						val = obj.mProp[col - 6];
					break;
		}
			
	}

	switch (col)
	{
	case 0:		variant << wxDataViewIconText(val, *ico);
		break;
	default:	
		if (objItem)
			variant = val;
		break;
	}
}
//---------------------------------------------------------------------------
/*
bool VObjCatalogDataViewModel::GetAttr(const wxDataViewItem &dataViewItem, unsigned int col, wxDataViewItemAttr &attr) const
{
	if (dataViewItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
		auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
		object_catalog::MTypeItem* typeItem(nullptr);
		
		bool has_bg = true;
		if (!objItem)
		{
			attr.SetBold(true);
			typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
			if (col > 6)
				attr.SetBackgroundColour(wxColour(240, 240, 240));
			//return true;

		}
		else
		{
			
			auto objArray = dynamic_cast<object_catalog::MObjArray*> (objItem->GetParent());
			typeItem = dynamic_cast<object_catalog::MTypeItem*> (objArray->GetParent());
			
			auto typeArray = dynamic_cast<object_catalog::MTypeArray*> (typeItem->GetParent());
			auto catalog = dynamic_cast<object_catalog::MObjCatalog*> (typeArray->GetParent());
			
			if (!typeItem->GetData().mType.IsNull() 
				&& ClsKind::Single != typeItem->GetData().GetClsType()
				&& col > 6)
				//attr.SetBackgroundColour(wxColour(240, 240, 240));
				has_bg = false;
			else
			{
				if (catalog->IsPropEnabled())
				{
					const rec::FavProps& favProp = catalog->GetFavProps();
					if (col > 6 && favProp.size())
					{
						const auto& field = favProp[col - 7];
						const auto& typeId = typeItem->GetData().mId;

						auto it = field.mCls->find(typeId);
						if (field.mCls->end() == it)
							//attr.SetBackgroundColour(wxColour(240, 240, 240));
							has_bg = false;
					}
				}//if (catalog->IsPropEnabled())
			}
			
			
		}
			

		if (typeItem)
		{
			const auto& cls = typeItem->GetData();
			if (!cls.mType.IsNull())
			{
				switch (cls.GetClsType())
				{
				default://0 
						attr.SetColour(wxColour(100, 100, 100)); break;
				case ClsKind::QtyByFloat:
				case ClsKind::QtyByOne: 
						attr.SetColour(wxColour(0, 80, 150));
						if (has_bg)
							attr.SetBackgroundColour(wxColour(210, 240, 250));
						break;
				case ClsKind::Single: 
						attr.SetColour(wxColour(100, 30, 0));
						if (has_bg)
							attr.SetBackgroundColour(wxColour(250, 240, 210));
						break;
				}//switch
			}//if (cls.GetClsType(clsType))
		}//if (typeItem)
	
		return true;
	}//if (dataViewItem.IsOk())
	return false;
}
*/
//---------------------------------------------------------------------------

bool VObjCatalogDataViewModel::SetValue(const wxVariant &variant, const wxDataViewItem &item,
	unsigned int col)	
{
	return false;
}
//---------------------------------------------------------------------------

wxDataViewItem VObjCatalogDataViewModel::GetParent(const wxDataViewItem &dataViewItem) const
{
	if (dataViewItem.IsOk())
	{
		using namespace object_catalog;
		
		auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
		auto typeItem = dynamic_cast<MTypeItem*> (modelInterface);
		if (typeItem)
			return wxDataViewItem(NULL);
		else
		{
			auto objItem = dynamic_cast<MObjItem*> (modelInterface);
			if (objItem)
			{
				auto objArray = dynamic_cast<MObjArray*> (objItem->GetParent());
				if (objArray)
				{
					auto typeItem = dynamic_cast<MTypeItem*> (objArray->GetParent());
					if (typeItem)
						return wxDataViewItem(typeItem);
				}
			}
		}
	}
	return wxDataViewItem(NULL);// the invisible root node has no parent
}
//---------------------------------------------------------------------------
unsigned int VObjCatalogDataViewModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
{
	if (!mCatalogModel)
		return 0;

	unsigned int children_count(0);

	if (parent.IsOk()) // ���� ���� �������� ( ��������==������ ), ��������� �������
	{
		auto modelInterface = static_cast<IModel*> (parent.GetID());
		auto typeItem = dynamic_cast<object_catalog::MTypeItem*>(modelInterface);

		auto objArrayModel = typeItem->mObjArray;

		children_count = objArrayModel->GetChildQty();
		for (unsigned int i = 0; i < children_count; ++i)
		{
			auto objItem = objArrayModel->GetChild(i);
			array.Add(wxDataViewItem(objItem.get()));
			
		}
	}
	else // ���� �������� ���(�.�. �������� ������ NULL) �� ��������� ����
	{
		children_count = mCatalogModel->mTypeArray->GetChildQty();
		for (unsigned int i = 0; i < children_count; ++i)
		{
			auto typeItem = mCatalogModel->mTypeArray->GetChild(i);
			array.Add(wxDataViewItem(typeItem.get()));

		}
	}
	return children_count;
}
//---------------------------------------------------------------------------
//int VObjCatalogDataViewModel::Compare(const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column, bool ascending) const
//{
//	wxVariant var1, var2;
//	GetValue(var1, item1, 0);
//	GetValue(var2, item2, 0);
//
//	wxString str1, str2;
//
//	wh::view::GetStringValue(var1, str1);
//	wh::view::GetStringValue(var2, str2);
//
//	double	value1, value2;
//	bool	isNumberVal1 = str1.ToDouble(&value1);
//	bool	isNumberVal2 = str2.ToDouble(&value2);
//
//	if (isNumberVal1 && isNumberVal2)
//	{
//		if (value1 == value2)
//			return 0;
//		return ascending ? value1>value2 : value2>value1;
//	}
//
//	if (isNumberVal1 && !isNumberVal2)
//		return ascending ? -1 : 1;
//	if (!isNumberVal1 && isNumberVal2)
//		return ascending ? 1 : -1;
//
//
//	return ascending ? str1.compare(str2) : str2.compare(str1);
//}

//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::SetModel(std::shared_ptr<IModel> model)
{
	mConnClsAppend.disconnect();
	mConnClsRemove.disconnect();
	mConnClsChange.disconnect();

	mCatalogModel = std::dynamic_pointer_cast<object_catalog::MObjCatalog>(model);

	if (!mCatalogModel)
		return;
	
	namespace sph = std::placeholders;

	mConnClsAppend = mCatalogModel->mTypeArray->ConnAfterInsert(
		std::bind(&VObjCatalogDataViewModel::OnClsAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnClsRemove = mCatalogModel->mTypeArray->ConnectBeforeRemove(
		std::bind(&VObjCatalogDataViewModel::OnClsBeforeRemove, this, sph::_1, sph::_2));
	mConnClsChange = mCatalogModel->mTypeArray->ConnectChangeSlot(
		std::bind(&VObjCatalogDataViewModel::OnClsChange, this, sph::_1, sph::_2));

//	void OnAppend(const IModel&, const std::vector<unsigned int>&);
//	void OnRemove(const IModel&, const std::vector<unsigned int>&);
//	void OnChange(const IModel&, const std::vector<unsigned int>&);

	std::vector<SptrIModel> vec;
	for (size_t i = 0; i < mCatalogModel->mTypeArray->GetChildQty(); i++)
		vec.emplace_back(mCatalogModel->mTypeArray->GetChild(i));
	OnClsAfterInsert(*mCatalogModel->mTypeArray.get(), vec, SptrIModel());
}
//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::ClearModel()
{
	wxDataViewItemArray itemArray;
	GetChildren(wxDataViewItem(NULL), itemArray);
	this->ItemsDeleted(wxDataViewItem(NULL), itemArray);
}


//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnClsAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	namespace sph = std::placeholders; 
	namespace cat = wh::object_catalog;

	auto typeArray = mCatalogModel->mTypeArray; // == vec

	wxDataViewItemArray typeItemArray;
	//for (const unsigned int& i : itemVec)
	for (const auto& curr : newItems)
	{
		auto clsModel = std::dynamic_pointer_cast<cat::MTypeItem>(curr);
						
		wxDataViewItem typeItem(clsModel.get());
		typeItemArray.Add(typeItem);

		//mConnAddObj[typeItem] = clsModel->mObjArray->ConnAfterInsert(
		//	std::bind(&VObjCatalogDataViewModel::OnObjAfterInsert, this, sph::_1, sph::_2, sph::_3));

		mConnDelObj[typeItem] = clsModel->mObjArray->ConnectBeforeRemove(
			std::bind(&VObjCatalogDataViewModel::OnObjBeforeRemove, this, sph::_1, sph::_2));

	}
	ItemsAdded(wxDataViewItem(NULL), typeItemArray);

}//OnAppend

//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnObjAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	// vec == clsModel->mObjArray
	
	auto clsItem = dynamic_cast<object_catalog::MTypeItem*>(vec.GetParent());
	wxDataViewItemArray newObjArray;
	for (const auto& curr : newItems)
	{
		wxDataViewItem objItem(curr.get());
		newObjArray.Add(objItem);
	}
	ItemsAdded(wxDataViewItem(clsItem), newObjArray);
}
//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnObjBeforeRemove(const IModel& vec,
	const std::vector<SptrIModel>& remVec)
{
	wxDataViewItemArray all;
	//GetChildren(wxDataViewItem(newVec.GetParent()), all);

	wxDataViewItemArray itemArray;
	for (const auto& remItem : remVec)
	{
		itemArray.Add(wxDataViewItem(remItem.get()));
	}
	ItemsDeleted(wxDataViewItem(vec.GetParent()), itemArray);


}//OnRemove

//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnClsBeforeRemove(const IModel& vec,
	const std::vector<SptrIModel>& remVec)
{
	wxDataViewItemArray itemArray;
	for (const auto& remItem : remVec)
	{
		wxDataViewItem dwitem(remItem.get());
		itemArray.Add(dwitem);

		mConnAddObj.erase(dwitem);
		mConnDelObj.erase(dwitem);
		mConnEditObj.erase(dwitem);

	}
	ItemsDeleted(wxDataViewItem(NULL), itemArray);
}//OnRemove
//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnClsChange(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = mCatalogModel->mTypeArray->GetChild(i);
		itemArray.Add(wxDataViewItem(item.get()));
	}
	ItemsChanged(itemArray);

}//OnChange



