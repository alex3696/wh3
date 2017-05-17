#ifndef __MODEL_DETAIL_H
#define __MODEL_DETAIL_H

#include "ModelHistory.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"
#include "ModelObjPropList.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelDetailPage : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_history24;
	const wxString mTitle = "Подробности";

	sig::scoped_connection connListItemChange;

	rec::PageHistory	mGuiModel;
	ModelHistory		mDataModel;
public:
	ModelDetailPage(const std::shared_ptr<rec::PageHistory>& data);

	ModelHistory& GetModelHistory(){ return mDataModel; }

	void Update();
	void PageForward();
	void PageBackward();

	void SetGuiModel(rec::PageHistory&& cfg);
	void SetGuiModel(const rec::PageHistory& cfg);
	const rec::PageHistory& GetGuiModel()const;
	sig::signal<void(const rec::PageHistory&)>	sigCfgUpdated;

	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Load(const boost::property_tree::ptree& page_val)override;
	virtual void Save(boost::property_tree::ptree& page_val)override;

};
//---------------------------------------------------------------------------





} //namespace mvp{
#endif // __IMVP_H