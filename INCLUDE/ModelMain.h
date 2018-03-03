#ifndef __MODELMAIN_H
#define __MODELMAIN_H

#include "IModelWindow.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelMain : public IModelWindow
{

public:
	ModelMain();

	virtual void UpdateTitle()override
	{
		sigUpdateTitle("wh3 main frame", wxNullIcon);
	}
	virtual void Load(const boost::property_tree::wptree& app_cfg)override;
	virtual void Save(boost::property_tree::wptree& app_cfg)override;
	
	sig::signal<void(const wxString&)> sigShowHelp;
};
//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H