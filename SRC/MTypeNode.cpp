#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;

const std::vector<Field> gTypeItemFieldVec =
{
	{ "Тип", FieldType::ftName, true },
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MTypeItem::MTypeItem(const char option)
	:TModelData<rec::Cls>(option)
	, mObjArray(new MObjArray)
	, mFavProp(new dlg::favprop::model::FavPropArray)
{
	this->Insert(std::dynamic_pointer_cast<IModel>(mObjArray));
	this->Insert(std::dynamic_pointer_cast<IModel>(mFavProp));
}




//-------------------------------------------------------------------------
bool MTypeItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data = this->GetData();
	data.mLabel = table->GetAsString(0, row);
	data.mId = table->GetAsLong(1, row);
	data.mType = table->GetAsString(2, row);
	data.mMeasure = table->GetAsString(3, row);
	mQty = table->GetAsString(4, row);
	data.mDefaultObj.mId = table->GetAsString(5, row);
	data.mDefaultObj.mLabel = table->GetAsString(6, row);
	
	auto type_array = dynamic_cast<MTypeArray*>(this->GetParent());
	if (type_array)
	{
		auto catalog = dynamic_cast<MObjCatalog*>(type_array->GetParent());
		if (catalog)
			data.mParent = catalog->GetRoot().mCls.mParent;
	}
	

	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
void MTypeItem::LoadChilds()
{
	auto type_array = dynamic_cast<MTypeArray*>(this->GetParent());
	auto catalog = dynamic_cast<MObjCatalog*>(type_array->GetParent());
	if (!catalog)
		return;

	if (catalog->mAutoLoadObj->GetData() && catalog->IsObjEnabled())
		mObjArray->Load();
	
}
//-------------------------------------------------------------------------
wxString MTypeItem::GetQty()const
{
	wxString result;
	/*
	хреновый вариант - точность хромает, проще спросить у БД
	if ("1" != GetData().mClsType)
		result = wxString::Format("%d", mObjArray->GetChildQty());
	if ("2" != GetData().mClsType || "3" != GetData().mClsType)
	{
		double sum = 0.0;
		for (unsigned int i = 0; i < mObjArray->GetChildQty(); ++i)
		{
			auto child = mObjArray->GetChild(i);
			auto obj = std::dynamic_pointer_cast<MObjItem>(child);
			double tmp;
			if (obj->GetData().mQty.ToDouble(&tmp) )
				sum += tmp ;
		}
		result = wxString::Format("%f", sum);
	}
	*/	

	result = wxString::Format("%s", mQty);
	
	return result;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsPropArray> MTypeItem::GetClsPropArray()
{
	if (!mPropArray)
	{
		mPropArray.reset(new MClsPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->Insert(item);
	}
	return mPropArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsActArray> MTypeItem::GetClsActArray()
{
	if (!mActArray)
	{
		mActArray.reset(new MClsActArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mActArray);
		this->Insert(item);
	}
	return mActArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsMoveArray> MTypeItem::GetClsMoveArray()
{
	if (!mMoveArray)
	{
		mMoveArray.reset(new MClsMoveArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mMoveArray);
		this->Insert(item);
	}
	return mMoveArray;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetSelectQuery(wxString& query)const
{
	return false;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetInsertQuery(wxString& query)const
{
	const auto& cls = GetData();

	const wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;
	wxString doid;
	if (!cls.mType.IsNull() && cls.mType.operator long()>0)
	{
		doid = cls.mDefaultObj.mId.IsNull() ? wxString("1") : cls.mDefaultObj.mId.SqlVal();
	}
	else
	{
		doid = "NULL";
	}
	
	

	query = wxString::Format(
		"INSERT INTO acls "
		" (title, note, kind, measure, pid, dobj) VALUES "
		" (%s, %s, %s, %s, %s, %s) "
		" RETURNING title, id, kind, measure, 0"
		, cls.mLabel.SqlVal()
		, cls.mComment.SqlVal()
		, cls.mType.IsNull() ? "0" : cls.mType.SqlVal()
		, cls.mMeasure.SqlVal()
		, parent
		, doid
		);
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetUpdateQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;

	query = wxString::Format(
		"UPDATE acls SET "
		" title=%s, note=%s, kind=%s, measure=%s, pid=%s, dobj=%s "
		" WHERE id = %s "
		, cls.mLabel.SqlVal()
		, cls.mComment.SqlVal()
		, cls.mType.SqlVal()
		, cls.mMeasure.SqlVal()
		, parent
		, cls.mDefaultObj.mId.SqlVal()
		, cls.mId.SqlVal() );
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetDeleteQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"DELETE FROM acls WHERE id = %s ",
		cls.mId.SqlVal() );
	return true;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MTypeArray::CreateChild()
{
	auto child = std::make_shared < MTypeItem >();
	rec::Cls cls;

	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());
	if (catalog)
	{
		cls.mParent.mId = catalog->GetRoot().mCls.mId;
		cls.mParent.mLabel = catalog->GetRoot().mCls.mLabel;
	}
	child->SetData(cls);
	return child;

}
//-------------------------------------------------------------------------
bool MTypeArray::GetSelectChildsQuery(wxString& query)const
{
	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());
	if (!catalog)
		return false;

	if (catalog->IsObjTree())
	{
		wxString catFilter = catalog->GetFilterCat();
		wxString clsFilter = catalog->GetFilterCls();
		wxString objFilter = catalog->GetFilterObj();

		query = wxString::Format(
			"SELECT acls.title, acls.id, acls.kind, acls.measure, osum.qty, acls.dobj, ob.title "
			" FROM (SELECT COALESCE(SUM(qty), 0) AS qty, cls_id FROM obj "
			"       WHERE obj.id > 0 %s %s GROUP BY obj.cls_id) osum "
			" LEFT JOIN acls ON osum.cls_id = acls.id "
			" LEFT JOIN obj_name ob ON ob.id = acls.dobj "
			" WHERE acls.id > 0 %s "
			, catFilter
			, objFilter
			, clsFilter);
		
		return true;
	}
	else
	{
		wxString catFilter = catalog->GetFilterCat();
		wxString clsFilter = catalog->GetFilterCls();
		wxString objFilter = catalog->GetFilterObj();

		query = wxString::Format(
			"SELECT acls.title, acls.id, acls.kind, acls.measure "
			" ,(SELECT COALESCE(SUM(qty), 0) FROM obj WHERE obj.id > 0 AND obj.cls_id=acls.id %s GROUP BY o.cls_id)  AS qty "
			" ,dobj AS doid, o.title AS dotitle"
			" FROM acls "
			" LEFT JOIN obj_name o ON o.id =acls.dobj "
			" WHERE acls.id > 0 %s %s"
			/*
			"SELECT t.title, osum.qty, t.id, t.kind, t.measure "
			" FROM cls_tree t "
			" LEFT JOIN(SELECT COALESCE(SUM(qty), 0) AS qty, cls_id "
			" FROM obj o "
			" GROUP BY o.cls_id) osum ON osum.cls_id = t.id "
			" WHERE t.pid = %s "
			" AND t.id > 99 "
			*/
			, objFilter
			, catFilter
			, clsFilter	);

		return true;

	}


	return false;
}

