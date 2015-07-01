#ifndef __MPROP_H
#define __MPROP_H

#include "TVec.h"
#include "db_rec.h"

#include "TModelArray.h"

namespace wh{

class MPropVec;
//-------------------------------------------------------------------------	


//-------------------------------------------------------------------------
class MPropChild
	: public TModelData<rec::Prop>
{
public:
	MPropChild(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	

	
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
//-------------------------------------------------------------------------
class MPropArray
	: public TModelArray<MPropChild>
{
public:
	MPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
	:TModelArray<MPropChild>(option)
	{}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};





}//namespace wh{
#endif // __****_H