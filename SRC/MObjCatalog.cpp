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
			"SELECT t_objnum.id, t_objnum.label, t_objnum.pid "
			"      ,t_cls.id,    t_cls.label, t_cls.pid "
			"FROM t_objnum "
			"LEFT JOIN t_cls ON t_cls.id = t_objnum.id "
			"WHERE t_objnum.id = %s "
			, data.mObj.mID
			);
		
	}
	else
	{
		
		const auto& data = this->GetData();
		if (data.mCls.mID.empty())
			return false;
		query = wxString::Format(
			"SELECT NULL,NULL, NULL, id, label  , pid "
			" FROM t_cls "
			" WHERE id=%s "
			, data.mCls.mID
			);

	}
	return true;
}
//-------------------------------------------------------------------------
bool MObjCatalog::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mObj.mID);
	table->GetAsString(1, row, data.mObj.mLabel);
	table->GetAsString(2, row, data.mObj.mPID);
	table->GetAsString(3, row, data.mCls.mID);
	table->GetAsString(4, row, data.mCls.mLabel);
	table->GetAsString(5, row, data.mCls.mParent.mId);
	
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
