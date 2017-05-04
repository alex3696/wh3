#include "_pch.h"
#include "ViewFilterList.h"
#include "globaldata.h"


//ftText = 0,			// =     Like			one multi	// textCtrl
//ftName = 1,			// =     Like			one multi	// textCtrl
//ftTextArray = 2,		// ??? = Like 
//ftLong = 100,			// interval = < >		one multi 	// textCtrl + textCtrl (validator)
//ftDouble = 101,		// interval = < >		one multi	// textCtrl + textCtrl (validator)
//ftDateTime = 200,		// interval = < >		one	multi	// dtCtrl + dtCtrl
//ftDate = 201,			// interval = < >		one	multi	// dCtrl + dCtrl
//ftTime = 202,			// interval = < >		one	multi	// tCtrl + tCtrl
//ftLink = 300,			// =     Like 			one multi	// textCtrl
//ftFile = 400,			// =     Like 			one multi	// textCtrl
//ftJSON = 500,			// =     Like 			one multi	// textCtrl
//ftBool = 600			// =					one	???		// chkboxCtrl
//-----------------------------------------------------------------------------
class wxDateTimeCtrl 
	: public wxPanel
{
	wxDatePickerCtrl*	mDateCtrl;
	wxTimePickerCtrl*	mTimeCtrl;
public:
	wxDateTimeCtrl(wxWindow *parent, wxWindowID id = wxID_ANY
		, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize)
		:wxPanel(parent, id, pos, size)
	{
		wxSizer *dtSz = new wxBoxSizer(wxHORIZONTAL);
		mDateCtrl = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime
			, wxDefaultPosition, wxDefaultSize
			, wxDP_DEFAULT | wxDP_SHOWCENTURY | wxDP_DROPDOWN);
		
		mTimeCtrl = new wxTimePickerCtrl(this, wxID_ANY);
		
		dtSz->Add(mDateCtrl, 0, wxEXPAND | wxALL, 1);
		dtSz->Add(mTimeCtrl, 0, wxEXPAND | wxALL, 1);
		this->SetSizer(dtSz);
		this->Layout();

	}

	wxDateTime GetDtValue()const
	{
		wxDateTime dt;
		
		auto d = mDateCtrl->GetValue();
		auto t = mTimeCtrl->GetValue();
		if (d.IsValid() && t.IsValid())
		{
			dt = mDateCtrl->GetValue().GetDateOnly();
			dt.SetHour(t.GetHour());
			dt.SetMinute(t.GetMinute());
			dt.SetSecond(t.GetSecond());
			dt.SetMillisecond(t.GetMillisecond());
		}
		return dt;
	}
	void SetDtValue(const wxDateTime& dt)
	{
		if (dt.IsValid())
		{
			mDateCtrl->SetValue(dt);
			mTimeCtrl->SetValue(dt);
		}
	}
	wxString GetValue()const 
	{
		wxString vec;
		wxDateTime dt = GetDtValue();
		if (dt.IsValid())
		{
			//wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
			vec= dt.Format();
		}
		return vec;
	}
	void SetValue(const wxString& str)
	{
		wxDateTime dt;
		dt.ParseDateTime(str);
		SetDtValue(dt);
	}
	

};
//-----------------------------------------------------------------------------

class FilterCtrl
{
public:
	FilterCtrl()
	{}

	virtual std::vector<wxString> GetStrVecValue()const = 0;
	virtual void SetStrVecValue(const std::vector<wxString>& vec) = 0;
};

//-----------------------------------------------------------------------------

template < class EDITOR >
class IntervalCtrl 
	: public wxPanel
	, public FilterCtrl
{
	EDITOR*	mBeginCtrl;
	EDITOR*	mEndCtrl;
public:
	IntervalCtrl(wxWindow *parent, wxWindowID id = wxID_ANY
		, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize)
		:wxPanel(parent, id, pos, size)
	{
		mBeginCtrl = new EDITOR(this, wxID_ANY);
		mEndCtrl = new EDITOR(this, wxID_ANY);
		auto mBeginLabel = new wxStaticText(this, wxID_ANY,"от");
		auto mEndLabel = new wxStaticText(this, wxID_ANY, "до");

		
		wxSizer *SzH1 = new wxBoxSizer(wxHORIZONTAL);
		SzH1->Add(mBeginLabel, 0, wxALL, 1);
		SzH1->Add(mBeginCtrl, 1, wxEXPAND | wxALL, 1);

		wxSizer *SzH2 = new wxBoxSizer(wxHORIZONTAL);
		SzH2->Add(mEndLabel, 0, wxALL, 1);
		SzH2->Add(mEndCtrl, 1, wxEXPAND | wxALL, 1);

		wxSizer *dtSz = new wxBoxSizer(wxVERTICAL);
		dtSz->Add(SzH1, 0, wxEXPAND | wxALL, 1);
		dtSz->Add(SzH2, 0, wxEXPAND | wxALL, 1);

		this->SetSizer(dtSz);
		this->Layout();

	}

	virtual std::vector<wxString>	GetStrVecValue()const override
	{
		std::vector<wxString> vec;
		vec.emplace_back(mBeginCtrl->GetValue());
		vec.emplace_back(mEndCtrl->GetValue());
		return vec;
	}
	virtual void SetStrVecValue(const std::vector<wxString>& vec)override
	{
		if (2 < vec.size())
		{
			mBeginCtrl->SetValue(vec[0]);
			mEndCtrl->SetValue(vec[1]);
		}
	}

};


//-----------------------------------------------------------------------------
template < class EDITOR >
class ValueCtrl: public wxPanel
{
	unsigned int mDelBtnId;
	std::map<unsigned int, wxSizer*> mCtrl;
public:
	ValueCtrl(wxWindow *parent, wxWindowID id = wxID_ANY
		, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize)
		:wxPanel(parent, id, pos, size), mDelBtnId(0)
	{
		wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
		this->SetSizer(mainSz);

		wxSizer *horSz = new wxBoxSizer(wxHORIZONTAL);
		auto btn_add = new wxBitmapButton(this, wxID_ADD
			, wxArtProvider::GetBitmap(wxART_PLUS, wxART_BUTTON)
			, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
		
		auto cb_type = new wxChoice(this, wxID_ANY
			, wxDefaultPosition, wxDefaultSize
			, wh::AllFilterOpStringArray::GetInstance()->GetStringArray(), 0);
		cb_type->SetSelection(0);

		//auto ck_enable = new wxCheckBox(this, wxID_ANY, "вкл.");
		//horSz->Add(ck_enable, 0, wxALL, 0);
		
		horSz->Add(cb_type, 0, wxALL, 0);
		horSz->Add(0, 0, 1, wxEXPAND, 0);
		horSz->Add(btn_add, 0, wxALL, 0);
		mainSz->Add(horSz, 0, wxEXPAND | wxALL, 0);
		this->Layout();

		Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ValueCtrl::OnSelectBtn, this, wxID_ADD);
		//OnSelectBtn();
		this->SetAutoLayout(true);
	}

	

	void OnSelectBtn(wxCommandEvent& evt = wxCommandEvent() )
	{
		//wxWindowUpdateLocker(this);

		wxSizer *mainSz = this->GetSizer();
		wxSizer *paneSz = new wxBoxSizer(wxHORIZONTAL);
		
		auto ctrl = new EDITOR(this, wxID_ANY);
		
		auto btn_delete = new wxButton(this, mDelBtnId, wxEmptyString
			, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT | wxBORDER_NONE );
		btn_delete->SetBitmap(wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON));
		btn_delete->SetBitmapHover(wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON));

		paneSz->Add(ctrl, 1, wxEXPAND | wxALL, 2);
		paneSz->Add(btn_delete, 0, wxALL, 2);
		
		//ctrl_panel->Layout();
		mainSz->Add(paneSz, 1, wxEXPAND | wxALL, 2);
		mCtrl.emplace(std::make_pair(mDelBtnId, paneSz));

		auto wnd = this->GetParent()->GetParent()->GetParent();
		wnd->FitInside();
		//this->Layout();

		Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ValueCtrl::OnDeleteBtn
			, this, mDelBtnId);
			//, this, mDelBtnId, -1, paneSz);
		mDelBtnId++;
	}

	void OnDeleteBtn(wxCommandEvent& evt = wxCommandEvent())
	{
		auto id = evt.GetId();
		auto ctrlSizer = mCtrl[id];
		mCtrl.erase(id);
		ctrlSizer->Clear(true);
		this->GetSizer()->Remove(ctrlSizer);

		this->GetParent()->InvalidateBestSize();
		
		auto scroll_wnd = this->GetParent()->GetParent()->GetParent();
		scroll_wnd->FitInside();

	}//void OnDeleteBtn(wxCommandEvent& evt = wxCommandEvent())

};


//-----------------------------------------------------------------------------
class wxCollapsibleFilterPane : public wxCollapsiblePane
{
public:
	wxCollapsibleFilterPane(wxWindow *parent
		, wxWindowID id = wxID_ANY, const wxString &label=wxEmptyString
		, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize
		, long style = wxTAB_TRAVERSAL | wxBORDER_NONE | wxCP_NO_TLW_RESIZE )
		:wxCollapsiblePane(parent, id, label, pos, size, style)
	{
		wxWindow *win = GetPane();
		wxSizer *paneSz = new wxBoxSizer(wxVERTICAL);
		//paneSz->Add(new wxStaticText(win, wxID_ANY, "test!"), 1, wxALL, 2);
		//paneSz->Add(new wxTextCtrl(win, wxID_ANY, "test_ctrl!"), 1, wxEXPAND | wxALL, 2);
		win->SetSizer(paneSz);
		paneSz->SetSizeHints(win);

		Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent& evt)
		{
			this->GetParent()->FitInside();
			//this->GetParent()->Layout();
		});
	}

	void SetFilter(const std::shared_ptr<const wh::ModelFilter>& filter)
	{
		SetLabel(filter->GetTitle());
		SetName(filter->GetSysTitle());

		wxWindow *win = GetPane();
		wxSizer *paneSz = win->GetSizer();

		auto type = filter->GetFieldType();
		
		switch (type)
		{
			case wh::ftDateTime:
				AddCtrl<wxDateTimeCtrl>(filter->GetKind());
				break;
			case wh::ftText:
				AddCtrl<wxComboBtn>(filter->GetKind());
				break;
			case wh::ftLong:	
			case wh::ftDouble:
				AddCtrl<wxTextCtrl>(filter->GetKind());
				break;

			default:break;
		}
	}

	template <class CTRL_TYPE>
	void AddCtrl(wh::FilterOp op)
	{
		wxWindow *win = GetPane();
		wxSizer *paneSz = win->GetSizer();

		wxWindow* ctrl = nullptr;
		if (wh::foBetween == op)
			ctrl = new ValueCtrl<IntervalCtrl<CTRL_TYPE>>(win);
		else
			ctrl = new ValueCtrl<CTRL_TYPE>(win);
		paneSz->Add(ctrl, 0, wxEXPAND | wxALL, 2);

	}

};
//-----------------------------------------------------------------------------



using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewFilterList::ViewFilterList(wxWindow* parent)
{
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	mPanel = new wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	mPanel->SetScrollRate(3, 3);
	//mPanel->SetBackgroundColour(*wxWHITE);

	//for (int i = 0; i < 20; i++)
	//{
	//	wxString title = "title ";
	//	title << i;
	//	auto collpane = new wxCollapsibleFilterPane(mPanel, wxID_ANY, title);
	//	mainSz->Add(collpane, 0, wxEXPAND | wxALL, 5);
	//}

	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewFilterList::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewFilterList::OnCmd_UpdateAll, this, wxID_SETUP);

	
	mPanel->SetSizer(mainSz);
	mPanel->Layout();
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
	if (data.empty())
		mPanel->GetSizer()->Clear(true);


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
	mPanel->FitInside();
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
	auto collpane = new wxCollapsibleFilterPane(mPanel);
	collpane->SetFilter(filter);
	mPanel->GetSizer()->Add(collpane, 0, wxEXPAND | wxALL, 5);

	
	collpane->GetPane()->SetBackgroundColour(wxColour(250, 250, 250));
	//collpane->SetBackgroundColour(*wxRED);
}
//-----------------------------------------------------------------------------

void ViewFilterList::Delete(const std::shared_ptr<const ModelFilter>& filter)
{
	auto wnd = mPanel->FindWindowByName(filter->GetSysTitle());
	if (wnd)
	{
		mPanel->GetSizer()->Remove(wnd->GetId());
	}
}
//-----------------------------------------------------------------------------

void ViewFilterList::Update(const std::shared_ptr<const ModelFilter>& new_filter
	, const std::shared_ptr<const ModelFilter>& old_filter)
{
	auto wnd = mPanel->FindWindowByName(new_filter->GetSysTitle());
	if (wnd)
	{
		//mPanel->GetSizer()->Remove(wnd->GetId());
	}

}
//-----------------------------------------------------------------------------

