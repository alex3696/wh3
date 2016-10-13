#ifndef __BTNPROPERTY_H
#define __BTNPROPERTY_H

#include "BaseOkCancelDialog.h"




//-------------------------------------------------------------------------------------------------
/// Свойство с кнопкой "..."
class BtnProperty 
	//: public wxPGProperty 
	: public wxStringProperty
	
{
	//WX_PG_DECLARE_PROPERTY_CLASS(BtnProperty)
	bool mForceEnable = false;
	wxBitmap mBmp;
	wxString mTitle = wxString("...");
	wxPGEditor* mEditor=nullptr;
public:
	std::function<bool(wxPGProperty*)> mFunc;

	BtnProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString);
	~BtnProperty();

	void SetOnClickButonFunc(std::function<bool(wxPGProperty*)>& func);


	void ForceEnableButton(bool enable = true);
	void SetButtonBitmap(const wxBitmap& bmp);
	void SetButtonTitle(const wxString& str);

	bool GetForceEnableButton()const{ return mForceEnable; }
	const wxBitmap* GetButtonBitmap()const{ return &mBmp; };
	wxString GetButtonTitle()const{ return mTitle; };

};
//-------------------------------------------------------------------------------------------------




#endif //__*_H

