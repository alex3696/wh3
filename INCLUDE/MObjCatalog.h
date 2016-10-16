#ifndef __OBJCATALOG_H
#define __OBJCATALOG_H

#include "TModelArray.h"
#include "db_rec.h"

#include "FilterModel.h"

#include "MObjCatPath.h"
#include "MTypeNode.h"
#include "MFavProp.h"

#include "dlg_move_view_Frame.h"

namespace wh{
namespace object_catalog {

//-------------------------------------------------------------------------	
class MObjCatalog
	: public TModelData<rec::CatCfg>
{
public:
	MObjCatalog(const char option
		= 
		  ModelOption::EnableNotifyFromChild
		| ModelOption::CommitLoad
		);
	
	void SetCfg(const rec::CatCfg& cfg);

	std::shared_ptr<TModelData<bool>> mAutoLoadObj;
	std::shared_ptr<MTypeArray>		mTypeArray;
	std::shared_ptr<MFavProp>		mFavProps;
	std::shared_ptr<model::MPath>	mPath;
	
	void SetCatFilter(long pid, bool enable = true);	
	wxString GetFilterCat()const;


	void SetFilterClsKind(ClsType ct, FilterOp fo = foEq, bool enable=true);
	const FilterData& GetFilterClsKind()const;
	void SetFilterClsId(long id, FilterOp fo = foEq, bool enable = true);
	const FilterData& GetFilterClsId()const;


	wxString GetFilterCls()const;
	wxString GetFilterObj()const;
	
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
	std::shared_ptr<MFilterArray>	mFilter;
	std::shared_ptr<MFilter>		mFilterCat;

	
	

};



//-------------------------------------------------------------------------
}//namespace object_catalog
//-------------------------------------------------------------------------









}//namespace wh{
#endif // __****_H
