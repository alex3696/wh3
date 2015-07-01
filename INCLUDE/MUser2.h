#ifndef __MUSER2_H
#define __MUSER2_H

#include "MUserGroup.h"



namespace wh{

//-------------------------------------------------------------------------	
class MUser2 : public TModelData<rec::User, true >
{
public:
	MUser2(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;
	virtual const std::vector<Field>& GetFieldVector()const override;

	std::shared_ptr<MUserGroupArray> GetGroupArray();
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	std::shared_ptr<MUserGroupArray>	mGroupArray;

	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

	virtual void LoadChilds()override;

};
//-------------------------------------------------------------------------
class MUserArray : public IModel
{
public:
	MUserArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	typedef  MUser2		T_Item;

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