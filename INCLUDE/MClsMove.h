#ifndef __MCLSMOVE_H
#define __MCLSMOVE_H

#include "PathPattern_model.h"
//#include "TModelArray.h"
//#include "db_rec.h"

namespace wh{

//-------------------------------------------------------------------------
class MClsMove : public TModelData<rec::ClsSlotAccess>
{
public:
	MClsMove(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;
	virtual const std::vector<Field>& GetFieldVector()const override;

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;

	//std::shared_ptr<temppath::model::Array>	mSrcPathArr;
	//std::shared_ptr<temppath::model::Array>	mDstPathArr;
protected:

	void OnChange(const IModel*, const DataType*);
	wxString mSrcPathGui;
	wxString mDstPathGui;
	sig::scoped_connection connChange;


	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;


};
//-------------------------------------------------------------------------
class MClsMoveArray : public TModelArray<MClsMove>
{
public:
	typedef  MClsMove		T_Item;

	MClsMoveArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

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