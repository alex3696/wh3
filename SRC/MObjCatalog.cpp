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
	//, mUniquePropStore(new MTypeNodePropArray)
{
	this->AddChild(std::dynamic_pointer_cast<IModel>(mPath));
	this->AddChild(std::dynamic_pointer_cast<IModel>(mTypeArray));
	//this->AddChild(std::dynamic_pointer_cast<IModel>(mUniquePropStore));
	this->AddChild(std::dynamic_pointer_cast<IModel>(mFavProps));
	this->AddChild(std::dynamic_pointer_cast<IModel>(mCfg));

	rec::CatalogCfg cfg(false);
	mCfg->SetData(cfg);
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
		if (data.mCls.mID.IsNull())
			return false;
		query = wxString::Format(
			"SELECT NULL,NULL, NULL, id, title  , pid "
			" FROM cls "
			" WHERE id=%s "
			, data.mCls.mID.SqlVal()
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
	data.mCls.mID = table->GetAsString(3, row);
	data.mCls.mLabel = table->GetAsString(4, row);
	data.mCls.mParent.mId = table->GetAsLong(5, row );
	
	SetData(data, true);
	return true;
};
//-------------------------------------------------------------------------
void MObjCatalog::LoadChilds()
{
	mFavProps->Load();
	mTypeArray->Load();
	mPath->Load();
}
//-------------------------------------------------------------------------

void MObjCatalog::SetCatalog(bool isObjCatalog, bool isSelectDlg,
	bool isFindResult, const wxString& root_find)
{
	rec::CatalogCfg cfg(isObjCatalog, isSelectDlg, isFindResult);
	mCfg->SetData(cfg,true);

	wh::rec::PathItem root;
	isObjCatalog ? root.mObj.mId = root_find : root.mCls.mID = root_find;

	SetData(root);
	Load();
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsObjTree()const
{
	const auto& cfg = mCfg->GetData();
	return cfg.mIsOjbTree;
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsSelectDlg()const
{
	const auto& cfg = mCfg->GetData();
	return cfg.mIsSelectDlg;
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsFindResult()const
{
	const auto& cfg = mCfg->GetData();
	return cfg.mIsFindResult;
}
//-------------------------------------------------------------------------
bool MObjCatalog::IsShowDebug()const
{
	const auto& cfg = mCfg->GetData();
	return cfg.mShowDebugColumns;
}
