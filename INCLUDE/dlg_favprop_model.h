#ifndef __DLG_FAVPROP_MODEL_H
#define __DLG_FAVPROP_MODEL_H

#include "db_rec.h"
#include "TModelArray.h"

namespace wh{
namespace dlg{
namespace favprop {
namespace model {

//-------------------------------------------------------------------------
class FavPropItem
	: public TModelData<rec::FavoriteProp>
	, public std::enable_shared_from_this<FavPropItem>
{
public:
	FavPropItem(const char option = ModelOption::EnableParentNotify	);

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	virtual void LoadChilds()override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;
};
//-------------------------------------------------------------------------	

class FavPropArray
	: public TModelArray<FavPropItem>
{
public:
	FavPropArray(const char option = ModelOption::EnableNotifyFromChild)
		: TModelArray<FavPropItem>(option)
	{}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

};

//-------------------------------------------------------------------------









} //namespace wh{
} //namespace dlg{
} //namespace favprop {
} //namespace model {

#endif // __****_H