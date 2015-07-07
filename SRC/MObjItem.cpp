#include "_pch.h"
#include "MObjItem.h"
#include "MTypeNode.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MObjItem::MObjItem(const char option)
	:TModelData<rec::Obj>(option)
{
}

//-------------------------------------------------------------------------
bool MObjItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	//" SELECT id, pid, obj_label, class_label, class_type, qty, last_log_id "
	T_Data data;

	unsigned int col = 0;

	table->GetAsString(col++, row, data.mID);
	table->GetAsString(col++, row, data.mPID);
	table->GetAsString(col++, row, data.mLabel);
	table->GetAsString(col++, row, data.mQty);
	table->GetAsString(col++, row, data.mLastLogId);

	table->GetAsString(col++, row, mPath);
	while (col < table->GetColumnCount())
	{
		wxString str_data;
		table->GetAsString(col, row, str_data);
		data.mProp.emplace_back(str_data);
		col++;
	}


	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
const MTypeItem* MObjItem::GetCls()const
{
	auto objArray = GetParent();
	if (objArray)
	{
		auto clsModel = objArray->GetParent();
		if (clsModel)
			return dynamic_cast<MTypeItem*>(clsModel);
	}
	return nullptr;
}
//-------------------------------------------------------------------------
wxString MObjItem::GetPathString() 
{
	const auto obj_array = this->GetParent();
	if (obj_array)
	{
		const auto cls = dynamic_cast<MTypeItem*>(obj_array->GetParent());
		if (cls)
		{ 
			const auto cls_array = dynamic_cast<MTypeArray*>(cls->GetParent());
			if (cls_array)
			{ 
				const auto catalog = dynamic_cast<MObjCatalog*>(cls_array->GetParent());
				if (catalog)
				{
					if (catalog->mCfg->GetData().mObjCatalog)
					{
						const auto obj_data = this->GetData();
						const auto cls_data = cls->GetData();

						return catalog->mPath->GetPathStr() 
							+ wxString::Format("[%s]%s/"
							, cls_data.mLabel
							, obj_data.mLabel);
					}
					else
					{
						auto path = std::make_shared<model::ObjPath>();
						this->AddChild(std::dynamic_pointer_cast<IModel>(path));
						path->Load();
						return path->AsString();
					}
				}
			}//if (cls_array)
		}//if (cls)
	} //if(obj_array)
	return wxEmptyString;
}
//-------------------------------------------------------------------------
bool MObjItem::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MObjArray*>(this->mParent);
	auto parentCls = dynamic_cast<MTypeItem*>(parentArray->GetParent());

	const rec::Cls& cls = parentCls->GetStored();
	const auto& newObj = this->GetData();
	wxString pid = (newObj.mPID.IsEmpty()) ? "0" : newObj.mPID;

	ClsType ct;
	if (cls.GetClsType(ct))
		switch (ct)
		{
		case ctSingle:
			query = wxString::Format(
				"INSERT INTO t_objnum (cls_id, obj_label, pid ) "
				" VALUES (%s, '%s', %s ) RETURNING id ",
				cls.mID, newObj.mLabel, pid);
			return true;
		case ctQtyByOne:
		case ctQtyByFloat:	
			query = wxString::Format(
				" SELECT fn_insert_objqty(%s, '%s', %s, %s) "
				, cls.mID, newObj.mLabel, pid, newObj.mQty);
			return true;
		default://ctAbstract
			break;
		}
	
	return false;
}
//-------------------------------------------------------------------------
bool MObjItem::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MObjArray*>(this->mParent);
	auto parentCls = dynamic_cast<MTypeItem*>(parentArray->GetParent());

	const rec::Cls& cls = parentCls->GetStored();
	const auto& oldObj = this->GetStored();
	const auto& newObj = this->GetData();
	wxString pid = (newObj.mPID.IsEmpty()) ? "0" : newObj.mPID;

	ClsType ct;
	if (cls.GetClsType(ct))
		switch (ct)
	{
		case ctSingle:
			query = wxString::Format(
				"UPDATE t_objnum SET "
				" obj_label='%s', pid=%s "
				" WHERE id=%s ",
				newObj.mLabel, newObj.mPID,
				oldObj.mID);
			return true;

		case ctQtyByOne:
		case ctQtyByFloat:
			query = wxString::Format(
				" SELECT fn_update_objqty(%s, %s, '%s', %s, %s) "
				, oldObj.mID, oldObj.mPID
				, newObj.mLabel, newObj.mPID, newObj.mQty
				);
			return true;
		default://ctAbstract
			break;
	}
	return false;
}


//-------------------------------------------------------------------------
bool MObjItem::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MObjArray*>(this->mParent);
	auto parentCls = dynamic_cast<MTypeItem*>(parentArray->GetParent());

	const rec::Cls& cls = parentCls->GetStored();
	const auto& oldObj = this->GetStored();

	ClsType ct;
	if (cls.GetClsType(ct))
		switch (ct)
	{
		case ctSingle:
			query = wxString::Format(
				"DELETE FROM t_objnum WHERE id=%s "
				, oldObj.mID);
			return true;
		case ctQtyByOne:
		case ctQtyByFloat:
			query = wxString::Format(
				"SELECT fn_delete_objqty(%s, %s)"
				, oldObj.mID, oldObj.mPID	);
			return true;
		default://ctAbstract
			break;
	}
	return false;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MObjArray::GetSelectChildsQuery(wxString& query)const
{
	auto typeItemModel = dynamic_cast<MTypeItem*>(this->GetParent());
	if (typeItemModel)
	{
		auto typeArrayModel = dynamic_cast<MTypeArray*>(typeItemModel->GetParent());
		auto catalogModel = dynamic_cast<MObjCatalog*>(typeArrayModel->GetParent());

		const auto& typeItemData = typeItemModel->GetData();
		const auto& catalogData = catalogModel->GetData();

		wxString qq;
		for (const auto& it : catalogModel->GetFavProps())
		{
			auto typeIt = it.mCls->find(typeItemData.mID);
			if (it.mCls->end() != typeIt)
				qq += wxString::Format(", \"%s\"", it.mLabel);
			else
				qq += wxString::Format(",NULL AS \"%s\"", it.mLabel);
		}


		wxString leftJoin;
		
		if ("1" != typeItemData.mID && "1" == typeItemData.mType)
			leftJoin =wxString::Format(
				" LEFT JOIN t_state_%s USING(obj_id) "
				, typeItemData.mID
				);

		if (catalogModel->mCfg->GetData().mObjCatalog)
		{
			query = wxString::Format(
				"SELECT w_obj.obj_id, obj_pid, w_obj.obj_label, qty "
				", last_log_id, NULL AS path %s "
				" FROM w_obj "
				" %s "
				" WHERE obj_pid = %s AND w_obj.cls_id = %s "
				, qq
				, leftJoin
				, catalogData.mObj.mID
				, typeItemData.mID
				);
			return true;
		}
		else
		{
			query = wxString::Format(
				"SELECT w_obj.obj_id, obj_pid, w_obj.obj_label, qty "
				 ", last_log_id, get_path( obj_pid) %s "
				" FROM w_obj "
				" %s "
				" WHERE w_obj.cls_id = %s "
				, qq
				, leftJoin
				, typeItemData.mID
				);
			return true;
		}
	}
	return false;
}

