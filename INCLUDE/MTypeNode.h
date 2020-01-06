#ifndef __MTYPENODE_H
#define __MTYPENODE_H


#include "MObjItem.h"
//#include "MObjCatalog.h"
#include "MClsProp.h"
#include "MClsAct.h"
#include "MClsMove.h"

//-------------------------------------------------------------------------
namespace wh{
//-------------------------------------------------------------------------
namespace object_catalog {
//-------------------------------------------------------------------------
class MTypeItem
	: public TModelData<rec::Cls>
	, public std::enable_shared_from_this<MTypeItem>
{
public:
MTypeItem(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	std::shared_ptr<MObjArray>	mObjArray;
	wxString					mQty;

	wxString GetQty()const;
	inline bool IsAbstract()const { return GetData().IsAbstract(); }
	
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;

	std::shared_ptr<MClsPropArray>	GetClsPropArray();
	std::shared_ptr<MClsActArray>	GetClsActArray();
	std::shared_ptr<MClsMoveArray>	GetClsMoveArray();
protected:
	virtual void LoadChilds()override;

	std::shared_ptr<MClsPropArray>	mPropArray;
	std::shared_ptr<MClsActArray>	mActArray;
	std::shared_ptr<MClsMoveArray>	mMoveArray;

	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

};
//-------------------------------------------------------------------------	

class MTypeArray
	: public TModelArray<MTypeItem>
{
public:
	MTypeArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CascadeLoad )
	: TModelArray<MTypeItem>(option)
	{
	}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual std::shared_ptr<IModel> CreateChild()override;
};
//-------------------------------------------------------------------------
}//object_catalog {
//-------------------------------------------------------------------------
}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H
