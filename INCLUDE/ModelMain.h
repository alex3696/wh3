#ifndef __MODELMAIN_H
#define __MODELMAIN_H

#include "IModelWindow.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelMain : public IModelWindow
{
	const wxIcon& mIco = wxNullIcon;
	const wxString mTitle = "wh3 main frame";

public:
	ModelMain();
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }

	virtual void Load(const boost::property_tree::wptree& app_cfg)override;
	virtual void Save(boost::property_tree::wptree& app_cfg)override;
	
	sig::signal<void(const wxString&)> sigShowHelp;
};
//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H