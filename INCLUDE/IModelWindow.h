#ifndef __IMODELWINDOW_H
#define __IMODELWINDOW_H

#include "_pch.h"

namespace wh{
//---------------------------------------------------------------------------
class IModelWindow
{
public:
	virtual ~IModelWindow()
	{
		sigClose();
	}

	sig::signal<void(const wxString&, const wxIcon&)>	sigUpdateTitle;
	sig::signal<void()>	sigClose;
	sig::signal<void()>	sigShow;

	virtual const wxIcon& GetIcon()const { return wxNullIcon; }
	virtual const wxString& GetTitle()const { return wxEmptyString2; }

	virtual void UpdateTitle() 
	{
		sigUpdateTitle(GetTitle(), GetIcon());
	}
	virtual void Show()
	{
		sigShow();
	}

	virtual void Load(const boost::property_tree::ptree& val)	{}
	virtual void Save(boost::property_tree::ptree& val) 		{}

};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H