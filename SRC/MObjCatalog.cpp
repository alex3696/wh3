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
	if (mCfg->GetData().mObjCatalog)
	{
		const auto& data = this->GetData();
		if (data.mObj.mID.empty())
			return false;
		query = wxString::Format(
			" SELECT o.id, o.title, o.pid, t.id, t.title, t.pid "
			" FROM obj_tree o "
			" LEFT JOIN cls_tree t ON t.id = o.cls_id "
			" WHERE o.id = %s "
			, data.mObj.mID
			);
		
	}
	else
	{
		
		const auto& data = this->GetData();
		if (data.mCls.mID.IsNull())
			return false;
		query = wxString::Format(
			"SELECT NULL,NULL, NULL, id, title  , pid "
			" FROM cls_tree "
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
	data.mObj.mID = table->GetAsString(0, row);
	data.mObj.mLabel = table->GetAsString(1, row);
	data.mObj.mPID = table->GetAsString(2, row);
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
void MObjCatalog::SetObjCatalog(unsigned int objPid)
{
	rec::CatalogCfg cfg(true);
	mCfg->SetData(cfg);
	wh::rec::PathItem root;
	root.mObj.mID = wxString::Format("%d", objPid);
	SetData(root);
	Load();
}
//-------------------------------------------------------------------------
void MObjCatalog::SetClsCatalog(unsigned int clsPid)
{
	rec::CatalogCfg cfg(false);
	mCfg->SetData(cfg);

	wh::rec::PathItem root;
	root.mCls.mID = wxString::Format("%d", clsPid);
	SetData(root);
	Load();
}
