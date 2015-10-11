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
	this->AddChild(std::dynamic_pointer_cast<IModel>(mObjArray));
	this->AddChild(std::dynamic_pointer_cast<IModel>(mFavProp));
}




//-------------------------------------------------------------------------
bool MTypeItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mLabel = table->GetAsString(0, row);
	data.mID = table->GetAsLong(1, row);
	data.mType = table->GetAsString(2, row);
	data.mMeasure = table->GetAsString(3, row);
	table->GetAsString(4, row, mQty);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
void MTypeItem::LoadChilds()
{
	//auto type_array = dynamic_cast<MTypeArray*>(this->GetParent());
	//auto catalog = dynamic_cast<MObjCatalog*>(type_array->GetParent());
	if ( "0" != GetData().mType)
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
//-------------------------------------------------------------------------
bool MTypeItem::GetInsertQuery(wxString& query)const
{
	const auto& cls = GetData();

	const wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;

	query = wxString::Format(
		"INSERT INTO cls_tree "
		" (title, note, kind, measure, pid) VALUES "
		" (%s, %s, %s, %s, %s) "
		" RETURNING title, id, kind, measure, 0"
		, cls.mLabel.SqlVal()
		, cls.mComment.SqlVal()
		, cls.mType
		, cls.mMeasure.SqlVal()
		, parent
		);
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetUpdateQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;

	query = wxString::Format(
		"UPDATE cls_tree SET "
		" title=%s, note=%s, kind=%s, measure=%s, pid=%s "
		" WHERE id = %s "
		, cls.mLabel.SqlVal()
		, cls.mComment.SqlVal()
		, cls.mType
		, cls.mMeasure.SqlVal()
		, parent
		, cls.mID.SqlVal() );
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetDeleteQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"DELETE FROM cls WHERE id = %s ",
		cls.mID.SqlVal() );
	return true;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MTypeArray::GetSelectChildsQuery(wxString& query)const
{
	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());
	if (!catalog)
		return false;

	const auto& root = catalog->GetData();

	if (catalog->mCfg->GetData().mObjCatalog)
	{
		query = wxString::Format(
			"SELECT r.title, r.id, r.kind, r.measure, osum.qty "
			" FROM cls_real r "
			" RIGHT JOIN(SELECT COALESCE(SUM(qty), 0) AS qty, cls_id "
			" FROM obj_tree o "
			" WHERE o.pid = %s "
			" AND o.id>99"
			" GROUP BY o.cls_id) osum ON osum.cls_id = r.id "
			, root.mObj.mId.SqlVal() );
		return true;
	}
	else
	{
		query = wxString::Format(
			" SELECT t.title, t.id, t.kind, t.measure "
			" ,(SELECT COALESCE(SUM(qty), 0) "
			"     FROM obj_tree o WHERE t.id = o.cls_id GROUP BY o.cls_id)  AS qty "
			" FROM cls_tree t "
			" WHERE t.pid = %s"
			" AND id > 99 "
			/*
			"SELECT t.title, osum.qty, t.id, t.kind, t.measure "
			" FROM cls_tree t "
			" LEFT JOIN(SELECT COALESCE(SUM(qty), 0) AS qty, cls_id "
			" FROM obj_tree o "
			" GROUP BY o.cls_id) osum ON osum.cls_id = t.id "
			" WHERE t.pid = %s "
			" AND t.id > 99 "
			*/
			, root.mCls.mID.SqlVal() );
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