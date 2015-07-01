#ifndef __MCLSFAVORITEACTPROP_H
#define __MCLSFAVORITEACTPROP_H

#include "TModel.h"
#include "db_rec.h"
#include "MClsBase.h"


namespace wh{

class MClsFavoriteArray;
class MClsActPropArray;

//-------------------------------------------------------------------------
class MClsFavorite
	: public MClsBase
{
public:

	MClsFavorite(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	std::shared_ptr<MClsFavoriteArray>	GetClsArray();
	std::shared_ptr<MClsActPropArray>	GetPropArray();

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;
protected:
	std::shared_ptr<MClsFavoriteArray>	mClsArray;
	std::shared_ptr<MClsActPropArray>	mPropArray;

	virtual void LoadChilds()override;

};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MClsFavoriteArray
	: public IModel
{
public:
	typedef  MClsFavorite		T_Item;

	MClsFavoriteArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		//| ModelOption::CascadeLoad
		);

	virtual std::shared_ptr<IModel> CreateChild()override;
	virtual const std::vector<Field>& GetFieldVector()const override;
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

	virtual void LoadChilds()override;

	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>&, const size_t)override;

};


}//namespace wh{
#endif // __****_H