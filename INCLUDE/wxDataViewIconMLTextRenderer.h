#ifndef __DATAVIEW_MLTEXT_RENDERER_H
#define __DATAVIEW_MLTEXT_RENDERER_H

#include "_pch.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class wxDataViewIconText2 : public wxObject
{
public:

	wxDataViewIconText2(const wxString &text = wxEmptyString,
		const wxIcon& icon = wxNullIcon)
		: m_text(text),
		m_icon(icon)
	{

	}

	/*
	wxDataViewIconText2(wxString &&text = wxEmptyString
		,const wxIcon& icon = wxNullIcon)
		: m_text(text)
		, m_icon(icon)
	{ 
	
	}
	*/

	wxDataViewIconText2(const wxDataViewIconText2 &other)
		: wxObject(),
		m_text(other.m_text),
		m_icon(other.m_icon)
	{ }

	void SetText(const wxString &text) { m_text = text; }
	const wxString& GetText() const    { return m_text; }
	void SetIcon(const wxIcon &icon)   { m_icon = icon; }
	const wxIcon &GetIcon() const        { return m_icon; }

	bool IsSameAs(const wxDataViewIconText2& other) const
	{
		return m_text == other.m_text && m_icon.IsSameAs(other.m_icon);
	}

	bool operator==(const wxDataViewIconText2& other) const
	{
		return IsSameAs(other);
	}

	bool operator!=(const wxDataViewIconText2& other) const
	{
		return !IsSameAs(other);
	}

private:
	wxString    m_text;
	wxIcon      m_icon;

	wxDECLARE_DYNAMIC_CLASS(wxDataViewIconText2);
};

DECLARE_VARIANT_OBJECT(wxDataViewIconText2)

//---------------------------------------------------------------------------
class wxDataViewIconMLTextRenderer : public  wxDataViewCustomRenderer
{
public:
	static wxString GetDefaultType() { return wxS("wxDataViewIconText2"); }

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
	wxDataViewIconText2   m_value;
protected:
	//wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer);
};
//---------------------------------------------------------------------------
#endif // __*_H