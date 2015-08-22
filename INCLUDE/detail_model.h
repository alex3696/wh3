#ifndef __DETAIL_MODEL_H
#define __DETAIL_MODEL_H

#include "TModelArray.h"
#include "db_rec.h"


namespace wh{
namespace detail{
namespace model{
//-----------------------------------------------------------------------------
class ObjPropArray;
class ClsPropArray;
//-----------------------------------------------------------------------------
class Obj
	: public TModelData<rec::ObjInfo>
{
public:
	Obj(const char option = ModelOption::EnableNotifyFromChild);

	void SetObject(const wxString& cls_id, const wxString& obj_id, const wxString& obj_pid);
protected:
	std::shared_ptr<ClsPropArray>	mClsProp;
	std::shared_ptr<ObjPropArray>	mObjProp;

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	virtual bool GetSelectQuery(wxString&)const override;
};
//-----------------------------------------------------------------------------
class ClsProp
	: public TModelData<rec::PropVal>
{
public:
	ClsProp(const char option = ModelOption::EnableParentNotify);
};
//-----------------------------------------------------------------------------
class ObjProp
	: public TModelData<rec::PropVal>
{
public:
	ObjProp(const char option = ModelOption::EnableParentNotify);
};
//-----------------------------------------------------------------------------
class ClsPropArray
	: public TModelArray<ClsProp>
{
public:
	ClsPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
};
//-----------------------------------------------------------------------------
class ObjPropArray
	: public TModelArray<ObjProp>
{
public:
	ObjPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
};



//-----------------------------------------------------------------------------
}//namespace model {
}//namespace detail {
}//namespace wh{
#endif // __****_H

