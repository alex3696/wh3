#include "_pch.h"
#include "wxDataViewIconMLTextRenderer.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewIconText2, wxObject);

IMPLEMENT_VARIANT_OBJECT(wxDataViewIconText2)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wxDataViewIconMLTextRenderer::wxDataViewIconMLTextRenderer(
	const wxString &  varianttype,
	wxDataViewCellMode  mode,
	int  align)
	:wxDataViewCustomRenderer(varianttype, mode, align)
{

}
//---------------------------------------------------------------------------
//virtual 
bool wxDataViewIconMLTextRenderer::Render(wxRect rect, wxDC *dc, int state)
{
	int xoffset = 0;
	
	const wxIcon& icon = m_value.GetIcon();
	//const wxIcon& icon = wxArtProvider::GetIcon(wxART_FILE_SAVE, wxART_BUTTON);
	if (icon.IsOk())
	{
		dc->DrawIcon(icon, rect.x, rect.y + (rect.height - icon.GetHeight()) / 2);
		xoffset = icon.GetWidth() + 4;
	}
	
	RenderText(m_value.GetText(), xoffset, rect, dc, state);

	return true;
}
//---------------------------------------------------------------------------
/*
void wxDataViewIconMLTextRenderer::RenderText(const wxString& text,
	int xoffset,
	wxRect rect,
	wxDC *dc,
	int state) //override
{
	wxDataViewCustomRenderer::RenderText(text, xoffset, rect, dc, state);
	return;


	//wxCoord widthText, heightText, heightLine;
	//dc->GetMultiLineTextExtent(text, &widthText, &heightText, &heightLine);

	//if (heightLine && heightText > heightLine)
	//{
	//	auto yu1 = GetView()->GetTargetWindow()->GetSize();
	//	auto yu = GetSize();
	//	auto yu2 = dc->GetSize();

	//	int qty = heightText / heightLine - 1;
	//	//int rh = GetView()->GetCharHeight() * 3 + 2;
	//	//int max_offset = (rh - heightLine) / 2;
	//	int calc_offset = qty* (heightLine / 2);
	//	//int curr_offset = (calc_offset > max_offset) ? max_offset : calc_offset;
	//	int curr_offset = calc_offset;
	//	rect.y = rect.y - curr_offset;
	//	rect.height = heightText;
	//	rect.width = widthText;
	//}
	//wxDataViewTextRenderer::RenderText(text, xoffset, rect, dc, state);

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
	//dc->SetTextForeground(GetView()->GetTargetWindow()->GetForegroundColour());

	// get the alignment to use
	dc->DrawLabel(ellipsizedText.empty() ? text : ellipsizedText,
					rectText, GetEffectiveAlignment());
}
*/
//---------------------------------------------------------------------------
//virtual 
wxSize wxDataViewIconMLTextRenderer::GetSize() const //override;
{
	//wxString m_text = m_value.GetText();
	const wxString& m_text = m_value.GetText();
	if (!m_text.empty())
	{
		size_t sz = m_text.size();
		size_t nStart = 0; 
		//size_t line = 0;
		wxSize text_sz;
		while (nStart < sz)
		{
			size_t nEnd = m_text.find('\n', nStart);
			if (MAXSIZE_T == nEnd)
				nEnd = sz;
			wxSize curr_sz = GetTextExtent(m_text.SubString(nStart, nEnd));
			text_sz.x = curr_sz.x > text_sz.x ? curr_sz.x : text_sz.x;
			text_sz.y += curr_sz.y;
			nStart = nEnd + 1;
			//line++;
		}
		const wxIcon& icon = m_value.GetIcon();
		if (icon.IsOk())
			text_sz.x += icon.GetWidth() + 4;
		return text_sz;
	}
	return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
}
//---------------------------------------------------------------------------
bool wxDataViewIconMLTextRenderer::SetValue(const wxVariant &value)
{
	m_value << value;
	return true;
}
//---------------------------------------------------------------------------
bool wxDataViewIconMLTextRenderer::GetValue(wxVariant& value) const
{
	value << m_value;
	return true;
}