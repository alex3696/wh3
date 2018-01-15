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
	auto data_ptr = std::make_shared<T_Data>();
	T_Data& data = *data_ptr.get();
	//T_Data data;

	unsigned int col = 0;

	data.mId = table->GetAsString(col++, row);
	data.mParent.mId = table->GetAsString(col++, row);
	data.mLabel = table->GetAsString(col++, row);
	data.mQty = table->GetAsString(col++, row);
	data.mLastMoveLogId = table->GetAsString(col++, row);
	table->GetAsString(col++, row, mPath);
	//data.mParent.mLabel = table->GetAsString(col++, row);

	const auto prp_qty = table->GetColumnCount();
	data.mProp.resize(prp_qty-col);
	for (unsigned int i = col; i < prp_qty; ++i)
	{
		if (!table->IsFieldNull(i, row))
			table->GetAsString(i, row, data.mProp[i - col]);
	}

	SetData(data_ptr, true, false);
	//SetData(data, true);

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
					if (catalog->IsObjTree())
					{
						return catalog->mPath->GetPathStr();
					}
					else
					{
						auto path = std::make_shared<model::ObjPath>();
						this->Insert(std::dynamic_pointer_cast<IModel>(path));
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

	wxString pid = "NULL";
	if (!newObj.mParent.mId.IsNull())
		pid = newObj.mParent.mId;
	else
	{
		if (cls.mDefaultObj.mId.IsNull())
			pid = "1";
		else
			pid = cls.mDefaultObj.mId;
	}

	if (!cls.mType.IsNull() )
	{
		query = wxString::Format(
			"INSERT INTO obj( title, cls_id, pid, qty )"
			" VALUES(%s, %s, %s, %s) "
			" RETURNING id, pid, title, qty, NULL, NULL"
			, newObj.mLabel.SqlVal()
			, cls.mId.SqlVal()
			, pid
			, newObj.mQty.SqlVal()
			);
		return true;
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

	wxString pid = "NULL";
	if (!newObj.mParent.mId.IsNull())
		pid = newObj.mParent.mId;
	else
	{
		if (cls.mDefaultObj.mId.IsNull())
			pid = "1";
		else
			pid = cls.mDefaultObj.mId;
	}

	//TODO: сделать тригер не позвол€ющий мен€ть если объекта нет в таблице блокировки
	if (!cls.mType.IsNull())
	{
		query = wxString::Format(
		"UPDATE obj SET title=%s WHERE id=%s AND cls_id=%s AND pid=%s "
			, newObj.mLabel.SqlVal()
			, oldObj.mId.SqlVal()
			, cls.mId.SqlVal()
			, oldObj.mParent.mId.SqlVal()
			);
		return true;
	}
	return false;
}


//-------------------------------------------------------------------------
bool MObjItem::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MObjArray*>(this->mParent);
	if (!parentArray)
		return false;
	auto parentCls = dynamic_cast<MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;
	if (msNull == parentCls->GetState() || msNull == GetState())
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	const auto& oldObj = this->GetStored();

	if (!cls.mType.IsNull() )
	{ 		
		query = wxString::Format(
			"DELETE FROM obj WHERE "
			" id=%s AND cls_id=%s AND pid=%s "
			, oldObj.mId.SqlVal()
			, cls.mId.toStr()
			, oldObj.mParent.mId.SqlVal()
			);
		return true;
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
		auto catalog = dynamic_cast<MObjCatalog*>(typeArrayModel->GetParent());

		const auto& typeItemData = typeItemModel->GetData();
		const auto& catalogData = catalog->GetRoot();

		wxString fields, leftJoin;
		if (catalog->IsPropEnabled()) //ctSingle == typeItemData.GetClsType()
		{
			wxString qq;
			for (const auto& it : catalog->GetFavProps())
				qq += wxString::Format("\"%s\" TEXT,", it.mID);
			
			if (!qq.IsEmpty())
			{
				qq.replace(qq.size() - 1, 1, " ");
				leftJoin = wxString::Format(" LEFT JOIN LATERAL jsonb_to_record(o.prop) as x(%s) ON true "
					, qq);
				fields = ", x.*";
			}
		}

		if (catalog->IsObjTree())
		{
			query = wxString::Format(
				" SELECT o.id, o.pid, o.title, o.qty "
				" , o.move_logid, NULL AS path "
				//" , parent.title "
				"   %s "
				" FROM obj o "
				" %s "
				//" LEFT JOIN obj_name parent ON parent.id = o.pid "
				" WHERE o.id>0 AND o.pid = %s AND o.cls_id = %s "
				" ORDER BY "
				"   (substring(o.title, '^[0-9]{1,9}')::INT, o.title ) ASC "
				, fields
				, leftJoin
				, catalogData.mObj.mId.toStr()
				, typeItemData.mId.SqlVal()
				);
			return true;
		}
		else
		{
			query = wxString::Format(
				"SELECT o.id, o.pid, o.title, o.qty "
				" , o.move_logid, get_path_objnum(o.pid,1)  AS path "
				//" , parent.title "
				"   %s "
				" FROM obj o "
				" %s "
				//" LEFT JOIN obj_name parent ON parent.id = o.pid "
				" WHERE o.id>0 AND o.cls_id = %s "
				" ORDER BY "
				"   (substring(o.title, '^[0-9]{1,9}')::INT, o.title ) ASC "
				, fields
				, leftJoin
				, typeItemData.mId.SqlVal()
				);
			return true;
		}
	}
	return false;
}

