#include "_pch.h"
#include "dlg_move_view_DvModel.h"


using namespace wh;
using namespace wh::dlg_move::view;


DvModel::DvModel()
	:wxDataViewModel()
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
bool DvModel::IsContainer(const wxDataViewItem &dataViewItem)const
{
	if (!dataViewItem.IsOk())
		return true;

	auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
	auto typeItem = dynamic_cast<model::DstType*> (modelInterface);

	if (typeItem && !typeItem->IsAbstract() )
		return true;
	return false;

}
//---------------------------------------------------------------------------

void DvModel::GetValue(wxVariant &variant, const wxDataViewItem &dataViewItem, unsigned int col) const
{
	if (!dataViewItem.IsOk())
	{
		switch (col)
		{
		default:	break;
		case 1:		variant << wxDataViewIconText(wxEmptyString, wxNullIcon);
			break;
		}
		return;
	}

	auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
	auto typeItem = dynamic_cast<model::DstType *> (modelInterface);
	auto objItem = dynamic_cast<model::DstObj*> (modelInterface);

	if (typeItem)
	{
		const auto& cls = typeItem->GetData();
		if (!cls.mType.IsNull())
		{
			auto mgr = ResMgr::GetInstance();
			wxIcon* ico = &wxNullIcon;
			switch (cls.GetClsType())
			{
			default/*0*/:		ico = &mgr->m_ico_type_abstract24;	break;
			case ctQtyByFloat:	
			case ctQtyByOne:	ico = &mgr->m_ico_type_qty24;		break;
			case ctSingle:		ico = &mgr->m_ico_type_num24;		break;
			} // switch (clsType)

			variant << wxDataViewIconText( cls.mLabel, *ico);
		} // if (cls.GetClsType(clsType))
	}
	else if (objItem)
	{
		const auto& obj = objItem->GetData();

		switch (col)
		{
		case 1:		variant << wxDataViewIconText(obj.mLabel, wxNullIcon);	break;
		case 2:		variant = obj.mParent.mLabel.toStr();	break;
		case 3:		variant = obj.mId.toStr();			break;
		//case 4:		variant = obj.mLastLogId;	break;
		default:	break;
		}

	}


}
//---------------------------------------------------------------------------
bool DvModel::GetAttr(const wxDataViewItem &dataViewItem, unsigned int col, wxDataViewItemAttr &attr) const
{
	/*
	if (dataViewItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
		auto objItem = dynamic_cast<model::DstObj*> (modelInterface);
		model::DstType* typeItem(nullptr);

		bool has_bg = true;
		if (!objItem)
		{

			typeItem = dynamic_cast<model::DstType*> (modelInterface);
			if (col > 5)
				attr.SetBackgroundColour(wxColour(240, 240, 240));
			//return true;

		}
		else
		{
			attr.SetBold(true);
			auto objArray = dynamic_cast<model::DstTypeArray*> (objItem->GetParent());
			typeItem = dynamic_cast<model::DstType*> (objArray->GetParent());

			auto typeArray = dynamic_cast<model::DstTypeArray*> (typeItem->GetParent());
			auto catalog = dynamic_cast<model::MovableObj*> (typeArray->GetParent());

			unsigned long clsType;
			if (typeItem->GetData().GetClsType(clsType) && clsType != 1 && col > 5)
				//attr.SetBackgroundColour(wxColour(240, 240, 240));
				has_bg = false;
			else
			{
				auto favProp = catalog->GetFavProps();
				if (col > 5 && favProp.size())
				{
					const auto& field = favProp[col - 6];
					const auto& typeId = typeItem->GetData().mClsId;

					auto it = field.mCls->find(typeId);
					if (field.mCls->end() == it)
						//attr.SetBackgroundColour(wxColour(240, 240, 240));
						has_bg = false;
				}
			}


		}


		if (typeItem)
		{
			unsigned long	clsType;
			const auto& cls = typeItem->GetData();
			if (cls.GetClsType(clsType))
			{
				switch (clsType)
				{
				//0
				default: attr.SetColour(wxColour(100, 100, 100)); break;
				case 3:
				case 2: attr.SetColour(wxColour(0, 80, 150));
					if (has_bg)
						attr.SetBackgroundColour(wxColour(240, 250, 255));
					break;
				case 1: attr.SetColour(wxColour(100, 30, 0));
					if (has_bg)
						attr.SetBackgroundColour(wxColour(250, 250, 245));
					break;
				}//switch
			}//if (cls.GetClsType(clsType))
		}//if (typeItem)

		return true;
	}//if (dataViewItem.IsOk())
	*/
	return false;
}
//---------------------------------------------------------------------------

bool DvModel::SetValue(const wxVariant &variant, const wxDataViewItem &item,
	unsigned int col)
{
	return false;
}
//---------------------------------------------------------------------------

wxDataViewItem DvModel::GetParent(const wxDataViewItem &dataViewItem) const
{
	if (dataViewItem.IsOk())
	{

		auto modelInterface = static_cast<IModel*> (dataViewItem.GetID());
		auto typeItem = dynamic_cast<model::DstType*> (modelInterface);
		if (typeItem)
			return wxDataViewItem(NULL);
		else
		{
			auto objItem = dynamic_cast<model::DstObj*> (modelInterface);
			if (objItem)
			{
				auto objArray = dynamic_cast<model::DstObjArray*> (objItem->GetParent());
				if (objArray)
				{
					auto typeItem = dynamic_cast<model::DstType*> (objItem->GetParent());
					if (typeItem)
						return wxDataViewItem(typeItem);
				}
			}
		}
	}
	return wxDataViewItem(NULL);// the invisible root node has no parent
}
//---------------------------------------------------------------------------
unsigned int DvModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
{
	if (!mMovable)
		return 0;

	unsigned int children_count(0);

	if (parent.IsOk()) // если есть родитель ( родитель==группа ), загружаем объекты
	{
		auto modelInterface = static_cast<IModel*> (parent.GetID());
		auto typeItem = dynamic_cast<model::DstType*>(modelInterface);

		auto objArrayModel = typeItem->GetObjects();

		children_count = objArrayModel->GetChildQty();
		for (unsigned int i = 0; i < children_count; ++i)
		{
			auto objItem = objArrayModel->GetChild(i);
			array.Add(wxDataViewItem(objItem.get()));

		}
	}
	else // если родителя нет(т.е. родитель корень NULL) то загружаем типы
	{
		children_count = mMovable->GetDstTypes()->GetChildQty();
		for (unsigned int i = 0; i < children_count; ++i)
		{
			auto typeItem = mMovable->GetDstTypes()->GetChild(i);
			array.Add(wxDataViewItem(typeItem.get()));

		}
	}
	return children_count;
}

//---------------------------------------------------------------------------
void DvModel::SetModel(std::shared_ptr<IModel> model)
{
	mConnClsAppend.disconnect();
	mConnClsRemove.disconnect();
	mConnClsChange.disconnect();

	mMovable = std::dynamic_pointer_cast<model::MovableObj>(model);

	if (!mMovable)
		return;

	namespace sph = std::placeholders;

	mConnClsAppend = mMovable->GetDstTypes()->ConnAfterInsert(
		std::bind(&DvModel::OnAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnClsRemove = mMovable->GetDstTypes()->ConnectBeforeRemove(
		std::bind(&DvModel::OnClsBeforeRemove, this, sph::_1, sph::_2));
	mConnClsChange = mMovable->GetDstTypes()->ConnectChangeSlot(
		std::bind(&DvModel::OnClsChange, this, sph::_1, sph::_2));

}
//---------------------------------------------------------------------------
void DvModel::ClearModel()
{
	wxDataViewItemArray itemArray;
	GetChildren(wxDataViewItem(NULL), itemArray);
	this->ItemsDeleted(wxDataViewItem(NULL), itemArray);
}


//---------------------------------------------------------------------------
void DvModel::OnAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore)
{
	// vec = mMovable->GetDstTypes()
	for (const auto& curr : newItems)
	{
		auto typeModel = std::dynamic_pointer_cast<model::DstType>(curr);
		
		wxDataViewItem typeItem(typeModel.get());
		ItemAdded(wxDataViewItem(NULL), typeItem);

		auto objArray = typeModel->GetObjects();
		wxDataViewItemArray objItemArray;

		//for (unsigned int j = 0; j < objArray->GetChildQty(); ++j)
		//{
		//	wxDataViewItem objItem(objArray->GetChild(j).get());
		//	objItemArray.Add(objItem);
		//}
		//ItemsAdded(typeItem, objItemArray);
	}

}//OnAppend
//---------------------------------------------------------------------------
void DvModel::OnClsBeforeRemove(const IModel& vec,
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
void DvModel::OnClsChange(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = mMovable->GetDstTypes()->GetChild(i);
		itemArray.Add(wxDataViewItem(item.get()));
	}
	ItemsChanged(itemArray);

}//OnChange



