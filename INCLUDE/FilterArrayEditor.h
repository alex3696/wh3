#ifndef __FILTERARRAYEDITOR_H
#define __FILTERARRAYEDITOR_H

#include "MTable.h"

namespace wh{
//-------------------------------------------------------------------------
class FilterArrayEditor
	:public wxScrolledWindow
{
public:
	FilterArrayEditor(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL,
		const wxString& name = wxPanelNameStr);

	void SetModel(std::shared_ptr<IFieldArray>& newModel);

protected:
	std::shared_ptr<IFieldArray> mModel;
};
//-------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H
