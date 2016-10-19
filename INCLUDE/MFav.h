#ifndef __MFAV_H
#define __MFAV_H

#include "MFavFilterCls.h"
#include "MFavFilterPath.h"
#include "MFavViewPropCls.h"
#include "MFavViewPropActCommon.h"
#include "MFavViewPropActUsr.h"
//-------------------------------------------------------------------------
namespace wh{

/** Модель узела избранного */
class MFav
	: public TModelData<rec::Fav>
{
public:
	MFav(const char option
		= ModelOption::CommitSave);

	std::shared_ptr<MFavFilterClsArray>			mFilterClsArray;
	//std::shared_ptr<MFavFilterPathArray>		mFilterPathArray;
	//std::shared_ptr<MFavViewPropClsArray>		mViewPropClsArray;
	//std::shared_ptr<MFavViewPropActCommonArray>	mViewPropActCommonArray;
	//std::shared_ptr<MFavViewPropActUsrArray>	mViewPropActUsrArray;

	virtual	bool GetFieldValue(unsigned int col, wxVariant &variant)override;
	virtual const std::vector<Field>& GetFieldVector()const override;
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
protected:
	//virtual void LoadChilds()override;
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;

};
//-------------------------------------------------------------------------
class MFavArray
	: public TModelArray<MFav>
{
public:
	MFavArray(const char option	= 
		  ModelOption::EnableNotifyFromChild
		| ModelOption::CascadeLoad
		| ModelOption::CascadeLoad
		)
		: TModelArray<MFav>(option)
	{
	}

protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual std::shared_ptr<IModel> CreateChild()override;
};



}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H