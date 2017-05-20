#ifndef __MODEL_DETAIL_H
#define __MODEL_DETAIL_H

#include "ModelHistory.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageDetail : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_obj_num24;
	const wxString mTitle = "Подробности";

	rec::ObjInfo						mObjInfo;
	std::shared_ptr<ModelPageHistory>	mModelHistory;
public:
	ModelPageDetail(const std::shared_ptr<rec::ObjInfo>& oi
					,const std::shared_ptr<rec::PageHistory>& data);

	std::shared_ptr<ModelPageHistory>& GetModelHistory(){ return mModelHistory; }

	void Update();

	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Init()override;
	virtual void Load(const boost::property_tree::ptree& page_val)override;
	virtual void Save(boost::property_tree::ptree& page_val)override;

};
//---------------------------------------------------------------------------





} //namespace mvp{
#endif // __IMVP_H