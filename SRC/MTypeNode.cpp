#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;

const std::vector<Field> gTypeItemFieldVec =
{
	{ "���", FieldType::ftName, true },
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MTypeItem::MTypeItem(const char option)
	:TModelData<rec::Cls>(option)
	, mObjArray(new MObjArray)
	, mFavProp(new dlg::favprop::model::FavPropArray)
{
	this->AddChild(std::dynamic_pointer_cast<IModel>(mObjArray));
	this->AddChild(std::dynamic_pointer_cast<IModel>(mFavProp));
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
			data.mParent = catalog->GetData().mCls.mParent;
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

	if (!catalog->IsObjTree() && catalog->IsSelectDlg())
		return;

	if( GetData().IsAbstract() )
		return;

	mObjArray->Load();
	
}
//-------------------------------------------------------------------------
wxString MTypeItem::GetQty()const
{
	wxString result;
	/*
	�������� ������� - �������� �������, ����� �������� � ��
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
		this->AddChild(item);
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
		this->AddChild(item);
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
		this->AddChild(item);
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

	query = wxString::Format(
		"INSERT INTO cls "
		" (title, note, kind, measure, pid, default_objid) VALUES "
		" (%s, %s, %s, %s, %s, %s) "
		" RETURNING title, id, kind, measure, 0"
		, cls.mLabel.SqlVal()
		, cls.mComment.SqlVal()
		, cls.mType.IsNull() ? "0" : cls.mType.SqlVal()
		, cls.mMeasure.SqlVal()
		, parent
		, cls.mDefaultObj.mId.SqlVal()
		);
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetUpdateQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;

	query = wxString::Format(
		"UPDATE cls SET "
		" title=%s, note=%s, kind=%s, measure=%s, pid=%s, default_objid=%s "
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
		"DELETE FROM cls WHERE id = %s ",
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
		cls.mParent.mId = catalog->GetData().mCls.mId;
		cls.mParent.mLabel = catalog->GetData().mCls.mLabel;
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

	const auto& root = catalog->GetData();

	if (catalog->IsObjTree())
	{
		query = wxString::Format(
			"SELECT r.title, r.id, r.kind, cr.measure, osum.qty, cr.default_objid, ob.title "
			"FROM(SELECT COALESCE(SUM(qty), 0) AS qty, cls_id FROM obj o "
			"WHERE o.pid = %s  AND o.id>99 GROUP BY o.cls_id) osum "
			"LEFT JOIN cls_real cr ON osum.cls_id = cr.id "
			"LEFT JOIN cls_name r USING(id)" 
			" LEFT JOIN obj_name ob ON ob.id = cr.default_objid "
			, root.mObj.mId.SqlVal() );
		return true;
	}
	else
	{
		query = wxString::Format(
			"SELECT t.title, t.id, t.kind, t.measure "
			" ,(SELECT COALESCE(SUM(qty), 0) "
			"     FROM obj o WHERE t.id = o.cls_id GROUP BY o.cls_id)  AS qty "
			" ,default_objid, o.title "
			" FROM cls t "
			" LEFT JOIN obj_name o ON o.id = t.default_objid "
			" WHERE t.pid = %s"
			" AND t.id > 99 "
			/*
			"SELECT t.title, osum.qty, t.id, t.kind, t.measure "
			" FROM cls_tree t "
			" LEFT JOIN(SELECT COALESCE(SUM(qty), 0) AS qty, cls_id "
			" FROM obj o "
			" GROUP BY o.cls_id) osum ON osum.cls_id = t.id "
			" WHERE t.pid = %s "
			" AND t.id > 99 "
			*/
			, root.mCls.mId.SqlVal() );
		if (catalog->IsSelectDlg())
			query += " AND t.kind=0 ";
		return true;

	}


	return false;
}


/*
SELECT t.title, osum.qty, t.id, t.kind, t.measure , t.pid
FROM cls_tree t
RIGHT   JOIN LATERAL
(
SELECT COALESCE(count(*), 0) AS qty, n.cls_id
FROM obj_num n
WHERE cls_id IN (SELECT id FROM cls_num WHERE pid = 100 AND id > 99)
GROUP BY n.cls_id
UNION ALL
SELECT COALESCE(SUM(qty), 0) AS qty, cls_id
FROM ONLY obj_names_qtyi n
LEFT JOIN obj_details_qtyi d ON n.id=objqty_id
WHERE cls_id IN (SELECT id FROM cls_qtyi WHERE pid = 100 AND id > 99)
GROUP BY n.cls_id
UNION ALL
SELECT COALESCE(SUM(qty), 0) AS qty, cls_id
FROM ONLY obj_names_qtyf n
LEFT JOIN obj_details_qtyf d ON n.id=objqty_id
WHERE cls_id IN (SELECT id FROM cls_qtyf WHERE pid = 100 AND id > 99)
GROUP BY n.cls_id

) osum ON osum.cls_id = t.id
*/