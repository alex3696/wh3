///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PNLFORMBUILDER_H__
#define __PNLFORMBUILDER_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class pnlSlotCommon
///////////////////////////////////////////////////////////////////////////////
class pnlSlotCommon : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_txtSlotClass;
		wxButton* m_btnSlotClass;
		wxStaticText* lblMaxQty;
		wxTextCtrl* m_txtMaxQty;
		wxStaticText* lblMinQty;
		wxTextCtrl* m_txtMinQty;
	
	public:
		
		pnlSlotCommon( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 515,371 ), long style = wxTAB_TRAVERSAL ); 
		~pnlSlotCommon();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class pnlSlotPerm
///////////////////////////////////////////////////////////////////////////////
class pnlSlotPerm : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_txtRolname1;
		wxButton* m_btnRolname1;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText181;
		wxTextCtrl* m_textCtrl10;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_txtRolname11;
		wxButton* m_btnRolname11;
		wxStaticText* m_staticText16;
		wxListCtrl* m_listCtrl5;
		wxBitmapButton* m_bpButton1;
		wxBitmapButton* m_bpButton2;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_textCtrl101;
		wxStaticText* m_staticText101;
		wxTextCtrl* m_txtRolname111;
		wxButton* m_btnRolname111;
		wxStaticText* m_staticText17;
		wxListCtrl* m_listCtrl51;
		wxBitmapButton* m_bpButton11;
		wxBitmapButton* m_bpButton21;
	
	public:
		
		pnlSlotPerm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 602,440 ), long style = wxTAB_TRAVERSAL ); 
		~pnlSlotPerm();
	
};

#endif //__PNLFORMBUILDER_H__
