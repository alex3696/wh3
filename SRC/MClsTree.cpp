#include "_pch.h"
#include "MClsTree.h"

#include "MClsProp.h"
#include "MClsAct.h"
#include "MClsMove.h"
#include "MClsObjNum.h"
#include "MClsObjQty.h"

using namespace wh;

const std::vector<Field> gClsNodeFieldVec = 
{
	{ "Имя", FieldType::ftName, true },
	{ "Комментарий", FieldType::ftText, true },
	{ "Messure", FieldType::ftName, true },
	{ "ID", FieldType::ftInt, true },
	{ "Тип", FieldType::ftName, false },
	{ "Parent", FieldType::ftName, false }
};


/*
int somefunc()
{
	gClsNodeFieldVec.emplace_back("Имя", FieldType::ftName, true);
	gClsNodeFieldVec.emplace_back("Комментарий", FieldType::ftText, true);
	gClsNodeFieldVec.emplace_back("Messure", FieldType::ftName, true);
	gClsNodeFieldVec.emplace_back("ID", FieldType::ftInt, true);
	gClsNodeFieldVec.emplace_back("Тип", FieldType::ftName, false);
	gClsNodeFieldVec.emplace_back("Parent", FieldType::ftName, false);

	return 0;
};
int ff = somefunc();
*/



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsNode::MClsNode(const char option)
//:TModelData<rec::Cls>(option)
:MClsBase(option)
{
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsNodeArray> MClsNode::GetClsArray()
{
	if (!mClsArray)
	{
		mClsArray.reset(new MClsNodeArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mClsArray);
		this->AddChild(item);
	}
	return mClsArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsPropArray> MClsNode::GetClsPropArray()
{
	if (!mPropArray)
	{
		mPropArray.reset(new MClsPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->AddChild(item);
	}
	return mPropArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsActArray> MClsNode::GetClsActArray()
{
	if (!mActArray)
	{
		mActArray.reset(new MClsActArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mActArray);
		this->AddChild(item);
	}
	return mActArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsMoveArray> MClsNode::GetClsMoveArray()
{
	if (!mMoveArray)
	{
		mMoveArray.reset(new MClsMoveArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mMoveArray);
		this->AddChild(item);
	}
	return mMoveArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsObjNumArray> MClsNode::GetClsObjNumArray()
{
	if (!mObjNumArray)
	{
		mObjNumArray.reset(new MClsObjNumArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mObjNumArray);
		this->AddChild(item);
	}
	return mObjNumArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsObjQtyArray> MClsNode::GetClsObjQtyArray()
{
	if (!mObjQtyArray)
	{
		mObjQtyArray.reset(new MClsObjQtyArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mObjQtyArray);
		this->AddChild(item);
	}
	return mObjQtyArray;
}
//-------------------------------------------------------------------------
/*
bool MClsNode::GetSelectQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"SELECT id, label, description, type, measurename, parent "
		" FROM t_class "
		//" WHERE id=%s"
		//, cls.mID);
		" WHERE label='%s' "
		, cls.mLabel);
	return true;
}
//-------------------------------------------------------------------------
bool MClsNode::GetInsertQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.IsEmpty() ? "Object" : cls.mParent;

	wxString measure;
	if ("0" == cls.mType)
		measure = "NULL";
	else if ("1" == cls.mType)
		measure = "'ед.'";
	else
		measure = "'" + cls.mMeasure + "'";

	query = wxString::Format(
		"INSERT INTO t_class "
		" (label, description, type, measurename, parent) VALUES "
		" ('%s', %s, %s, %s, '%s') "
		" RETURNING id, label, description, type, measurename, parent, vid"
		, cls.mLabel
		, cls.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", cls.mComment)
		, cls.mType
		, measure
		, parent
		);
	return true;
}
//-------------------------------------------------------------------------
bool MClsNode::GetUpdateQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.IsEmpty() ? "Object" : cls.mParent;

	wxString measure;
	if ("0" == cls.mType)
		measure = "NULL";
	else if ("1" == cls.mType)
		measure = "'ед.'";
	else
		measure = "'" + cls.mMeasure + "'";

	query = wxString::Format(
		"UPDATE t_class SET "
		" label='%s', description=%s, type=%s, measurename=%s, parent='%s' "
		" WHERE id = %s "
		, cls.mLabel
		, cls.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", cls.mComment)
		, cls.mType
		, measure
		, parent
		, cls.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MClsNode::GetDeleteQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"DELETE FROM t_class WHERE id = %s ",
		cls.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MClsNode::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel);
	table->GetAsString(2, row, data.mComment);
	table->GetAsString(3, row, data.mType);
	table->GetAsString(4, row, data.mMeasure);
	table->GetAsString(5, row, data.mParent);
	//table->GetAsString(1, row, data.mVID);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------

bool MClsNode::GetFieldValue(unsigned int col, wxVariant &val)
{
	const auto& rec = this->GetData();
	auto mgr = ResMgr::GetInstance();

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
	case 2: val = rec.mComment; break;
	case 3: val = rec.mMeasure; break;
	case 4: val = rec.mID; break;
	}//switch(col) 
	mgr->FreeInst();
	return true;
}
*/
//-------------------------------------------------------------------------
void MClsNode::LoadChilds()
{
	if (!mClsArray)
	{
		mClsArray.reset(new MClsNodeArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mClsArray);
		this->AddChild(item);
	}
	mClsArray->Load();
}
//-------------------------------------------------------------------------
/*
const std::vector<Field>& MClsNode::GetFieldVector()const
{
	return gClsNodeFieldVec;
	//return gEmptyFieldVec;
}
*/



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsNodeArray::MClsNodeArray(const char option)
:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MClsNodeArray::GetSelectChildsQuery(wxString& query)const
{
	const IModel*	parent = this->GetParent();
	const MClsNode*	clsParent = dynamic_cast<const MClsNode*>(parent);

	wxString pid = "1";
	if (clsParent)
	{
		
		const auto& clsParentData = clsParent->GetData();
		pid = clsParentData.mID;
	}

	query = wxString::Format(
		"SELECT id, label, description, type, measurename, pid FROM t_cls "
		" WHERE pid=%s "
		, pid);

	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsNodeArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsNodeArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsNode>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};
//-------------------------------------------------------------------------
const std::vector<Field>& MClsNodeArray::GetFieldVector()const
{
	return gClsNodeFieldVec;
	//return gEmptyFieldVec;
}







