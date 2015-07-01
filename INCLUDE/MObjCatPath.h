#ifndef __OBJCATPATH_H
#define __OBJCATPATH_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{
//-------------------------------------------------------------------------
namespace object_catalog {
//-------------------------------------------------------------------------
class MPathItem
	: public TModelData<rec::PathItem>
{
public:
	MPathItem(const char option
		= ModelOption::EnableParentNotify);

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
};
//-------------------------------------------------------------------------	

class MPath
	: public TModelArray<MPathItem>
{
public:

	MPath(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
	:TModelArray<MPathItem>(option)
	{}

	wxString GetPathStr()const;
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};
//-------------------------------------------------------------------------
}//object_catalog {
//-------------------------------------------------------------------------
}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H
