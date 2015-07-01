#ifndef __MCLSBASE_H
#define __MCLSBASE_H

#include "TModel.h"
#include "db_rec.h"

namespace wh{

class MClsNodeArray;

class MClsPropArray;
class MClsActArray;
class MClsMoveArray;
class MClsObjNumArray;
class MClsObjQtyArray;



//-------------------------------------------------------------------------
class MClsBase
	: public TModelData<rec::Cls>
{
public:

	MClsBase(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	virtual const std::vector<Field>& GetFieldVector()const override;

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;

	//std::shared_ptr<MClsNodeArray> GetClsArray();

	
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	
protected:
	//std::shared_ptr<MClsNodeArray>	mClsArray;

	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;
	//virtual void LoadChilds()override;

};

//-------------------------------------------------------------------------


}//namespace wh{
#endif // __****_H