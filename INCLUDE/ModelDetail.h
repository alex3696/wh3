#ifndef __MODEL_DETAIL_H
#define __MODEL_DETAIL_H

#include "ModelHistory.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelObjDetail
{
	//std::shared_ptr<rec::ObjInfo> mObjInfo;
	rec::ObjInfo mObjInfo;
public:
	ModelObjDetail();
	
	void Load();
	const rec::ObjInfo& Get()const;
	void Set(const wxString& oid, const wxString& parent_oid);

	sig::signal<void(const rec::ObjInfo&)>	sigObjDetailUpdated;

};
//---------------------------------------------------------------------------

class ModelPageDetail : public IModelWindow
{
	sig::scoped_connection connModel_ObjDetailUpdated;
public:
	std::shared_ptr<ModelObjDetail>		mModelObjDetail;
	std::shared_ptr<ModelPageHistory>	mModelPageHistory;

	ModelPageDetail(const std::shared_ptr<rec::ObjInfo>& oi
					,const std::shared_ptr<rec::PageHistory>& data);

	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override { sigShow(); }
	virtual void Init()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

};
//---------------------------------------------------------------------------





} //namespace mvp{
#endif // __IMVP_H