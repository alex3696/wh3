#ifndef __DATAVIEW_MLTEXT_RENDERER_H
#define __DATAVIEW_MLTEXT_RENDERER_H

#include "_pch.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class wxDataViewMLTextRenderer : public wxDataViewTextRenderer
{
public:
	wxDataViewMLTextRenderer(
		const wxString &  varianttype = wxT("string"), //GetDefaultType(),
		wxDataViewCellMode  mode = wxDATAVIEW_CELL_INERT,
		int  align = wxDVR_DEFAULT_ALIGNMENT);

	virtual void RenderText(const wxString& text,
		int xoffset,
		wxRect rect,
		wxDC *dc,
		int state) override;

};
//---------------------------------------------------------------------------
#endif // __*_H