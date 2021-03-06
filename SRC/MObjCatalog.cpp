#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MObjCatalog::MObjCatalog(const char option)
	:TModelData<rec::CatCfg>(option)
	, mPath(new model::MPath)
	, mTypeArray(new MTypeArray)
	, mFilter(new MFilterArray)
	, mFilterCat(new MFilter)
	, mAutoLoadObj(new TModelData<bool>)
	
{

	mAutoLoadObj->SetData(false, true);
	Insert(mAutoLoadObj);


	this->Insert(mPath);
	this->Insert(mTypeArray);
	this->Insert(mFilter);

	// 0
	{
		FilterData fd;
		fd.mConn = fcAND;
		fd.mFieldName = "acls.kind";
		fd.mOp = foEq;
		fd.mVal = wxString::Format("%d", (int)ClsKind::Abstract);
		fd.mFieldType = ftLong;
		fd.mIsEnabled = false;
		auto item = mFilter->CreateItem(fd,true);
		mFilter->Insert(item);
	}
	// 1
	mFilter->Insert(
		mFilter->CreateItem(
			FilterData("0", "acls.id", ftLong, foEq, fcAND, false), true));

	// 2
	{
		FilterData fd;
		fd.mConn = fcAND;
		fd.mFieldName = "acls.title";
		fd.mOp = foLike;
		fd.mVal = "";
		fd.mFieldType = ftText;
		fd.mIsEnabled = false;
		auto item = mFilter->CreateItem(fd, true);
		mFilter->Insert(item);
	}

}
//-------------------------------------------------------------------------

void MObjCatalog::SetCfg(const rec::CatCfg& cfg)
{
	this->SetData(cfg);

	unsigned long parent_id = cfg.mHideSystemRoot ? 1 : 0;
	
	switch (cfg.mCatType)
	{
	case rec::catCls:	
	case rec::catObj:
		SetCatFilter(parent_id, true);
		break;
	default:
		SetCatFilter(0, false);
		break;
	}
}

//-------------------------------------------------------------------------
void MObjCatalog::LoadChilds()
{
	mPath->Load();
	mTypeArray->Load();
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsObjTree()const
{
	const auto& cfg = this->GetData();
	return rec::catObj == cfg.mCatType;
}

//-------------------------------------------------------------------------
bool MObjCatalog::IsShowDebug()const
{
	const auto& cfg = this->GetData();
	return cfg.mShowDebugColumns;
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsObjEnabled()const
{
	if (IsAbstractTree())
		return false;

	const auto& cfg = this->GetData();
	return cfg.mEnableObj;
}
//-------------------------------------------------------------------------
void MObjCatalog::ObjEnable(bool enable)
{
	auto cfg = this->GetData();
	cfg.mEnableObj = enable;
	this->SetData(cfg);
}
//-------------------------------------------------------------------------
rec::PathItem MObjCatalog::GetRoot()
{
	const auto& cfg = this->GetData();
	rec::PathItem root;
	std::shared_ptr<MFilter> mfilter;

	switch (cfg.mCatType)
	{
	case rec::catCls:
		root.mCls.mId = mFilterCat->GetData().mVal;
	case rec::catObj:
		root.mObj.mId = mFilterCat->GetData().mVal;
		break;
	default:break;
	}
	return root;
}
//-------------------------------------------------------------------------
void MObjCatalog::DoUp()
{
	const auto& cfg = this->GetData();

	std::shared_ptr<model::MPathItem> pathItem;
	unsigned long pid = cfg.mHideSystemRoot ? 1 : 0;
	
	switch (cfg.mCatType)
	{
	case rec::catCls:	
		if (mPath->GetChildQty() > 1)
		{
			pathItem = mPath->at(1);
			if (pathItem && !pathItem->GetData().mCls.mId.IsNull())
				pid = pathItem->GetData().mCls.mId;
		}
		SetCatFilter(pid);
		Load();
		break;
	case rec::catObj:	
		if (mPath->GetChildQty() > 1)
		{
			pathItem = mPath->at(1);
			if (pathItem && !pathItem->GetData().mObj.mId.IsNull())
				pid = pathItem->GetData().mObj.mId;
		}
		SetCatFilter(pid);
		Load();
		break;
	default:break;
	}

}

//-------------------------------------------------------------------------
void MObjCatalog::SetFilterClsKind(ClsKind ct, FilterOp fo, bool enable)
{
	FilterData fd(wxString::Format("%d", (int)ct)
		, "acls.kind", ftLong, fo, fcAND, enable);
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
		, "acls.id", ftLong, fo, fcAND, enable);

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
			return ClsKind::Abstract == (ClsKind)val;
	}
	return false;
}
//-------------------------------------------------------------------------
wxString MObjCatalog::GetFilterCls()const
{
	return mFilter->GetSqlString();
}
//-------------------------------------------------------------------------
wxString MObjCatalog::GetFilterObj()const
{
	return wxEmptyString;
}
//-------------------------------------------------------------------------
void MObjCatalog::SetCatFilter(long pid, bool enable)
{
	const auto& cfg = this->GetData();
	const wxString pidStr = wxString::Format("%d", pid);
	switch (cfg.mCatType)
	{
	case rec::catCls:
		mFilterCat->SetData(FilterData(pidStr, "acls.pid", ftLong, foEq, fcAND, enable));
		break;
	case rec::catObj:
		mFilterCat->SetData(FilterData(pidStr, "obj.pid", ftLong, foEq, fcAND, enable));
		break;
	default:
		mFilterCat->SetData(FilterData(wxEmptyString, wxEmptyString, ftLong, foEq, fcAND, false));
		break;
	}
}
//-------------------------------------------------------------------------
wxString MObjCatalog::GetFilterCat()const
{
	return mFilterCat->GetData().GetSqlString();
}
//-------------------------------------------------------------------------
void MObjCatalog::Find(const wxString& cls_filter)
{
	auto cfilter = std::dynamic_pointer_cast<MFilter>(mFilter->GetChild(2));
	auto cfilter_data = cfilter->GetData();
	cfilter_data.mVal = cls_filter;
	cfilter_data.mIsEnabled = true;
	cfilter->SetData(cfilter_data, true);

	auto cat_old = mFilterCat->GetData();
	wh::FilterData cat_filter = cat_old;
	cat_filter.mIsEnabled = false;
	mFilterCat->SetData(cat_filter);
	
	Load();

	cfilter_data.mIsEnabled = false;
	cfilter->SetData(cfilter_data, true);

	mFilterCat->SetData(cat_old, true);
}
