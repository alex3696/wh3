#ifndef __DATAVIEW_MLTEXT_RENDERER_H
#define __DATAVIEW_MLTEXT_RENDERER_H

#include "_pch.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class wxDataViewIconMLTextRenderer : public  wxDataViewCustomRenderer
{
public:
	static wxString GetDefaultType() { return wxS("wxDataViewIconText"); }

	wxDataViewIconMLTextRenderer(const wxString &varianttype = GetDefaultType(),
		wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
		int align = wxDVR_DEFAULT_ALIGNMENT);

	virtual bool SetValue(const wxVariant &value);
	virtual bool GetValue(wxVariant &value) const;

	virtual bool Render(wxRect cell, wxDC *dc, int state);
	//void RenderText(const wxString& text,int xoffset,wxRect rect,wxDC *dc,int state);
	virtual wxSize GetSize() const;

	//virtual bool HasEditorCtrl() const { return true; }
	//virtual wxWindow* CreateEditorCtrl(wxWindow *parent, wxRect labelRect,
	//	const wxVariant &value);
	//virtual bool GetValueFromEditorCtrl(wxWindow* editor, wxVariant &value);

private:
	wxDataViewIconText   m_value;

protected:
	//wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer);
};
//---------------------------------------------------------------------------
#endif // __*_H