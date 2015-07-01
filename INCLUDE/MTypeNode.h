#ifndef __MTYPENODE_H
#define __MTYPENODE_H


#include "MObjItem.h"

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
protected:
	virtual void LoadChilds()override;
};
//-------------------------------------------------------------------------	

class MTypeArray
	: public TModelArray<MTypeItem>
{
public:
	MTypeArray(const char option
		= ModelOption::EnableParentNotify
		//| ModelOption::EnableNotifyFromChild
		| ModelOption::CascadeLoad )
	: TModelArray<MTypeItem>(option)
	{
	}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};
//-------------------------------------------------------------------------
}//object_catalog {
//-------------------------------------------------------------------------
}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H
