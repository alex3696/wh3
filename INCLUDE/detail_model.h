#ifndef __DETAIL_MODEL_H
#define __DETAIL_MODEL_H

#include "TModelArray.h"
#include "db_rec.h"


namespace wh{
namespace detail{
namespace model{
//-----------------------------------------------------------------------------
class ObjPropArray;
class ObjPropValLoader;
class ClsPropArray;
//-----------------------------------------------------------------------------
class Obj
	: public TModelData<rec::ObjInfo>
{
public:
	Obj(const char option = ModelOption::EnableNotifyFromChild);

	void SetObject(const wxString& cls_id, const wxString& obj_id, const wxString& obj_pid);

	std::shared_ptr<ClsPropArray>	GetClsPropArray()const { return mClsProp; }
	std::shared_ptr<ObjPropArray>	GetObjPropArray()const { return mObjProp; }
protected:
	std::shared_ptr<ClsPropArray>	mClsProp;
	std::shared_ptr<ObjPropArray>	mObjProp;

	virtual void LoadChilds()override;

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	virtual bool GetSelectQuery(wxString&)const override;

private:
	std::shared_ptr<ObjPropValLoader>	mObjPropValLoader;
};
//-----------------------------------------------------------------------------
class ClsProp
	: public TModelData<rec::PropVal>
{
public:
	ClsProp(const char option = ModelOption::EnableParentNotify);
	bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
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
class ObjProp
	: public TModelData<rec::PropVal>
{
public:
	ObjProp(const char option = ModelOption::EnableParentNotify);
	bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
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
class ObjPropValLoader
	: public IModel
{
public:
	ObjPropValLoader(const char option = 0);

	virtual void LoadData()override;

	bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t);
	bool GetSelectQuery(wxString&)const;

};


//-----------------------------------------------------------------------------
}//namespace model {
}//namespace detail {
}//namespace wh{
#endif // __****_H

