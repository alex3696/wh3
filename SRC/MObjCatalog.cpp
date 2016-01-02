#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MObjCatalog::MObjCatalog(const char option)
	:TModelData<rec::PathItem>(option)
	, mPath(new model::MPath)
	, mTypeArray(new MTypeArray)
	, mFavProps(new MFavProp)
	, mCfg(new MCatalogCfg)
	, mFilter(new MFilterArray)
	
{
	this->AddChild(mPath);
	this->AddChild(mTypeArray);
	this->AddChild(mFavProps);
	this->AddChild(mCfg);
	this->AddChild(mFilter);
	
	rec::CatalogCfg cfg(false,true);
	mCfg->SetData(cfg);

	SetFilterClsKind(ctAbstract, foEq, false);
	SetFilterClsId(1, foEq, false);
}

//-------------------------------------------------------------------------
bool MObjCatalog::GetSelectQuery(wxString& query)const
{
	if (IsObjTree())
	{
		const auto& data = this->GetData();
		if (data.mObj.mId.IsNull())
			return false;
		query = wxString::Format(
			" SELECT o.id, o.title, o.pid, t.id, t.title, t.pid "
			" FROM obj o "
			" LEFT JOIN cls t ON t.id = o.cls_id "
			" WHERE o.id = %s "
			, data.mObj.mId.SqlVal()
			);
		
	}
	else
	{
		
		const auto& data = this->GetData();
		if (data.mCls.mId.IsNull())
			return false;
		query = wxString::Format(
			"SELECT NULL,NULL, NULL, id, title  , pid "
			" FROM cls "
			" WHERE id=%s "
			, data.mCls.mId.SqlVal()
			);

	}
	return true;
}
//-------------------------------------------------------------------------
bool MObjCatalog::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mObj.mId = table->GetAsString(0, row);
	data.mObj.mLabel = table->GetAsString(1, row);
	data.mObj.mParent.mId = table->GetAsString(2, row);
	data.mCls.mId = table->GetAsString(3, row);
	data.mCls.mLabel = table->GetAsString(4, row);
	data.mCls.mParent.mId = table->GetAsLong(5, row );
	
	SetData(data, true);
	return true;
};
//-------------------------------------------------------------------------
void MObjCatalog::LoadChilds()
{
	if(IsPropEnabled())
		mFavProps->Load();
	mTypeArray->Load();
	mPath->Load();
}
//-------------------------------------------------------------------------

void MObjCatalog::SetCatalog(bool isObjCatalog)
{
	auto cfg = mCfg->GetData();
	cfg.mIsOjbTree = isObjCatalog;
	mCfg->SetData(cfg,true);
	
	rec::PathItem root;
	if (isObjCatalog)
		root.mObj.mId = 1;
	else
		root.mCls.mId = 1;

	SetRoot(root);
}
//-------------------------------------------------------------------------
void MObjCatalog::SetRoot(const rec::PathItem&	new_root)
{
	SetData(new_root);
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsObjTree()const
{
	const auto& cfg = mCfg->GetData();
	return cfg.mIsOjbTree;
}

//-------------------------------------------------------------------------
bool MObjCatalog::IsShowDebug()const
{
	const auto& cfg = mCfg->GetData();
	return cfg.mShowDebugColumns;
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsPropEnabled()const
{
	if (IsAbstractTree())
		return false;

	const auto& cfg = mCfg->GetData();
	return cfg.mEnableProp && cfg.mEnableObj;
}
//-------------------------------------------------------------------------
void MObjCatalog::PropEnable(bool enable)
{
	auto cfg = mCfg->GetData();
	cfg.mEnableProp = enable;
	mCfg->SetData(cfg);
	//Load();
}

//-------------------------------------------------------------------------
bool MObjCatalog::IsObjEnabled()const
{
	if (IsAbstractTree())
		return false;

	const auto& cfg = mCfg->GetData();
	return cfg.mEnableObj;
}
//-------------------------------------------------------------------------
void MObjCatalog::ObjEnable(bool enable)
{
	auto cfg = mCfg->GetData();
	cfg.mEnableObj = enable;
	mCfg->SetData(cfg);
	//Load();
}
//-------------------------------------------------------------------------
rec::PathItem MObjCatalog::GetRoot()
{
	const auto& root = this->GetData();
	return root;
}
//-------------------------------------------------------------------------
void MObjCatalog::DoUp()
{
	const auto old_root = GetRoot();

	unsigned long pid(0);
	rec::PathItem new_root = old_root;

	if (IsObjTree())
	{
		pid = old_root.mObj.mParent.mId;
		if (0 < pid)
			new_root.mObj.mId = old_root.mObj.mParent.mId;
	}
	else
	{
		pid = old_root.mCls.mParent.mId;
		if (0 < pid)
			new_root.mCls.mId = old_root.mCls.mParent.mId;
	}
	SetData(new_root);
}

//-------------------------------------------------------------------------
void MObjCatalog::SetFilterClsKind(ClsType ct, FilterOp fo, bool enable)
{
	FilterData fd(wxString::Format("%d", (int)ct)
		, "cls.kind", dbLong, fo, fcAND, enable);

	if (!mFilter->GetChildQty())
	{
		auto item = mFilter->CreateItem();
		mFilter->AddChild(item);
	}
		
	mFilter->at(0)->SetData(fd, true);
		
}
//-------------------------------------------------------------------------
const FilterData& MObjCatalog::GetFilterClsKind()const
{
	return mFilter->at(0)->GetData();
};

//-------------------------------------------------------------------------
void MObjCatalog::SetFilterClsId(long id, FilterOp fo, bool enable)
{
	FilterData fd(wxString::Format("%d", id)
		, "cls.id", dbLong, fo, fcAND, enable);

	if (mFilter->GetChildQty() < 2)
	{
		auto item = mFilter->CreateItem();
		mFilter->AddChild(item);
	}

	mFilter->at(1)->SetData(fd, true);

}
//-------------------------------------------------------------------------
const FilterData& MObjCatalog::GetFilterClsId()const
{
	return mFilter->at(1)->GetData();
};

//-------------------------------------------------------------------------
bool MObjCatalog::IsAbstractTree()const
{
	const auto& filter = mFilter->at(0)->GetData();
	if (filter.mIsEnabled)
	{
		unsigned long val;
		if (
			filter.mVal.ToCULong(&val))
			return ctAbstract == (ClsType)val;
	}
	return false;
}
//-------------------------------------------------------------------------
wxString MObjCatalog::GetFilterSqlString()const
{
	return mFilter->GetSqlString();
}