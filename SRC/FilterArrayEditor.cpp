#include "_pch.h"
#include "FilterArrayEditor.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// PathPatternEditor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterArrayEditor::FilterArrayEditor(wxWindow *parent,
	wxWindowID winid,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxScrolledWindow(parent, winid, pos, size, style, name)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	this->SetScrollRate(5, 5);
	wxBoxSizer* szrPath = new wxBoxSizer(wxHORIZONTAL);
	this->SetMinSize(wxSize(-1, 50));

	this->SetSizer(szrPath);
	this->Layout();
	szrPath->Fit(this);

	auto rc = ResMgr::GetInstance();


}

//---------------------------------------------------------------------------
void FilterArrayEditor::SetModel(std::shared_ptr<IFieldArray>& newModel)
{
	if (newModel == mModel)
		return;
	mModel = newModel;
	if (!mModel)
		return;

	namespace ph = std::placeholders;
	
}
