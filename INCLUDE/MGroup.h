#ifndef __MGROUP_H
#define __MGROUP_H


#include "TVec.h"
#include "db_rec.h"

namespace wh{

class MGroupArray;

//-------------------------------------------------------------------------
class MGroup
	: public TModelData<rec::Role>
{
public:
	MGroup(const char option 
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;
	virtual const std::vector<Field>& GetFieldVector()const override;
protected:
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MGroupArray
	: public IModel
{
public:
	typedef  MGroup		T_Item;

	MGroupArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);


	virtual std::shared_ptr<IModel> CreateChild()override;
	virtual const std::vector<Field>& GetFieldVector()const override;
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child, 
		std::shared_ptr<whTable>&, const size_t)override;

};






}//namespace wh{

#endif // __****_H