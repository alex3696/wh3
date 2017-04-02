#include "_pch.h"
#include "wxDataViewMLTextRenderer.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wxDataViewMLTextRenderer::wxDataViewMLTextRenderer(
	const wxString &  varianttype,
	wxDataViewCellMode  mode,
	int  align)
:wxDataViewTextRenderer(varianttype, mode, align)
{

}
//---------------------------------------------------------------------------
//virtual
void wxDataViewMLTextRenderer::RenderText(const wxString& text,
	int xoffset,
	wxRect rect,
	wxDC *dc,
	int state) //override
{
	/*
	wxCoord widthText, heightText, heightLine;
	dc->GetMultiLineTextExtent(text, &widthText, &heightText, &heightLine);

	if (heightLine && heightText > heightLine)
	{
		auto yu1 = GetView()->GetTargetWindow()->GetSize();
		auto yu = GetSize();
		auto yu2 = dc->GetSize();

		int qty = heightText / heightLine - 1;
		//int rh = GetView()->GetCharHeight() * 3 + 2;
		//int max_offset = (rh - heightLine) / 2;
		int calc_offset = qty* (heightLine / 2);
		//int curr_offset = (calc_offset > max_offset) ? max_offset : calc_offset;
		int curr_offset = calc_offset;
		rect.y = rect.y - curr_offset;
		rect.height = heightText;
		rect.width = widthText;
	}
	wxDataViewTextRenderer::RenderText(text, xoffset, rect, dc, state);
	*/
	wxRect rectText = rect;
	rectText.x += xoffset;
	rectText.width -= xoffset;

	// check if we want to ellipsize the text if it doesn't fit
	wxString ellipsizedText;
	if (GetEllipsizeMode() != wxELLIPSIZE_NONE)
	{
		ellipsizedText = wxControl::Ellipsize(
											text,
											*dc,
											GetEllipsizeMode(),
											rectText.width,
											wxELLIPSIZE_FLAGS_NONE
											);
	}
	//auto brush = dc->GetBrush();
	//brush.SetColour(*wxBLUE);
	//dc->SetBackground(brush);
	//auto pen = dc->GetPen();
	//pen.SetColour(*wxRED);
	//dc->SetLogicalFunction(wxSRC_INVERT);

	//dc->SetTextForeground(*wxBLACK);
	dc->SetTextForeground(GetView()->GetTargetWindow()->GetForegroundColour());
	//wxDCTextColourChanger changeFg(*dc, *wxRED);
	
	// get the alignment to use
	dc->DrawLabel(ellipsizedText.empty() ? text : ellipsizedText,
					rectText, GetEffectiveAlignment());
}

