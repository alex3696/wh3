#ifndef __MCLSACTPROP_H
#define __MCLSACTPROP_H


#include "TVec.h"
#include "db_rec.h"

namespace wh{

//-------------------------------------------------------------------------
class MClsActProp : public TModelData<rec::ClsActProp>
{
public:
	MClsActProp(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;
	virtual const std::vector<Field>& GetFieldVector()const override;

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;


};
//-------------------------------------------------------------------------
class MClsActPropArray : public IModel
{
public:
	typedef  MClsActProp		T_Item;

	MClsActPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual std::shared_ptr<IModel> CreateChild()override;
	virtual const std::vector<Field>& GetFieldVector()const override;
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>&, const size_t)override;

};

//-------------------------------------------------------------------------
class MClsAllActPropArray : public MClsActPropArray
{
public:
	typedef  MClsActProp		T_Item;

	MClsAllActPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};

//-------------------------------------------------------------------------
}//namespace wh{



#endif // __****_H