#ifndef __MFAVFILTERPATH_H
#define __MFAVFILTERPATH_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{

/** Модель фильтра по месту*/
class MFavFilterPath
	: public TModelData<rec::FavFilterPath>
{
public:
	MFavFilterPath(const char option
		= ModelOption::CommitSave); //: TModelData<rec::FavFilterCls>(option){}

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	virtual void LoadChilds()override;
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

};
//-------------------------------------------------------------------------
class MFavFilterPathArray
	: public TModelArray<MFavFilterPath>
{
public:
	MFavFilterPathArray(const char option =
		ModelOption::EnableNotifyFromChild
		| ModelOption::CascadeLoad); //: TModelArray<MFavFilterCls>(option){}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual std::shared_ptr<IModel> CreateChild()override;
};

}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H