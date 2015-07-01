#include <wx/combo.h> 


#ifndef __WXCOMBOBTN_H
#define __WXCOMBOBTN_H

/** контрол для выбора данных типа комбобокса, только с троеточием */
class wxComboBtn: public wxComboCtrl
{
public:
    wxComboBtn() : wxComboCtrl() { Init(); }

    wxComboBtn(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();
        Create(parent,id,value,
               pos,size,
               // Style flag wxCC_STD_BUTTON makes the button
               // behave more like a standard push button.
               style | wxCC_STD_BUTTON,
               validator,name);

        //
        // Prepare custom button bitmap (just '...' text)
        wxMemoryDC dc;
        wxBitmap bmp(12,16);
        dc.SelectObject(bmp);

        // Draw transparent background
        wxColour magic(255,0,255);
        wxBrush magicBrush(magic);
        dc.SetBrush( magicBrush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(0,0,bmp.GetWidth(),bmp.GetHeight());

        // Draw text
        wxString str = wxT("...");
        int w,h;
        dc.GetTextExtent(str, &w, &h, 0, 0);
        dc.DrawText(str, (bmp.GetWidth()-w)/2, (bmp.GetHeight()-h)/2);

        dc.SelectObject( wxNullBitmap );

        // Finalize transparency with a mask
        wxMask *mask = new wxMask( bmp, magic );
        bmp.SetMask( mask );

        SetButtonBitmaps(bmp,true);
    }

    virtual void OnButtonClick()
    {
		
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
		evt.SetEventObject(this);
		ProcessWindowEvent(evt);
    }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup))
    {
    }

private:
    void Init()
    {
        // Initialize member variables here
    }
};


#endif