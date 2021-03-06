#ifndef __MCLSPROP_H
#define __MCLSPROP_H

#include "TModelArray.h"
#include "db_rec.h"

namespace wh{

//-------------------------------------------------------------------------
class MClsProp : public TModelData<rec::ClsProp>
{
public:
	MClsProp(const char option
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
class MClsPropArray : public TModelArray<MClsProp>
{
public:
	typedef  MClsProp		T_Item;

	MClsPropArray(const char option
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



}//namespace wh{
#endif // __****_H