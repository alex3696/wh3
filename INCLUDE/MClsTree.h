#ifndef __MCLSTREE_H
#define __MCLSTREE_H

#include "TVec.h"
#include "db_rec.h"
#include "MClsBase.h"


namespace wh{

class MClsNodeArray;

class MClsPropArray;
class MClsActArray;
class MClsMoveArray;
class MClsObjNumArray;
class MClsObjQtyArray;

//-------------------------------------------------------------------------
class MClsNode
	: public MClsBase
{
public:

	MClsNode(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);

	std::shared_ptr<MClsNodeArray> GetClsArray();

	std::shared_ptr<MClsPropArray>	GetClsPropArray();
	std::shared_ptr<MClsActArray>	GetClsActArray();
	std::shared_ptr<MClsMoveArray>	GetClsMoveArray();
	std::shared_ptr<MClsObjNumArray>	GetClsObjNumArray();
	std::shared_ptr<MClsObjQtyArray>	GetClsObjQtyArray();

protected:
	std::shared_ptr<MClsNodeArray>	mClsArray;

	std::shared_ptr<MClsPropArray>	mPropArray;
	std::shared_ptr<MClsActArray>	mActArray;
	std::shared_ptr<MClsMoveArray>	mMoveArray;
	std::shared_ptr<MClsObjNumArray>	mObjNumArray;
	std::shared_ptr<MClsObjQtyArray>	mObjQtyArray;

	virtual void LoadChilds()override;

};
//-------------------------------------------------------------------------
/*
class MClsNode
	: public TModelData<rec::Cls>
{
public:

	MClsNode(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);
	~MClsNode();

	virtual const std::vector<Field>& GetFieldVector()const override;

	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)override;

	std::shared_ptr<MClsNodeArray> GetClsArray();

	std::shared_ptr<MClsPropArray>	GetClsPropArray();
	std::shared_ptr<MClsActArray>	GetClsActArray();
	std::shared_ptr<MClsMoveArray>	GetClsMoveArray();
	std::shared_ptr<MClsObjNumArray>	GetClsObjNumArray();
	std::shared_ptr<MClsObjQtyArray>	GetClsObjQtyArray();
	
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	
	
protected:
	std::shared_ptr<MClsNodeArray>	mClsArray;

	std::shared_ptr<MClsPropArray>	mPropArray;
	std::shared_ptr<MClsActArray>	mActArray;
	std::shared_ptr<MClsMoveArray>	mMoveArray;
	std::shared_ptr<MClsObjNumArray>	mObjNumArray;
	std::shared_ptr<MClsObjQtyArray>	mObjQtyArray;


	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;
	virtual void LoadChilds()override;

};
*/

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MClsNodeArray
	: public IModel
{
public:
	typedef  MClsNode		T_Item;

	MClsNodeArray(const char option
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