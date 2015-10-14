#ifndef __OBJCATALOG_H
#define __OBJCATALOG_H

#include "TModelArray.h"
#include "db_rec.h"

#include "MObjCatPath.h"
#include "MTypeNode.h"
#include "MFavProp.h"

#include "dlg_move_view_Frame.h"

namespace wh{







//-----------------------------------------------------------------------------
enum CatalogType
{
	ctByPath =	0,
	ctByType =	1
};


//-------------------------------------------------------------------------
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

	std::shared_ptr<MTypeItem> GetTypeItem(unsigned int pos)
	{
		return std::dynamic_pointer_cast<MTypeItem>(mTypeArray->GetChild(pos));
	}
	
	
	std::shared_ptr<model::MPath>	mPath;

	std::shared_ptr<MCatalogCfg>	mCfg;
	std::shared_ptr<MTypeArray>		mTypeArray;
	std::shared_ptr<MFavProp>		mFavProps;
	
	void SetCatalog(bool isObjCatalog, bool isSelectDlg, 
		bool isFindResult, const wxString& root_find);

	bool IsObjTree()const;
	bool IsSelectDlg()const;
	bool IsFindResult()const;
	bool IsShowDebug()const;

	virtual void LoadChilds()override;


	const rec::FavProps& GetFavProps()const
	{
		return mFavProps->GetData();
	}


	
protected:
	

	virtual bool GetSelectQuery(wxString&)const override;

	

};



//-------------------------------------------------------------------------
}//namespace object_catalog
//-------------------------------------------------------------------------









}//namespace wh{
#endif // __****_H
