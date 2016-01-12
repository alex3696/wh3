#include "_pch.h"
#include "dlg_act_model_Act.h"
#include "dlg_act_model_Obj.h"

using namespace wh;
using namespace wh::dlg_act::model;


//-----------------------------------------------------------------------------
// Act
//-----------------------------------------------------------------------------
Act::Act(const char option)
	: TModelData<DataType>(option)
	, mPropArray(new PropArray)
{
	this->Insert(std::dynamic_pointer_cast<IModel>(mPropArray));
}

//-----------------------------------------------------------------------------
std::shared_ptr<PropArray> Act::GetPropArray()const
{
	return mPropArray;
}

//-----------------------------------------------------------------------------

void Act::LoadChilds()
{
	mPropArray->Load();
}

//-------------------------------------------------------------------------
bool Act::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel );
	table->GetAsString(2, row, data.mComment );
	table->GetAsString(3, row, data.mColor );
	table->GetAsString(4, row, data.mScript);
	

	SetData(data);
	return true;
};
//-----------------------------------------------------------------------------
void Act::DoAct()
{
	whDataMgr::GetDB().BeginTransaction();

	auto act_array = this->GetParent();
	auto obj_model = dynamic_cast<dlg_act::model::Obj*>(act_array->GetParent());

	const rec::PathItem& subj = obj_model->GetData();


	wxString propdata = "{";

	unsigned int propQty = this->mPropArray->GetChildQty();

	for (unsigned int i = 0; i < propQty; ++i)
	{
		auto prop = std::dynamic_pointer_cast<model::Prop>(this->mPropArray->GetChild(i));
		if (prop)
		{
			auto propData = prop->GetData();
			wxString propval;
			if (propData.mProp.mType == ftLong || propData.mProp.mType == ftDouble)
				propval = propData.mVal.toStr();
			else
				propval = wxString::Format("\"%s\"",propData.mVal.toStr());

			propdata += wxString::Format("\"%s\":%s", 
				propData.mProp.mId.toStr(), propval);
			if (propQty - 1!= i )
				propdata += ",";
		}
	}
	if (propdata.size()>1)

	propdata += "}";


	wxString query = wxString::Format(
		"SELECT do_act(%s, %s, '%s')"
		, subj.mObj.mId.SqlVal()
		, this->GetData().mID
		, propdata
		);
	
	int result = whDataMgr::GetDB().Exec(query);
	if (result)
	{
		whDataMgr::GetDB().RollBack();
		whDataMgr::GetDB().BeginTransaction();
	}

	query = wxString::Format(
		"SELECT lock_reset(%s,%s)"
		, subj.mObj.mId.SqlVal()
		, subj.mObj.mParent.mId.SqlVal() );
	whDataMgr::GetDB().Exec(query);


	whDataMgr::GetDB().Commit();
}

//-----------------------------------------------------------------------------
// ActArray
//-----------------------------------------------------------------------------
ActArray::ActArray()
	: TModelArray<ItemType>(ModelOption::EnableParentNotify /*| ModelOption::CascadeLoad*/)
{
}

//-----------------------------------------------------------------------------
bool ActArray::GetSelectChildsQuery(wxString& query)const
{
	auto actObjModel = dynamic_cast<Obj*>(GetParent());
	if (!actObjModel)
		return false;

	const rec::PathItem& actObj = actObjModel->GetData();

	query = wxString::Format(
		" SELECT id, title, note, color, script "
		" FROM lock_for_act(%s, %s)"
		, actObj.mObj.mId.SqlVal()
		, actObj.mObj.mParent.mId.SqlVal()
		);
	return true;

}
//-----------------------------------------------------------------------------
/*
void ActArray::LoadChilds()
{
	mTypeUnique.clear();
	TModelArray::LoadChilds();
}

//-----------------------------------------------------------------------------
bool ActArray::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	wxString cls_id;
	table->GetAsString(0, pos, cls_id);

	auto it = mTypeUnique.find(cls_id);
	if (mTypeUnique.end() != it)
	{
		DstObj::DataType dst_obj;

		dst_obj.mID = table->GetAsString(2, pos);
		dst_obj.mLabel = table->GetAsString(3, pos);
		dst_obj.mPID = ObjArrayToPath(table->GetAsString(4, pos));

		auto dstObjModel = std::make_shared<DstObj>();
		dstObjModel->SetData(dst_obj, true);

		it->second->mObjects->AddChild(std::dynamic_pointer_cast<IModel>(dstObjModel));
		return false;
	}


	bool is_loaded = TModelArray::LoadChildDataFromDb(child, table, pos);
	if (is_loaded)
	{
		auto dst_type = std::dynamic_pointer_cast<DstType>(child);
		mTypeUnique.emplace(cls_id, dst_type);
	}
	return is_loaded;
}
*/