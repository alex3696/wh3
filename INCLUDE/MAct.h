#ifndef __MACT_H
#define __MACT_H

#include "MActProp.h"

namespace wh{

//-------------------------------------------------------------------------	
class MAct : public TModelData<rec::Act, true >
{
public:
	MAct(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;
	virtual const std::vector<Field>& GetFieldVector()const override;

	std::shared_ptr<MActPropArray> GetPropArray();
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	std::shared_ptr<MActPropArray>	mPropArray;

	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

	virtual void LoadChilds()override;

};
//-------------------------------------------------------------------------
class MActArray : public TModelArray<MAct>
{
public:
	typedef  MAct		T_Item;

	MActArray(const char option
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