#include "_pch.h"
#include "dlg_move_model_MovableObj.h"

using namespace wh;
using namespace wh::dlg_move::model;


//-----------------------------------------------------------------------------
// class DstType: public TModelData<rec::TypeNode>
//-----------------------------------------------------------------------------
DstType::DstType()
	:TModelData<DataType>(ModelOption::EnableNotifyFromChild)
	, mObjects(new DstObjArray)
{
	this->Insert(std::dynamic_pointer_cast<IModel>(mObjects));
}

//-----------------------------------------------------------------------------
std::shared_ptr<DstObjArray> DstType::GetObjects()const
{
	return mObjects;
}

//-----------------------------------------------------------------------------

void DstType::LoadChilds()
{
	//mObjects->Load();
}

//-------------------------------------------------------------------------
bool DstType::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data dst_cls;
	dst_cls.mType = "1";
	dst_cls.mId = table->GetAsString(0, row);
	dst_cls.mLabel = table->GetAsString(1, row);
	
	DstObj::DataType dst_obj;
	dst_obj.mId = table->GetAsString(2, row);
	dst_obj.mLabel = table->GetAsString(3, row);

	ObjKeyPath op;
	op.ParsePath(table->GetAsString(4, row));
	wxString path_str;
	op.GeneratePath(path_str,false);
	dst_obj.mParent.mLabel = path_str;

	auto dstObjModel = std::make_shared<DstObj>();
	dstObjModel->SetData(dst_obj);

	mObjects->Insert(dstObjModel);
	SetData(dst_cls);

	return true;
	
};


//-----------------------------------------------------------------------------
// DstTypeArray
//-----------------------------------------------------------------------------
DstTypeArray::DstTypeArray()
	: TModelArray<ItemType>(ModelOption::EnableParentNotify | ModelOption::CascadeLoad)
{
}

//-----------------------------------------------------------------------------
bool DstTypeArray::GetSelectChildsQuery(wxString& query)const 
{
	auto movableModel = dynamic_cast<MovableObj*>(GetParent());
	if (!movableModel)
		return false;
	
	const rec::PathItem& movable = movableModel->GetData();

	query = wxString::Format(
		" SELECT _dst_cls_id, acls.title as dst_cls_label "
		", _dst_obj_id, _dst_obj_label "
		", get_path_objnum(_dst_obj_pid,1) AS DST_PATH "
		" FROM lock_for_move(%s,%s) "
		" LEFT JOIN acls ON acls.id = _dst_cls_id "
		" ORDER BY "
		"   acls.title ASC "
		"   ,(substring(_dst_obj_label, '^[0-9]+')::INT, _dst_obj_label ) ASC "
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal()
		);
	return true;
}
//-----------------------------------------------------------------------------
void DstTypeArray::LoadChilds()
{
	mTypeUnique.clear();
	TModelArray::LoadChilds();
}
//-----------------------------------------------------------------------------
bool DstTypeArray::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	wxString cls_id;
	table->GetAsString(0, pos, cls_id);
	
	auto it = mTypeUnique.find(cls_id);
	if (mTypeUnique.end() != it)
	{
		DstObj::DataType dst_obj;

		dst_obj.mId = table->GetAsString(2, pos);
		dst_obj.mLabel = table->GetAsString(3, pos);

		ObjKeyPath op;
		op.ParsePath(table->GetAsString(4, pos));
		wxString path_str;
		op.GeneratePath(path_str, false);
		dst_obj.mParent.mLabel = path_str;

		
		auto dstObjModel = std::make_shared<DstObj>();
		dstObjModel->SetData(dst_obj,true);
					
		it->second->mObjects->Insert(dstObjModel);
		return false;
	}

	
	bool is_loaded =  TModelArray::LoadChildDataFromDb(child, table, pos);
	if (is_loaded)
	{
		auto dst_type = std::dynamic_pointer_cast<DstType>(child);
		mTypeUnique.emplace(cls_id, dst_type);
	}
	return is_loaded;
}