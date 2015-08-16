#ifndef __MUSERGROUP_H
#define __MUSERGROUP_H

#include "TModelArray.h"
#include "db_rec.h"

namespace wh{

//-------------------------------------------------------------------------
class MUserGroup : public TModelData<rec::UserRole >
{
public:
	MUserGroup(const char option
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
class MUserGroupArray : public TModelArray<MUserGroup>
{
public:
	typedef  MUserGroup		T_Item;
	
	MUserGroupArray(const char option
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