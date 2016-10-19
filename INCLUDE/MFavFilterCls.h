#ifndef __MFAVFILTERCLS_H
#define __MFAVFILTERCLS_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{

/** Модель фильтра по классу*/
class MFavFilterCls
	: public TModelData<rec::FavFilterCls>
{
public:
	MFavFilterCls(const char option
		= ModelOption::CommitSave); //: TModelData<rec::FavFilterCls>(option){}

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	//virtual void LoadChilds()override;
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

};
//-------------------------------------------------------------------------
class MFavFilterClsArray
	: public TModelArray<MFavFilterCls>
{
public:
	typedef  MFavFilterCls		T_Item;

	MFavFilterClsArray(const char option =
		ModelOption::EnableNotifyFromChild
		| ModelOption::CascadeLoad);

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual std::shared_ptr<IModel> CreateChild()override;
};

}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H