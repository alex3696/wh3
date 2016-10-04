#ifndef __MFAVVIEWPROPACTCOMMON_H
#define __MFAVVIEWPROPACTCOMMON_H

#include "TModelArray.h"
#include "db_rec.h"

//-------------------------------------------------------------------------
namespace wh{

/** Модель фильтра по месту*/
class MFavViewPropActCommon
	: public TModelData<rec::FavViewPropActCommon>
{
public:
	MFavViewPropActCommon(const char option
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
class MFavViewPropActCommonArray
	: public TModelArray<MFavViewPropActCommon>
{
public:
	MFavViewPropActCommonArray(const char option =
		ModelOption::EnableNotifyFromChild
		| ModelOption::CascadeLoad); //: TModelArray<MFavFilterCls>(option){}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual std::shared_ptr<IModel> CreateChild()override;
};

}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H