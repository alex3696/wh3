#ifndef __OBJCATALOG_H
#define __OBJCATALOG_H

#include "TModelArray.h"
#include "db_rec.h"

#include "db_filter.h"

#include "MObjCatPath.h"
#include "MTypeNode.h"
#include "MFavProp.h"

#include "dlg_move_view_Frame.h"

namespace wh{
namespace object_catalog {
//-------------------------------------------------------------------------

class MCatalogCfg
	: public TModelData<rec::CatalogCfg>
{
public:
	MCatalogCfg(const char option = ModelOption::EnableParentNotify)
		:TModelData<rec::CatalogCfg>()
	{}
};

//-------------------------------------------------------------------------	
class MObjCatalog
	: public TModelData<rec::PathItem>
{
public:
	MObjCatalog(const char option
		= 
		  ModelOption::EnableNotifyFromChild
		| ModelOption::CommitLoad
		);
	
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;

	std::shared_ptr<model::MPath>	mPath;
	std::shared_ptr<MCatalogCfg>	mCfg;
	std::shared_ptr<MTypeArray>		mTypeArray;
	std::shared_ptr<MFavProp>		mFavProps;
	std::shared_ptr<MFilterArray>	mFilter;


	void SetFilterClsKind(ClsType ct, FilterOp fo = foEq, bool enable=true);
	const FilterData& GetFilterClsKind()const;
	void SetFilterClsId(long id, FilterOp fo = foEq, bool enable = true);
	const FilterData& GetFilterClsId()const;
	
	wxString GetFilterSqlString()const;
	
	void SetCatalog(bool isObjCatalog);
	
	void SetRoot(const rec::PathItem&	new_root);
	rec::PathItem GetRoot();
	
	bool IsPropEnabled()const;
	void PropEnable(bool enable = true);
	
	bool IsObjEnabled()const;
	void ObjEnable(bool enable = true);


	bool IsObjTree()const;
	bool IsClsTree()const	{ return !IsObjTree(); };
	bool IsShowDebug()const;
	
	bool IsAbstractTree()const;

	virtual void LoadChilds()override;


	const rec::FavProps& GetFavProps()const
	{
		return mFavProps->GetData();
	}

	
	void DoUp();
	
	
protected:

	virtual bool GetSelectQuery(wxString&)const override;

	// filter

	

};



//-------------------------------------------------------------------------
}//namespace object_catalog
//-------------------------------------------------------------------------









}//namespace wh{
#endif // __****_H
