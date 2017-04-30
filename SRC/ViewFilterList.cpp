#include "_pch.h"
#include "ViewFilterList.h"
#include "globaldata.h"
#include <wx/collpane.h>
#include "wxDateTimeProperty.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(whModelFilter)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPGPFilterProperty, wxPGProperty,
whModelFilter, const whModelFilter&, TextCtrl)


//-----------------------------------------------------------------------------
wxPGPFilterProperty::wxPGPFilterProperty(const wxString& label,
					const wxString& name, const whModelFilter& value)
					//:BtnProperty(label, name)
	:wxPGProperty(label, name)
{
	//const whModelFilter& parent = whModelFilterRefFromVariant(m_value);
	//ChangeFlag(wxPG_PROP_READONLY, true);
	//this->SetEditor("CheckBox");

	const auto kind = value.GetKind();
	const auto ft = value.GetFieldType();

	if (wh::FilterKind::EqIntervalValue == kind && wh::ftDateTime == ft)
	{
		whModelFilter val = value;
		auto vec = val.GetValueVec();
		vec.resize(2);
		if (vec[0].IsEmpty())
			vec[0] = wxDateTime::Now().Format();
		if (vec[1].IsEmpty())
			vec[1] = wxDateTime::Now().Format();
		val.SetValue(vec);


		this->SetValue(WXVARIANT(val));
		//const whModelFilter& data = whModelFilterRefFromVariant(m_value);
		//wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
		wxDateTime begin_dt, end_dt;
		begin_dt.ParseDateTime(val.GetValue(0));
		end_dt.ParseDateTime(val.GetValue(1));

		auto pgp_begin = new wxDateTimeProperty("начало", wxPG_LABEL, begin_dt);
		auto pgp_end = new wxDateTimeProperty("конец", wxPG_LABEL, end_dt);

		AddPrivateChild(pgp_begin);
		AddPrivateChild(pgp_end);
		//this->SetExpanded(true);

	}
	else
		this->SetValue(WXVARIANT(value));


}
//-----------------------------------------------------------------------------
wxPGPFilterProperty::~wxPGPFilterProperty() { }
//-----------------------------------------------------------------------------
void wxPGPFilterProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	if ("whModelFilter" != m_value.GetType())
		return;
	const whModelFilter& parent = whModelFilterRefFromVariant(m_value);
	wxDateTime dt_begin, dt_end;
	dt_begin.ParseDateTime(parent.GetValue(0));
	dt_end.ParseDateTime(parent.GetValue(1));

	Item(0)->SetValue(WXVARIANT(dt_begin));
	Item(1)->SetValue(WXVARIANT(dt_end));

}
//-----------------------------------------------------------------------------
wxVariant wxPGPFilterProperty::ChildChanged(wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue) const
{
	whModelFilter filter;
	filter << thisValue;

	switch (childIndex)
	{
		case 0:
		{
			auto var = filter.GetValueVec();
			var.resize(2);
			wxDateTime dt = childValue.GetDateTime();
			wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);

			if (dt.IsValid())
				var[0] = childValue.GetDateTime().Format(format_dt);
			else
				var[0] = dt.Now().Format(format_dt);
			
			filter.SetValue(var);
		}
		break;
		case 1:
		{
			auto var = filter.GetValueVec();
			var.resize(2);
			wxDateTime dt = childValue.GetDateTime();
			wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);

			if (dt.IsValid())
				var[1] = childValue.GetDateTime().Format(format_dt);
			else
				var[1] = dt.Now().Format(format_dt);

			filter.SetValue(var);
		}
		break;

	}

	wxVariant newVariant;
	newVariant << filter;
	return newVariant;

}
//-----------------------------------------------------------------------------
wxString  wxPGPFilterProperty::ValueToString(wxVariant &  value, int  argFlags)  const
{
	if ("whModelFilter" == value.GetType())
	{
		const auto& obj = whModelFilterRefFromVariant(m_value);
		wxString ret;
		const std::vector<wxString>& vec = obj.GetValueVec();

		const wxString delim = " - ";
		for (const auto& v : vec)
		{
			ret << delim << v;
		}
		ret.Replace(delim, wxEmptyString, false);
		return ret;
	}
	//return "unknown type";
	return value.GetString();
}







class wxCollapsibleFilterPane : public wxCollapsiblePane
{
public:
	wxCollapsibleFilterPane(wxWindow *parent, wxWindowID id, const wxString &label
		, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize
		, long style = wxTAB_TRAVERSAL | wxBORDER_NONE | wxCP_NO_TLW_RESIZE )
		:wxCollapsiblePane(parent, id, label, pos, size, style)
	{
		wxWindow *win = GetPane();
		wxSizer *paneSz = new wxBoxSizer(wxHORIZONTAL);
		paneSz->Add(new wxStaticText(win, wxID_ANY, "test!"), 1, wxALL, 2);
		paneSz->Add(new wxTextCtrl(win, wxID_ANY, "test_ctrl!"), 1, wxALL, 2);
		win->SetSizer(paneSz);
		paneSz->SetSizeHints(win);

		Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent& evt)
		{
			this->GetParent()->FitInside();
			//this->GetParent()->Layout();
		});


	}

};



















using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxAuiPanel : public wxPanel
{
public:
	wxAuiPanel(wxWindow* wnd)
		:wxPanel(wnd)
	{
		mAuiMgr.SetManagedWindow(this);
	}
	~wxAuiPanel()
	{
		mAuiMgr.UnInit();
	}
	wxAuiManager	mAuiMgr;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewFilterList::ViewFilterList(wxWindow* parent)
{
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	auto panel = new wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	panel->SetScrollRate(5, 5);



	//auto panel = new wxAuiPanel(parent);
	//mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	panel->SetScrollbar(wxVERTICAL, 0, 10, 1, true);

	

	//auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	//auto face_colour = *wxRED;
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);


	for (int i = 0; i < 20; i++)
	{
		wxString title = "title ";
		title << i;

		//auto collpane = new wxPropertyGrid(mPanel);
		auto collpane = new wxCollapsibleFilterPane(mPanel, wxID_ANY, title);
		mainSz->Add(collpane, 0, wxEXPAND | wxALL, 5);
	}


	mPG = new wxPropertyGrid(mPanel);
	//panel->mAuiMgr.AddPane(mPG, wxAuiPaneInfo().
	//	Name("PropertyGrid").CaptionVisible(false)
	//	.CenterPane()
	//	.PaneBorder(false)
	//	);
	mainSz->Add(mPG, 0, wxEXPAND | wxALL, 5);
	mPG->SetWindowStyleFlag(wxPG_SPLITTER_AUTO_CENTER);
	mPG->CenterSplitter(true);
	mPG->SetMinSize(wxSize(100, 400));

	//auto pgp_end = new wxDateTimeProperty("ТестВремя", wxPG_LABEL);
	//mPG->Append(pgp_end);



	
	//panel->mAuiMgr.AddPane(collpane, wxAuiPaneInfo().
	//	Name("CollapsiblePane").CaptionVisible(false)
	//	.CenterPane()
	//	.PaneBorder(false)
	//	.MinSize(200, 200)
	//	);


	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewFilterList::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewFilterList::OnCmd_UpdateAll, this, wxID_SETUP);
	

	
	
	mPanel->SetSizer(mainSz);
	mPanel->Layout();
	//mAuiMgr->Update();
}
//-----------------------------------------------------------------------------
ViewFilterList::ViewFilterList(std::shared_ptr<IViewWindow> parent)
	:ViewFilterList(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------

wxWindow* ViewFilterList::GetWnd()const
{
	return mPanel;
}

//-----------------------------------------------------------------------------
//virtual 
void ViewFilterList::Update(const std::vector<NotyfyItem>& data) //override;
{
	auto p0 = GetTickCount();
	for (const auto& item : data)
	{
		if (item.first && item.second)
		{
			Update(item.first, item.second);
		}
		else if (item.first && !item.second)
		{
			Delete(item.first);
		}
		else if (!item.first && item.second)
		{
			Insert(item.second);
		}
		
	}


	//mPG->FitColumns();//		
	mPG->ExpandAll();
	
	//mPG->Layout();
	//mPG->Refresh();
	//mAuiMgr->Update();

	wxLogMessage(wxString::Format("%d \t ViewFilterList : \t Update", GetTickCount() - p0));
}
//-----------------------------------------------------------------------------
void ViewFilterList::OnCmd_UpdateAll(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

	sigUpdateAll();
}
//-----------------------------------------------------------------------------
void ViewFilterList::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

	sigUpdateAll();
}
//-----------------------------------------------------------------------------
//IViewWindow virtual 
void ViewFilterList::OnShow()//override 
{
	//OnCmd_Update(); 
}
//-----------------------------------------------------------------------------
void ViewFilterList::Insert(const std::shared_ptr<const ModelFilter>& filter
	, const std::shared_ptr<const ModelFilter>& before)
{
	auto old_pgp = mPG->GetProperty(filter->GetSysTitle());
	if (old_pgp)
	{
		auto new_pgp = MakePGP(filter);
		mPG->ReplaceProperty(old_pgp, new_pgp);
	}
	else
	{
		auto new_pgp = MakePGP(filter);
		mPG->Append(new_pgp);
	}
}
//-----------------------------------------------------------------------------

void ViewFilterList::Delete(const std::shared_ptr<const ModelFilter>& filter)
{
	auto pgp = mPG->GetProperty(filter->GetSysTitle());
	if (pgp)
		mPG->DeleteProperty(pgp);
}
//-----------------------------------------------------------------------------

void ViewFilterList::Update(const std::shared_ptr<const ModelFilter>& new_filter
	, const std::shared_ptr<const ModelFilter>& old_filter)
{
	auto old_pgp = mPG->GetProperty(old_filter->GetSysTitle());
	if (old_pgp)
	{
		auto new_pgp = MakePGP(new_filter);
		mPG->ReplaceProperty(old_pgp, new_pgp);
	}
		

}
//-----------------------------------------------------------------------------

wxPGProperty* ViewFilterList::MakePGP(const std::shared_ptr<const ModelFilter>& filter)
{
	return  new wxPGPFilterProperty(filter->GetTitle()
		, filter->GetSysTitle()
		, *filter);

}
