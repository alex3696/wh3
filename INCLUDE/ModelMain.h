#ifndef __MODELMAIN_H
#define __MODELMAIN_H

#include "IModelWindow.h"
#include "ResManager.h"

#include "CtrlNotebook.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelMain : public IModelWindow
{
	const wxIcon& mIco = wxNullIcon;
	const wxString mTitle = "wh3 main frame";

	std::shared_ptr<CtrlNotebook> mCtrlNotebook;
public:
	ModelMain()
	{

	}
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }

	virtual void Load(const boost::property_tree::wptree& val)override { }
	virtual void Save(boost::property_tree::wptree& page)override { }

};
//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H