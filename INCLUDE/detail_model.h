#ifndef __DETAIL_MODEL_H
#define __DETAIL_MODEL_H

#include "TModelArray.h"
#include "db_rec.h"


namespace wh{
class MLogTable;
namespace detail{
namespace model{
//-----------------------------------------------------------------------------
class ObjPropArray;
class ObjPropValLoader;
class ClsPropArray;

//-----------------------------------------------------------------------------
class Obj
	: public TModelData<rec::ObjInfo>
{
public:
	Obj(const char option = ModelOption::EnableNotifyFromChild);

	void SetObject(const rec::ObjInfo& oi);

	std::shared_ptr<ClsPropArray>	GetClsPropArray()const { return mClsProp; }
	std::shared_ptr<ObjPropArray>	GetObjPropArray()const { return mObjProp; }
	std::shared_ptr<MLogTable>		GetObjHistory()const { return mLogModel; }
protected:
	std::shared_ptr<ClsPropArray>	mClsProp;
	std::shared_ptr<ObjPropArray>	mObjProp;
	std::shared_ptr<MLogTable>		mLogModel;

	virtual void LoadChilds()override;

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	virtual bool GetSelectQuery(wxString&)const override;

private:
	std::shared_ptr<ObjPropValLoader>	mObjPropValLoader;
};
//-----------------------------------------------------------------------------
class ClsProp
	: public TModelData<rec::PropVal>
{
public:
	ClsProp(const char option = ModelOption::EnableParentNotify);
	bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
};
//-----------------------------------------------------------------------------
class ClsPropArray
	: public TModelArray<ClsProp>
{
public:
	ClsPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;
};
//-----------------------------------------------------------------------------
class ObjProp
	: public TModelData<rec::PropVal>
{
public:
	ObjProp(const char option = ModelOption::EnableParentNotify);
	bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	void SetSelected(bool select = false) { mSelected = select;	}
	bool GetSelected()const { return mSelected; }
protected:
	bool mSelected = false;
};
//-----------------------------------------------------------------------------
class ObjPropArray
	: public TModelArray<ObjProp>
{
public:
	ObjPropArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	void SetPropArray(const wxString& prop_str, const wxString& act_id_str);

	void UnselectAll();
protected:
	virtual bool GetSelectChildsQuery(wxString& query)const override;

	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& db, const size_t pos) override;


	// ObjPropArray - будет хранить все уникальные свойства для обьекта
	struct extr_pid
	{
		typedef const wxString result_type;
		inline result_type operator()(const std::shared_ptr<ObjProp>& r)const
		{
			return r->GetData().mProp.mId.toStr();
		}
	};

	using UniqueIndex_PId =
		boost::multi_index_container
		<
			std::shared_ptr<ObjProp>,
			indexed_by
			<
				ordered_unique< extr_pid >
			>
		>;

	using ActProps = std::map<wxString, std::shared_ptr<UniqueIndex_PId>>;

	ActProps mActProps;
	UniqueIndex_PId mUniqueIndex_PId;

	sig::scoped_connection		mConnRowAR;
	void OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);

};
//-----------------------------------------------------------------------------
class ObjPropValLoader
	: public IModel
{
public:
	ObjPropValLoader(const char option = 0);

	virtual void LoadData()override;

	bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t);
	bool GetSelectQuery(wxString&)const;

};


//-----------------------------------------------------------------------------
}//namespace model {
}//namespace detail {
}//namespace wh{
#endif // __****_H

