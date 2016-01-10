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
	, mFavProps(new MFavProp)
	, mFilter(new MFilterArray)
	, mFilterCat(new MFilter)
	
{
	this->AddChild(mPath);
	this->AddChild(mTypeArray);
	this->AddChild(mFavProps);
	this->AddChild(mFilter);

	// 0
	{
		FilterData fd;
		fd.mConn = fcAND;
		fd.mFieldName = "cls.kind";
		fd.mOp = foEq;
		fd.mVal = wxString::Format("%d", (int)ctAbstract);
		fd.mFieldType = ftLong;
		fd.mIsEnabled = false;
		auto item = mFilter->CreateItem(fd,true);
		mFilter->AddChild(item);
	}
	// 1
	mFilter->AddChild(
		mFilter->CreateItem(
			FilterData("0", "cls.id", ftLong, foEq, fcAND, false), true));

}
//-------------------------------------------------------------------------

void MObjCatalog::SetCfg(const rec::CatCfg& cfg)
{
	this->SetData(cfg);
	
	switch (cfg.mCatType)
	{
	case rec::catCls:	
	case rec::catObj:
		SetCatFilter(0, true);	
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

	if(IsPropEnabled())
		mFavProps->Load();
	
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
bool MObjCatalog::IsPropEnabled()const
{
	if (IsAbstractTree())
		return false;

	const auto& cfg = this->GetData();
	return cfg.mEnableProp && cfg.mEnableObj;
}
//-------------------------------------------------------------------------
void MObjCatalog::PropEnable(bool enable)
{
	auto cfg = this->GetData();
	cfg.mEnableProp = enable;
	this->SetData(cfg);
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
	long pid(0);
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
	unsigned long pid(0);
	
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
		break;
	case rec::catObj:	
		if (mPath->GetChildQty() > 1)
		{
			pathItem = mPath->at(1);
			if (pathItem && !pathItem->GetData().mObj.mId.IsNull())
				pid = pathItem->GetData().mObj.mId;
		}
		SetCatFilter(pid);
		break;
	case rec::catFav:
	default:break;
	}

}

//-------------------------------------------------------------------------
void MObjCatalog::SetFilterClsKind(ClsType ct, FilterOp fo, bool enable)
{
	FilterData fd(wxString::Format("%d", (int)ct)
		, "cls.kind", ftLong, fo, fcAND, enable);
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
		, "cls.id", ftLong, fo, fcAND, enable);

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
		mFilterCat->SetData(FilterData(pidStr, "cls.pid", ftLong, foEq, fcAND, enable));
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

