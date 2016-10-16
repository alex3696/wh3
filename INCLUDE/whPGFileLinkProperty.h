#ifndef __WHPGFILELINKPROPERTY_H
#define __WHPGFILELINKPROPERTY_H

#include "_pch.h"


class whPGFileLinkProperty
	: public wxStringProperty
{
public:
	whPGFileLinkProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString);
	~whPGFileLinkProperty();

	void SetOpenFunc(std::function<bool(wxPGProperty*)>& func);
	void SetExecFunc(std::function<bool(wxPGProperty*)>& func);
	const std::function<bool(wxPGProperty*)>& GetOpenFunc()const;
	const std::function<bool(wxPGProperty*)>& GetExecFunc()const;
protected:
	class LinkImpl;
	std::unique_ptr<LinkImpl> mImpl;

	std::function<bool(wxPGProperty*)> mExecFunc;
	std::function<bool(wxPGProperty*)> mOpenFunc;

};
//-------------------------------------------------------------------------------------------------



#endif //__*_H