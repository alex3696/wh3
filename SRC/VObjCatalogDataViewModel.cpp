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
	return !col ? "wxDataViewIconText" : "string";
}
//---------------------------------------------------------------------------
bool VObjCatalogDataViewModel::IsContainer(const wxDataViewItem &dataViewItem)const
{
	if (!dataViewItem.IsOk())
		return true;

	auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
	auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);

	if (typeItem)
	{ 
		auto typeArray = typeItem->GetParent();
		if (typeArray)
		{ 
			auto catalog = dynamic_cast<object_catalog::MObjCatalog*>(typeArray->GetParent());
			if (catalog)
			{
				if (catalog->IsObjEnabled())
				{
					const auto& cls_data = typeItem->GetData();
					return !cls_data.IsAbstract();
				}
				else
					return false;
			}
				
		}
	}
	return typeItem ? true : false;
}
//---------------------------------------------------------------------------

void VObjCatalogDataViewModel::GetValue(wxVariant &variant, const wxDataViewItem &dataViewItem, unsigned int col) const
{
	wxString val;
	wxIcon*  ico(&wxNullIcon);
	
	
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
			
			default: //ctAbstract
				if (1 == col)
				{
					ico = &mgr->m_ico_type_abstract24;
					val = wxString::Format("%s", cls.mLabel.toStr());
				}
				break;
			case ctQtyByFloat:
			case ctQtyByOne:ico = &mgr->m_ico_type_qty24;
							val = wxString::Format("%s - %s (%s)",
							//cls.mClsLabel, cls.mQty, cls.mMeasure);
							cls.mLabel.toStr(), typeItem->GetQty(), cls.mMeasure.toStr());
							break;
			case ctSingle:	ico = &mgr->m_ico_type_num24;
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
		case 1:		val = obj.mLabel;	break;
		case 2:		val = obj.mQty + " (" + typeItem->GetData().mMeasure.toStr() + ")"; break;
		case 3:		val = obj.mId;				break;
		case 4:		val = obj.mParent.mId;		break;
		case 5:		val = obj.mLastMoveLogId;	break;
		case 6:		val = objItem->mPath;		break;
		default:	if (obj.mProp.size() > (col - 7) )
						val = obj.mProp[col - 7];
					break;
		}
			
	}

	switch (col)
	{
	case 1:		variant << wxDataViewIconText(val, *ico);
		break;
	default:	
		if (objItem)
			variant = val;
		break;
	}
}
//---------------------------------------------------------------------------
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
				&& ctSingle != typeItem->GetData().GetClsType()
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
				case ctQtyByFloat:
				case ctQtyByOne: 
						attr.SetColour(wxColour(0, 80, 150));
						if (has_bg)
							attr.SetBackgroundColour(wxColour(210, 240, 250));
						break;
				case ctSingle: 
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

	if (parent.IsOk()) // если есть родитель ( родитель==группа ), загружаем объекты
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
	else // если родителя нет(т.е. родитель корень NULL) то загружаем типы
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

	mConnClsAppend = mCatalogModel->mTypeArray->ConnectAppendSlot(
		std::bind(&VObjCatalogDataViewModel::OnClsAppend, this, sph::_1, sph::_2));
	mConnClsRemove = mCatalogModel->mTypeArray->ConnectBeforeRemove(
		std::bind(&VObjCatalogDataViewModel::OnClsRemove, this, sph::_1, sph::_2));
	mConnClsChange = mCatalogModel->mTypeArray->ConnectChangeSlot(
		std::bind(&VObjCatalogDataViewModel::OnClsChange, this, sph::_1, sph::_2));

	mCfg = &mCatalogModel->mCfg->GetData();
//	void OnAppend(const IModel&, const std::vector<unsigned int>&);
//	void OnRemove(const IModel&, const std::vector<unsigned int>&);
//	void OnChange(const IModel&, const std::vector<unsigned int>&);

}
//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::ClearModel()
{
	wxDataViewItemArray itemArray;
	GetChildren(wxDataViewItem(NULL), itemArray);
	this->ItemsDeleted(wxDataViewItem(NULL), itemArray);
}


//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnClsAppend(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	namespace sph = std::placeholders; 

	auto typeArray = mCatalogModel->mTypeArray;

	wxDataViewItemArray typeItemArray;
	for (const unsigned int& i : itemVec)
	{
		auto clsModel = std::dynamic_pointer_cast<object_catalog::MTypeItem>
			(typeArray->GetChild(i));
						
		wxDataViewItem typeItem(clsModel.get());
		typeItemArray.Add(typeItem);

		mConnAddObj[typeItem] = clsModel->mObjArray->ConnectAppendSlot(
			std::bind(&VObjCatalogDataViewModel::OnObjAppend, this, sph::_1, sph::_2));

		mConnDelObj[typeItem] = clsModel->mObjArray->ConnectBeforeRemove(
			std::bind(&VObjCatalogDataViewModel::OnObjRemove, this, sph::_1, sph::_2));

	}
	ItemsAdded(wxDataViewItem(NULL), typeItemArray);

}//OnAppend

//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnObjAppend(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	auto clsItem = dynamic_cast<object_catalog::MTypeItem*>(newVec.GetParent());
	
	wxDataViewItemArray newObjArray;
	for (const unsigned int& i : itemVec)
	{
		auto child = newVec.GetChild(i);
		wxDataViewItem objItem(child.get());
		newObjArray.Add(objItem);
	}
	ItemsAdded(wxDataViewItem(clsItem), newObjArray);
}
//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnObjRemove(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray all;
	//GetChildren(wxDataViewItem(newVec.GetParent()), all);

	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = newVec.GetChild(i);
		itemArray.Add(wxDataViewItem(item.get()));
	}
	ItemsDeleted(wxDataViewItem(newVec.GetParent()), itemArray);


}//OnRemove

//---------------------------------------------------------------------------
void VObjCatalogDataViewModel::OnClsRemove(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = mCatalogModel->mTypeArray->GetChild(i);
		wxDataViewItem dwitem(item.get());
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



