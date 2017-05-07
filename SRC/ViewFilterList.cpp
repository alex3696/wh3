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
using namespace wh;
//-----------------------------------------------------------------------------
class FilterCtrl
{
public:
	virtual wxString GetStrValue()const = 0;
	virtual void SetStrValue(const wxString& str) = 0;
};
//-----------------------------------------------------------------------------
class FilterDateTimeCtrl
	: public wxPanel
	, public FilterCtrl
{
	wxDatePickerCtrl*	mDateCtrl;
	wxTimePickerCtrl*	mTimeCtrl;
public:
	FilterDateTimeCtrl(wxWindow *parent, wxWindowID id = wxID_ANY
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
			wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
			vec = dt.Format(format_dt);
		}
		return vec;
	}
	void SetValue(const wxString& str)
	{
		wxDateTime dt;
		wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
		dt.ParseFormat(str, format_dt);
		SetDtValue(dt);
	}
	virtual wxString GetStrValue()const override
	{
		return GetValue();
	}
	virtual void SetStrValue(const wxString& str)override
	{
		SetValue(str);
	}
};
//-----------------------------------------------------------------------------
class FilterIntCtrl 
	: public wxSpinCtrl
	, public FilterCtrl
{
public:
	FilterIntCtrl(wxWindow *parent,	wxWindowID id = wxID_ANY)
		//:mCtrl(new wxSpinCtrl(parent, id))
		:wxSpinCtrl(parent, id)
	{
		this->SetRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	}
	virtual wxString GetStrValue()const override
	{
		return wxString::Format("%d", GetValue());
	}
	virtual void SetStrValue(const wxString& str)override
	{
		SetValue(str);
	}
};
//-----------------------------------------------------------------------------
class FilterDoubleCtrl
	: public wxSpinCtrlDouble
	, public FilterCtrl
{
public:
	FilterDoubleCtrl(wxWindow *parent, wxWindowID id = wxID_ANY)
		//:mCtrl(new wxSpinCtrl(parent, id))
		:wxSpinCtrlDouble(parent, id)
	{
		this->SetRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
		this->SetIncrement(0.10);
	}
	virtual wxString GetStrValue()const override
	{
		return wxString::Format("%f", GetValue());
	}
	virtual void SetStrValue(const wxString& str)override
	{
		SetValue(str);
	}
};
//-----------------------------------------------------------------------------
class FilterTextCtrl
	: public wxComboBtn
	, public FilterCtrl
{
public:
	FilterTextCtrl(wxWindow *parent, wxWindowID id = wxID_ANY)
		:wxComboBtn(parent, id)
	{
	}
	virtual wxString GetStrValue()const override
	{
		return  GetValue();
	}
	virtual void SetStrValue(const wxString& str)override
	{
		SetValue(str);
	}
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

	virtual wxString GetStrValue()const override
	{
		return wxString::Format("%s\t%s"
			, mBeginCtrl->GetStrValue()
			, mEndCtrl->GetStrValue()
			);
	}
	virtual void SetStrValue(const wxString& str)override
	{
		int pos = str.Find('\t');
		if (wxNOT_FOUND != pos)
		{
			mBeginCtrl->SetValue(str.SubString(0,pos));
			mEndCtrl->SetValue(str.SubString(pos, str.size()));
		}
	}

};
//-----------------------------------------------------------------------------
class wxCollapsibleFilterPane 
	: public wxCollapsiblePane
	, public wh::ViewFilterCtrl
{
	//subcontrols
	wxChoice* mOpCtrl;

	


	unsigned int mDelBtnId;
	std::map<unsigned int, std::pair<wxSizer*, FilterCtrl*> > mCtrl;
	wh::FilterOp	mFilterOp;
	wh::FieldType	mFieldType;

	wxWindow* MkCtrl()const
	{
		switch (mFieldType)
		{
		case wh::ftText:
		case wh::ftName:
		case wh::ftLink:
		case wh::ftFile:
			return AddCtrl<FilterTextCtrl>();
			break;
		case wh::ftLong:
			return AddCtrl<FilterIntCtrl>();
			break;
		case wh::ftDouble:
			return AddCtrl<FilterDoubleCtrl>();
			break;
		case wh::ftDateTime:	
			return  AddCtrl<FilterDateTimeCtrl>();
			break;
			//case wh::ftDate:	AddCtrl<wxDatePickerCtrl>(filter->GetKind()); break;
			//case wh::ftTime:	AddCtrl<wxTimePickerCtrl>(filter->GetKind()); break;
		default:break;
		}//switch
		return nullptr;
	}

	template <class EDITOR>
	wxWindow* AddCtrl()const
	{
		wxWindow* win = GetPane();

		wxWindow* ctrl = nullptr;
		if (wh::foBetween == mFilterOp)
			ctrl = new IntervalCtrl<EDITOR> (win, wxID_ANY);
		else
			ctrl = new EDITOR(win, wxID_ANY);
		
		return ctrl;

	}

	FilterCtrl* AppendItemCtrl()
	{
		auto win = GetPane();
		wxWindowUpdateLocker lock(win);
		wxSizer *mainSz = win->GetSizer();
		wxSizer *paneSz = new wxBoxSizer(wxHORIZONTAL);

		auto ctrl = MkCtrl();

		auto btn_delete = new wxButton(win, mDelBtnId, wxEmptyString
			, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT | wxBORDER_NONE);
		btn_delete->SetBitmap(wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON));
		btn_delete->SetBitmapHover(wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON));

		paneSz->Add(ctrl, 1, wxEXPAND | wxALL, 2);
		paneSz->Add(btn_delete, 0, wxALL, 2);

		mainSz->Add(paneSz, 1, wxEXPAND | wxALL, 2);

		auto filter_ctrl = dynamic_cast<FilterCtrl*>(ctrl);
		std::pair<wxSizer*, FilterCtrl*> content = std::make_pair(paneSz, filter_ctrl);
		mCtrl.emplace(std::make_pair(mDelBtnId, content));

		auto scrolled_wnd = this->GetParent();
		scrolled_wnd->FitInside();

		Bind(wxEVT_COMMAND_BUTTON_CLICKED, &wxCollapsibleFilterPane::OnDeleteBtn
			, this, mDelBtnId);
		mDelBtnId++;
		return filter_ctrl;
	}
	void RemoveItemCtrl(unsigned int id)
	{
		auto win = GetPane();
		wxWindowUpdateLocker lock(win);

		auto ctrlSizer = mCtrl[id].first;
		mCtrl.erase(id);
		ctrlSizer->Clear(true);

		win->GetSizer()->Remove(ctrlSizer);
		win->InvalidateBestSize();

		auto scrolled_wnd = this->GetParent();
		scrolled_wnd->FitInside();
	}
	void RemoveAllCtrl()
	{
		auto dif = mCtrl.size();
		while (dif)
		{
			auto id = mCtrl.crbegin()->first;
			RemoveItemCtrl(id);
			dif--;
		}
	}
	
	void OnAddValueEditor(wxCommandEvent& evt = wxCommandEvent())
	{
		AppendItemCtrl();
	}
	void OnDeleteBtn(wxCommandEvent& evt = wxCommandEvent())
	{
		auto id = evt.GetId();
		RemoveItemCtrl(id);

	}//void OnDeleteBtn(wxCommandEvent& evt = wxCommandEvent())

	void OnCmd_SelectOperation(wxCommandEvent& evt = wxCommandEvent())
	{
		int sel = evt.GetSelection();
		if (mFilterOp != sel)
		{
			mFilterOp = (wh::FilterOp)sel;
			RemoveAllCtrl();
		}
			
	}
public:
	wxCollapsibleFilterPane(wxWindow *parent
		, wxWindowID id = wxID_ANY, const wxString &label = wxEmptyString
		, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize
		, long style = wxTAB_TRAVERSAL | wxBORDER_NONE | wxCP_NO_TLW_RESIZE)
		:wxCollapsiblePane(parent, id, label, pos, size, style)
		, mDelBtnId(0)
	{
		wxWindow *win = GetPane();
		wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);

		wxSizer *horSz = new wxBoxSizer(wxHORIZONTAL);
		auto btn_add = new wxBitmapButton(win, wxID_ADD
			, wxArtProvider::GetBitmap(wxART_PLUS, wxART_BUTTON)
			, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

		mOpCtrl = new wxChoice(win, wxID_PROPERTIES
			, wxDefaultPosition, wxDefaultSize
			, wh::AllFilterOpStringArray::GetInstance()->GetStringArray(), 0);
		mOpCtrl->SetSelection(0);

		//auto ck_enable = new wxCheckBox(this, wxID_ANY, "вкл.");
		//horSz->Add(ck_enable, 0, wxALL, 0);

		horSz->Add(mOpCtrl, 0, wxALL, 0);
		horSz->Add(0, 0, 1, wxEXPAND, 0);
		horSz->Add(btn_add, 0, wxALL, 0);
		mainSz->Add(horSz, 0, wxEXPAND | wxALL, 0);

		win->SetSizer(mainSz);
		mainSz->SetSizeHints(win);
		this->Layout();
		this->SetAutoLayout(true);

		Bind(wxEVT_COMMAND_BUTTON_CLICKED, &wxCollapsibleFilterPane::OnAddValueEditor
			, this, wxID_ADD);

		Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent& evt)
		{
			this->GetParent()->FitInside();	//this->GetParent()->Layout();
		});

		Bind(wxEVT_CHOICE, &wxCollapsibleFilterPane::OnCmd_SelectOperation, this, wxID_PROPERTIES);
	}

	std::vector<wxString>	GetStrVecValue()const
	{
		std::vector<wxString> vec;
		for (const auto& editor : mCtrl)
		{
			const FilterCtrl* editor_vec = editor.second.second;
			vec.emplace_back(editor_vec->GetStrValue());
		}
		return vec;
	}
	void SetStrVecValue(const std::vector<wxString>& vec)
	{
		auto dif = vec.size() - mCtrl.size();
		if (dif > 0)// values not present in ctrl
		{ 
			while (dif)
			{
				AppendItemCtrl();
				dif--;
			}
		}
		if (dif < 0)
		{
			while (dif)
			{
				auto id = mCtrl.crbegin()->first;
				RemoveItemCtrl(id);
				dif++;
			}
		}
		
		auto it = mCtrl.begin();
		for (const auto& str_val : vec)
		{
			FilterCtrl* editor = (*it).second.second;
			editor->SetStrValue(str_val);
			++it;
		}

	}

	// ViewFilterCtrl overrides
	virtual wxString GetSysTitle()const override
	{
		return this->GetName();
	}
	virtual void SetFilter(const wxString& title, const wxString& sys_title
		, FilterOp op, FieldType type
		, const std::vector<wxString>& val) override
	{
		SetLabel(title);
		SetName(sys_title);

		if (type != mFieldType || mFilterOp != op)
			RemoveAllCtrl();
		
		mFilterOp = op;
		mOpCtrl->SetSelection(op);
		mFieldType = type;
		SetStrVecValue(val);
	}
	virtual void GetFilter(wxString& title, wxString& sys_title
		, FilterOp& op, FieldType& type
		, std::vector<wxString>& val)const override
	{
		title = this->GetLabel();
		sys_title = this->GetName();
		op = mFilterOp;
		type = mFieldType;
		val = GetStrVecValue();
	}


};//wxCollapsibleFilterPane
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewFilterList::ViewFilterList(wxWindow* parent)
{
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	mPanel = new wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	mPanel->SetScrollRate(3, 3);
	//mPanel->SetBackgroundColour(*wxWHITE);
	mBtnApply = new wxButton(mPanel, wxID_APPLY, wxT("Применить"));
	mainSz->Add(mBtnApply, 0, wxALL | wxEXPAND, 5);

	mFilterSizer = new wxBoxSizer(wxVERTICAL);
	mainSz->Add(mFilterSizer, 1, wxALL | wxEXPAND, 0);
	mPanel->SetSizer(mainSz);
	mPanel->Layout();

	//mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewFilterList::OnCmd_Update, this, wxID_REFRESH);
	//mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewFilterList::OnCmd_UpdateAll, this, wxID_SETUP);
	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewFilterList::OnCmd_Apply, this, wxID_APPLY);
		
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
	if (data.empty())
	{
		mCtrlList.clear();
		mFilterSizer->Clear(true);
	}
		
	
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
void ViewFilterList::OnCmd_Apply(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	ViewToModel();
	sigApply();
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
	collpane->SetFilter(filter->GetTitle(), filter->GetSysTitle(),
		filter->GetOperation(), filter->GetFieldType()
		, filter->GetValueVec()	);
	collpane->GetPane()->SetBackgroundColour(wxColour(250, 250, 250));
	mFilterSizer->Add(collpane, 0, wxEXPAND | wxALL, 5);
	mCtrlList.emplace_back(collpane);

	if (filter->GetValueVecSize())
		collpane->Expand();
	
}
//-----------------------------------------------------------------------------

void ViewFilterList::Delete(const std::shared_ptr<const ModelFilter>& filter)
{
	auto& idxSysTile = mCtrlList.get<1>();
	auto it = idxSysTile.find(filter->GetSysTitle());
	if (idxSysTile.end() != it)
		idxSysTile.erase(it);

	auto wnd = mPanel->FindWindowByName(filter->GetSysTitle());
	if (wnd)
	{
		mFilterSizer->Remove(wnd->GetId());
	}
}
//-----------------------------------------------------------------------------

void ViewFilterList::Update(const std::shared_ptr<const ModelFilter>& old_filter
	, const std::shared_ptr<const ModelFilter>& new_filter)
{
	auto& idxSysTile = mCtrlList.get<1>();
	auto it = idxSysTile.find(old_filter->GetSysTitle());
	if (idxSysTile.end() != it)
	{
		(*it)->SetFilter(new_filter->GetTitle(), new_filter->GetSysTitle(),
			new_filter->GetOperation(), new_filter->GetFieldType()
			, new_filter->GetValueVec());
	}
}
//-----------------------------------------------------------------------------
//virtual 
void ViewFilterList::ViewToModel()// override;
{
	for (const auto& ctrl : mCtrlList)
	{
		wxString title;
		wxString sys_title;
		FilterOp op;
		FieldType type;
		std::vector<wxString> val;
		
		ctrl->GetFilter(title, sys_title, op, type, val);
		sigUpdateFilter(title, sys_title, op, type, val);
	}
}