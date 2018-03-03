#ifndef __IMODELWINDOW_H
#define __IMODELWINDOW_H

#include "_pch.h"

namespace wh{
//---------------------------------------------------------------------------
class IModelWindow
{
public:
	virtual ~IModelWindow(){}

	sig::signal<void(const wxString&, const wxIcon&)>	sigUpdateTitle;
	sig::signal<void()>	sigClose;
	sig::signal<void()>	sigShow;

	virtual void UpdateTitle() 
	{
		sigUpdateTitle(wxEmptyString, wxNullIcon );
	}
	virtual void Show()
	{
		sigShow();
	}
	virtual void Close()		
	{ 
		sigClose();
	}

	virtual void Init()											{}
	virtual void Load(const boost::property_tree::wptree& val)	{}
	virtual void Save(boost::property_tree::wptree& val) 		{}

};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H