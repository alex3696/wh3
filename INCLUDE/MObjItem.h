#ifndef __MOBJITEM_H
#define __MOBJITEM_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{
//-------------------------------------------------------------------------
namespace object_catalog {
//-------------------------------------------------------------------------
class MTypeItem;


class MObjItem
	: public TModelData<rec::Obj>
	, public std::enable_shared_from_this<MObjItem>
{
public:
	MObjItem(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;

	const MTypeItem* GetCls()const;

protected:
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

};
//-------------------------------------------------------------------------	

class MObjArray
	: public TModelArray<MObjItem>
{
public:
	MObjArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave )
	: TModelArray<MObjItem>(option)
	{}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};
//-------------------------------------------------------------------------
}//object_catalog {
//-------------------------------------------------------------------------
}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H
