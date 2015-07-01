#include "_pch.h"
#include "MClsFavoriteActProp.h"
#include "MClsActProp.h"

//#include "MClsActProp.h"


using namespace wh;


const std::vector<Field> gClsNodeFieldVec =
{
	{ "Имя", FieldType::ftName, true },
	{ "Свойства", FieldType::ftText, true },
	//{ "Комментарий", FieldType::ftText, true },
	//{ "Messure", FieldType::ftName, true },
	//{ "ID", FieldType::ftInt, true },
	//{ "Тип", FieldType::ftName, false },
	//{ "Parent", FieldType::ftName, false }
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsFavorite::MClsFavorite(const char option)
	:MClsBase(option)
{
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsFavoriteArray> MClsFavorite::GetClsArray()
{
	if (!mClsArray)
	{
		mClsArray.reset(new MClsFavoriteArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mClsArray);
		this->AddChild(item);
	}
	return mClsArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsActPropArray> MClsFavorite::GetPropArray()
{
	
	if (!mPropArray)
	{
		mPropArray.reset(new MClsActPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->AddChild(item);
	}
	return mPropArray;
}
//-------------------------------------------------------------------------
void MClsFavorite::LoadChilds()
{
	if (!mClsArray)
	{
		mClsArray.reset(new MClsFavoriteArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mClsArray);
		this->AddChild(item);
	}
	mClsArray->Load();

	//for (size_t i = 0; i < mClsArray->GetChildQty(); i++)
	//{
	//	auto item = std::dynamic_pointer_cast<MClsFavorite>(mClsArray->GetChild(i));
	//	auto propArray = item->GetPropArray();
	//	propArray->Load();
	//}


	if (!mPropArray)
	{
		mPropArray.reset(new MClsActPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->AddChild(item);
	}
	mPropArray->Load();


	
}
//-------------------------------------------------------------------------
bool MClsFavorite::GetFieldValue(unsigned int col, wxVariant &val)
{
	const auto& rec = this->GetData();
	auto mgr = ResMgr::GetInstance();

	wxString tmpPropArr;



	switch (col)
	{
	default: break;
	case 1:	if ("0" == rec.mType)
		val << wxDataViewIconText(rec.mLabel, mgr->m_ico_type_abstract24);
			else if ("1" == rec.mType)
				val << wxDataViewIconText(rec.mLabel, mgr->m_ico_type_num24);
			else
				val << wxDataViewIconText(rec.mLabel, mgr->m_ico_type_qty24);
			break;
	case 2: 
		if (mPropArray)
			for (unsigned int i = 0; i < mPropArray->GetChildQty(); ++i)
			{
				auto child = mPropArray->GetChild(i);
				auto propModel = std::dynamic_pointer_cast<MClsActProp>(child);
				const auto& prop = propModel->GetData();
				tmpPropArr += " (" + prop.mActLabel + ": " + prop.mPropLabel + ")";
			}
		val = tmpPropArr; break;
	}//switch(col) 
	mgr->FreeInst();
	return true;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsFavoriteArray::MClsFavoriteArray(const char option)
	:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MClsFavoriteArray::GetSelectChildsQuery(wxString& query)const
{
	const IModel*	parent = this->GetParent();
	const MClsFavorite*	clsParent = dynamic_cast<const MClsFavorite*>(parent);

	wxString pid="1";
	if (clsParent)
	{
		const auto& clsParentData = clsParent->GetData();
		pid = clsParentData.mID;
	}

	query = wxString::Format(
		"SELECT id, label, description, type, measurename, pid FROM t_cls "
		" WHERE pid='%s' "
		, pid);

	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsFavoriteArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}
//-------------------------------------------------------------------------
void MClsFavoriteArray::LoadChilds()
{
	IModel::LoadChilds();



	for (size_t i = 0; i < this->GetChildQty(); i++)
	{
		auto item = std::dynamic_pointer_cast<MClsFavorite>(this->GetChild(i));
		auto propArray = item->GetPropArray();
		propArray->Load();
	}

}
//-------------------------------------------------------------------------
bool MClsFavoriteArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsFavorite>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};
//-------------------------------------------------------------------------
const std::vector<Field>& MClsFavoriteArray::GetFieldVector()const
{
	return gClsNodeFieldVec;
}







