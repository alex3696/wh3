#ifndef __OBJCATPATH_H
#define __OBJCATPATH_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{
//-------------------------------------------------------------------------
namespace object_catalog {
//-------------------------------------------------------------------------
namespace model {
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
	wxString GetLastItemStr()const;
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MTypeItem;
//-------------------------------------------------------------------------
class ClsPathItem
	: public TModelData<rec::Cls>
{
public:
	ClsPathItem(const char option
		= ModelOption::EnableParentNotify);
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;

};

//-------------------------------------------------------------------------
class ClsPath
	: public TModelArray<MPathItem>
{
public:

	ClsPath(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
		:TModelArray<MPathItem>(option)
	{}

	wxString AsString()const;
	void SetCls(std::shared_ptr<MTypeItem> cls);
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

	std::shared_ptr<MTypeItem> mCls;
};


//-------------------------------------------------------------------------
class ObjPathItem
	: public TModelData<rec::PathItem>
{
public:
	ObjPathItem(const char option = ModelOption::EnableParentNotify);
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
};
//-------------------------------------------------------------------------
class ObjPath
	: public TModelArray<ObjPathItem>
{
public:
	ObjPath(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
		:TModelArray<ObjPathItem>(option)
	{}

	wxString AsString()const;
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};
//-------------------------------------------------------------------------





}//namespace model {
//-------------------------------------------------------------------------
}//object_catalog {
//-------------------------------------------------------------------------
}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H
