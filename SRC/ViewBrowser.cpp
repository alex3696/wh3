#include "_pch.h"
#include "ViewBrowser.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"
#include "wxComboBtn.h"
#include "config.h"

using namespace wh;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVTableBrowser
	: public wxDataViewModel
{
	const IIdent64* mCurrentRoot = nullptr;
	std::vector<const IIdent64*>	mClsList;


	bool	mGroupByType = true;
	//wxRegEx mStartNum = "(^[0-9]{1,9})";
	//wxRegEx mStartNum = "^(-?)(0|([1-9][0-9]*))((\\.|\\,)[0-9]+)?$";
	wxRegEx mStartNum = "^(-?)([0-9]*)((\\.|\\,)[0-9]+)?";

	const wxString format_d = wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
	const wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
	const wxString mActNotExecuted="не выполнялось";
public:
	ActColumns		mActColumns;
	PropColumns		mCPropColumns;
	PropColumns		mOPropColumns;

	wxDVTableBrowser(){};
	~wxDVTableBrowser(){};

	// implementation of base class virtuals to define model
	virtual unsigned int	GetColumnCount() const override
	{
		return 4;
	}
	virtual wxString		GetColumnType(unsigned int col) const override 
	{
		switch (col)
		{
		case 0: return "wxDataViewIconText";
		default: break;
		}
		return "string";
	}
	virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const override
	{
		return true;
	}

	virtual bool IsContainer(const wxDataViewItem &item)const override
	{
		if (!item.IsOk())
			return true;
		
		const auto node = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*>(node);
		if (mGroupByType 
			&& cls 
			&& cls != mCurrentRoot
			&& ClsKind::Abstract != cls->GetKind())
			return true;
		
		return false;
	}

	void GetErrorValue(wxVariant &variant, unsigned int col) const
	{
		if(0==col)
			variant << wxDataViewIconText("*ERROR*", wxNullIcon);
		else
			variant = "*ERROR*";
		return;
	}

	void GetClsValue(wxVariant &variant, unsigned int col
		, const ICls64& cls) const
	{
		auto mgr = ResMgr::GetInstance();

		if (mCurrentRoot == &cls)
		{
			if(0==col)
				variant << wxDataViewIconText("..", mgr->m_ico_back24); 
			return;
		}
		
		switch (col)
		{
		case 0: {
			const wxIcon*  ico(&wxNullIcon);
			switch (cls.GetKind())
			{
			case ClsKind::Abstract: ico = &mgr->m_ico_type_abstract24; break;
			case ClsKind::Single:	ico = &mgr->m_ico_type_num24; break;
			case ClsKind::QtyByOne:
			case ClsKind::QtyByFloat:
			default: ico = &mgr->m_ico_type_qty24;	break;
			}//switch
			variant << wxDataViewIconText(cls.GetTitle(), *ico);
		}break;
		//case 1: variant = wxString("qwe2");  break;
		case 2: {
			switch (cls.GetKind())
			{
			case ClsKind::Single: case ClsKind::QtyByOne: case ClsKind::QtyByFloat:
				variant = wxString::Format("%s (%s)", cls.GetObjectsQty(), cls.GetMeasure());
				break;
			case ClsKind::Abstract:
			default: break;
			}
		}break;
		//case 3: variant = wxString("qwe3");  break;
		default: {
			const auto& idxCol = mActColumns.get<1>();
			const auto it = idxCol.find(col);
			if (idxCol.end() != it)
			{
				if (FavAPropInfo::PeriodDay == it->mAInfo)
				{
					wxString period;
					if (cls.GetActPeriod(it->mAid, period))
					{
						double dperiod;
						if (period.ToCDouble(&dperiod))
							variant = wxString::Format("%g", dperiod );
					}
				}
				else
				{
					const auto& aprop_array = cls.GetFavAPropValue();
					const auto& idxFAV = aprop_array.get<0>();
					auto fit = idxFAV.find(boost::make_tuple(it->mAid, it->mAInfo));
					if (idxFAV.end() != fit)
						variant = (*fit)->mValue;
				}
				return;
			}//if (idxCol.end() != it)
			const auto& idxPropCol = mCPropColumns.get<1>();
			auto prop_it = idxPropCol.find(col);
			if (idxPropCol.end() != prop_it)
			{
				const auto favCPrpop = cls.GetFavCPropValue();
				auto val_it = favCPrpop.find(prop_it->mPid );
				if(favCPrpop.cend()!=val_it)
					variant = (*val_it)->mValue;
			}


		}break;
		}//switch (col)
	}

	void GetObjValue(wxVariant &variant, unsigned int col
		, const IObj64& obj) const
	{
		const wxIcon*  ico(&wxNullIcon);
		auto mgr = ResMgr::GetInstance();

		switch (col)
		{
		case 0: variant << wxDataViewIconText(obj.GetTitle(), *ico); break;
		case 1: variant = obj.GetCls()->GetTitle();	break;
		case 2: variant = wxString::Format("%s (%s)"
			, obj.GetQty()
			, obj.GetCls()->GetMeasure());
			break;
		case 3: variant = obj.GetPath()->AsString(); break;

		default:{
			const auto& idxCol = mActColumns.get<1>();
			const auto it = idxCol.find(col);
			if (idxCol.end() != it)
			{
				switch (it->mAInfo)
				{
				case FavAPropInfo::PreviosDate: {
					wxDateTime dt;
					int ret = obj.GetActPrevios(it->mAid, dt);
					switch (ret)
					{
					case 1: {
						if (dt.GetDateOnly() == dt)
							variant = wxString::Format("%s", dt.Format(format_d));
						else
							variant = wxString::Format("%s %s", dt.Format(format_d), dt.Format(format_t));
					}break;
					case -1: {
						variant = mActNotExecuted;
					}
					default: break;
					}
				}break;
				case FavAPropInfo::PeriodDay: if (!mGroupByType) {
					wxString period;
					if (obj.GetActPeriod(it->mAid, period))
					{
						double dperiod;
						if (period.ToCDouble(&dperiod))
						{
							variant = wxString::Format("%g", dperiod);
						}
					}
				} break;
				case FavAPropInfo::NextDate: {
					wxDateTime dt;
					int ret = obj.GetActNext(it->mAid, dt);
					switch (ret)
					{
					case 1: {
						if (dt.GetDateOnly() == dt)
							variant = wxString::Format("%s", dt.Format(format_d));
						else
							variant = wxString::Format("%s %s", dt.Format(format_d), dt.Format(format_t));
					}break;
					case -1: {
						variant = mActNotExecuted;
					}
					default: break;
					}//switch (ret)
				}break;
				case FavAPropInfo::LeftDay: {
					double left;
					int ret = obj.GetActLeft(it->mAid, left);
					switch (ret)
					{
					case 1: {
						variant = wxString::Format("%g", left);
					}break;
					case -1: {
						variant = mActNotExecuted;
					}break;
					default: break;
					}//switch (ret)
				}break;
				default: {
					int64_t aid = it->mAid;
					FavAPropInfo info = it->mAInfo;

					const auto& idxFAV = obj.GetFavAPropValue().get<0>();
					auto fit = idxFAV.find(boost::make_tuple(aid, info));
					if (idxFAV.end() != fit)
					{
						const auto str_value = (*fit)->mValue;
						if (str_value == "null")
							variant = mActNotExecuted;
						else
							variant = str_value;
					}
				}break;
				}//switch (it->mAInfo)
				return;
			}//if (idxCol.end() != it)

			const auto& idxOPropCol = mOPropColumns.get<1>();
			auto prop_it = idxOPropCol.find(col);
			if (idxOPropCol.end() != prop_it)
			{
				const auto favOPrpop = obj.GetFavOPropValue();
				auto val_it = favOPrpop.find(prop_it->mPid);
				if (favOPrpop.cend() != val_it)
					variant = (*val_it)->mValue;
			}
		}break;
		}//switch (col)
	}

	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
		const auto node = static_cast<const IIdent64*> (dvitem.GetID());
		const auto ident = node;
		
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
		{
			GetClsValue(variant, col, *cls);
			return;
		}
		const auto obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			GetObjValue(variant, col, *obj);
			return;
		}
		GetErrorValue(variant, col);
	}
	virtual bool GetAttr(const wxDataViewItem &item, unsigned int col,
		wxDataViewItemAttr &attr) const override
	{
		
		if (!item.IsOk())
			return false;

		const auto node = static_cast<const IIdent64*> (item.GetID());
		const auto obj = dynamic_cast<const IObj64*>(node);
		if(!obj)
			return false;



		switch (col)
		{
		case 0:/*case 1:*/case 2:/*case 3:*/{
			if (0 == col || (2 == col && ClsKind::Single != obj->GetCls()->GetKind()))
			{
				attr.SetBold(true);
				return true;
			}
		}break;
		default:{
			const auto& idxCol = mActColumns.get<1>();
			const auto it = idxCol.find(col);
			if (idxCol.end() != it)
			{
				switch (it->mAInfo){
				default: break;
				case FavAPropInfo::NextDate: { //next
					wxDateTime next;
					int ret = obj->GetActNext(it->mAid, next);
					switch (ret)
					{
					case 1: {
						if (next.IsEarlierThan(wxDateTime::Now() + wxTimeSpan(240, 0, 0, 0)))
						{
							attr.SetBold(true);
							if (next.IsEarlierThan(wxDateTime::Now()))
								attr.SetColour(*wxRED);
							else
								attr.SetColour(wxColour(230, 130, 30));
							return true;
						}
					}break;
					case -1: {
						attr.SetBold(true);
						attr.SetColour(*wxRED);
						return true;
					}break;
					default: break;
					}//switch (ret)
				}break;
				case FavAPropInfo::LeftDay: {
					double left;
					int ret = obj->GetActLeft(it->mAid, left);
					switch (ret)
					{
					case 1: {
						if (left < 10)
						{
							attr.SetBold(true);
							if (left<0)
							{
								//attr.SetBackgroundColour(wxColour(255, 200, 200));
								attr.SetColour(*wxRED);
							}
							else
							{
								//attr.SetBackgroundColour(*wxYELLOW);
								attr.SetColour(wxColour(230, 130, 30));
							}//else if (left<0)
							return true;
						}
					}break;
					case -1: {
						attr.SetBold(true);
						attr.SetColour(*wxRED);
						return true;
					}break;
					default: break;
					}//switch (ret)

				}break;
				}//switch (it->mAInfo)
			}//if (idxCol.end() != it)
		}break;
		}//switch (col)

		return false;
	}
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col)override
	{
		return false;
	}
	
	virtual int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2
		, unsigned int column, bool ascending) const override
	{
		wxVariant value1, value2;
		GetValue(value1, item1, column);
		GetValue(value2, item2, column);

		if (mCurrentRoot)
		{
			if (static_cast<const IIdent64*> (item1.GetID()) == mCurrentRoot)
				return -1;
			else if (static_cast<const IIdent64*> (item2.GetID()) == mCurrentRoot)
				return 1;
		}

		if (!ascending)
			std::swap(value1, value2);

		if (value1.IsNull())
		{
			if (!value2.IsNull())
				return 1;
		}
		else
			if (value2.IsNull())
				return -1;

		// all columns sorted by TRY FIRST numberic value
		if (value1.GetType() == "string" || value1.GetType() == "wxDataViewIconText")
		{
			wxString str1;
			wxString str2;

			if (value1.GetType() == "wxDataViewIconText")
			{
				wxDataViewIconText iconText1, iconText2;

				iconText1 << value1;
				iconText2 << value2;
				str1 = iconText1.GetText();
				str2 = iconText2.GetText();
			}
			else
			{
				str1 = value1.GetString();
				str2 = value2.GetString();
			}
			//wxRegEx mStartNum = "(^[0-9]+)";
			if (mStartNum.Matches(str1))
			{
				size_t start1;
				size_t len1;
				double num1;
				bool match = mStartNum.GetMatch(&start1, &len1);
				if (match && str1.substr(start1, len1).ToDouble(&num1))
				{
					if (mStartNum.Matches(str2))
					{
						size_t start2;
						size_t len2;
						double num2;
						match = mStartNum.GetMatch(&start2, &len2);
						if (match && str2.substr(start2, len2).ToDouble(&num2))
						{
							if (num1 < num2)
								return -1;
							else if (num1 > num2)
								return 1;
							str1.erase(start1, start1 + len1);
							str2.erase(start2, start2 + len2);
						}
						else
							return 1;
					}//if (mStartNum.Matches(str2))
				}//if (match && str1.substr(start, len).ToLong(&num1))
				else
				{
					if (mStartNum.Matches(str2))
					{
						size_t start2;
						size_t len2;
						double num2;
						match = mStartNum.GetMatch(&start2, &len2);
						if (match && str2.substr(start2, len2).ToDouble(&num2))
							return -1;
					}//if (mStartNum.Matches(str2))
				}//else if (match && str1.substr(start, len).ToLong(&num1))
			}//if (mStartNum.Matches(str1))

			int res = str1.CmpNoCase(str2);
			if (res)
				return res;

			// items must be different
			wxUIntPtr id1 = wxPtrToUInt(item1.GetID()),
				id2 = wxPtrToUInt(item2.GetID());
			return ascending ? id1 - id2 : id2 - id1;
		}//if (value1.GetType() == "string" || value1.GetType() == "wxDataViewIconText")
		
		return wxDataViewModel::Compare(item1, item2, column, ascending);
	}

	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override
	{
		if (!item.IsOk())
			return wxDataViewItem(nullptr);

		const auto ident = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
			return wxDataViewItem(nullptr);

		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
			return wxDataViewItem((void*)obj->GetCls().get());

		return wxDataViewItem(nullptr);
	}

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &arr) const override
	{
		if (!parent.IsOk() )
		{
			if (mCurrentRoot && 1 < mCurrentRoot->GetId())
			{
				wxDataViewItem dvitem((void*)mCurrentRoot);
				arr.push_back(dvitem);
			}
			for (const auto& child_cls : mClsList)
			{
				wxDataViewItem dvitem((void*)child_cls);
				arr.push_back(dvitem);
			}
			return arr.size();
		}
		const auto ident = static_cast<const IIdent64*> (parent.GetID());
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (mGroupByType && cls)
		{
			const auto obj_list = cls->GetObjTable();
			if (obj_list && obj_list->size())
			{
				for (const auto& sp_obj : *obj_list)
				{
					wxDataViewItem dvitem((void*)sp_obj.get());
					arr.push_back(dvitem);
				}
				return arr.size();
			}
		}


		return 0;
	}

	const IIdent64* GetCurrentRoot()const
	{
		return mCurrentRoot;
	}
	virtual bool  IsListModel() const override
	{
		return !mGroupByType;
	}

	void SetClsList(const std::vector<const IIdent64*>& current, const IIdent64* curr, bool group_by_type)
	{
		mGroupByType = group_by_type;
		mCurrentRoot = curr;
		mClsList = current;
		Cleared();
	}



	const std::vector<const IIdent64*>& GetClsList()const
	{
		return mClsList;
	}


};
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
//-----------------------------------------------------------------------------
ViewTableBrowser::ViewTableBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewTableBrowser(parent->GetWnd())
{}
//-----------------------------------------------------------------------------
ViewTableBrowser::ViewTableBrowser(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES 
		 | wxDV_VERT_RULES 
		 //| wxDV_HORIZ_RULES
		 //| wxDV_MULTIPLE
		);
	mTable = table;

	auto dv_model = new wxDVTableBrowser();
	table->AssociateModel(dv_model);
	dv_model->DecRef();
	
	#define ICON_HEIGHT 24+2
	int row_height = table->GetCharHeight() + 2;// + 1px in bottom and top 
	if(ICON_HEIGHT > row_height )
		row_height = ICON_HEIGHT;
	table->SetRowHeight(row_height);
	ResetColumns();

	table->GetTargetWindow()->SetToolTip("ToolTip");
	table->GetTargetWindow()->Bind(wxEVT_MOTION, &ViewTableBrowser::OnCmd_MouseMove, this);

	table->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, [this](wxDataViewEvent& evt)
		{ StoreSelect(); evt.Skip(); });
	table->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, [this](wxDataViewEvent& evt) 
		{ RestoreSelect(); } );

	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewTableBrowser::OnCmd_Activate, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING
		, &ViewTableBrowser::OnCmd_Expanding, this);
	table->Bind(wxEVT_DATAVIEW_ITEM_EXPANDED
		, &ViewTableBrowser::OnCmd_Expanded, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_COLLAPSED
		, &ViewTableBrowser::OnCmd_Collapseded, this);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigRefresh(); }, wxID_REFRESH);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigUp(); }, wxID_UP);
	//table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigAct(); }, wxID_EXECUTE);
	//table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigMove(); }, wxID_REPLACE);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {SetShowDetail(); }, wxID_VIEW_DETAILS);
	table->GetTargetWindow()->Bind(wxEVT_MIDDLE_UP,	[this](wxMouseEvent&) {SetShowDetail(); });
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {SetInsertType(); }, wxID_NEW_TYPE);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {SetInsertObj(); }, wxID_NEW_OBJECT);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {SetDeleteSelected(); }, wxID_DELETE);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {SetUpdateSelected(); }, wxID_EDIT);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) 
		{ sigToggleGroupByType(); }, wxID_VIEW_LIST);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ SetShowFav(); }, wxID_PROPERTIES);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ sigShowSettings(); }, wxID_HELP_INDEX);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ sigShowHelp("ViewBrowserPage"); }, wxID_HELP_INDEX);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ sigClosePage(); }, wxID_CLOSE);

	wxAcceleratorEntry entries[16];
	char i = 0;
	entries[i++].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F5,   wxID_REFRESH);
	entries[i++].Set(wxACCEL_NORMAL, WXK_BACK, wxID_UP);
	
	entries[i++].Set(wxACCEL_NORMAL, WXK_F6, wxID_REPLACE);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F7, wxID_EXECUTE);
	entries[i++].Set(wxACCEL_CTRL, WXK_RETURN, wxID_VIEW_DETAILS);

	entries[i++].Set(wxACCEL_CTRL, (int) 'T', wxID_NEW_TYPE);
	entries[i++].Set(wxACCEL_CTRL, (int) 'O', wxID_NEW_OBJECT);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F8, wxID_DELETE);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F4, wxID_EDIT);

	entries[i++].Set(wxACCEL_CTRL, (int) 'G', wxID_VIEW_LIST);
	entries[i++].Set(wxACCEL_CTRL, (int) 'P', wxID_PROPERTIES);
	entries[i++].Set(wxACCEL_CTRL, (int) 'N', wxID_SETUP);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F1, wxID_HELP_INDEX);

	entries[i++].Set(wxACCEL_CTRL, (int) 'W', wxID_CLOSE);

	wxAcceleratorTable accel(16, entries);
	table->SetAcceleratorTable(accel);

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_MouseMove(wxMouseEvent& evt)
{
	auto pos = evt.GetPosition();
	pos = mTable->ScreenToClient((mTable->GetMainWindow()->ClientToScreen(pos)));

	wxDataViewItem item(nullptr);
	wxDataViewColumn* col = nullptr;
	
	mTable->HitTest(pos, item, col);
	if (!col || !item.IsOk())
		return;
	
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;
	wxString val;
	wxVariant var;
	dvmodel->GetValue(var, item, col->GetModelColumn());
	if (0 == col)
	{
		wxDataViewIconText d;
		d << var;
		val = d.GetText();
	}
	else
		val = var.GetString();

	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return;

	wxString str;
	const auto cls = dynamic_cast<const ICls64*>(ident);
	if (cls)
	{
		str= wxString::Format("%s\n\n%s \n#[%s]"
			, val
			, cls->GetTitle()
			, cls->GetIdAsString());
	}
	const auto obj = dynamic_cast<const IObj64*>(ident);
	if (obj)
	{
		auto cls = obj->GetCls();
		str=wxString::Format("%s\n\n[%s]\t%s \n#[%s]\t%s)"
			, val
			, cls->GetTitle(), obj->GetTitle()
			, cls->GetIdAsString(), obj->GetIdAsString() );
	}
	
	mTable->GetTargetWindow()->GetToolTip()->SetTip(str);
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Activate(wxDataViewEvent& evt)
{
	TEST_FUNC_TIME;

	auto item = evt.GetItem();

	if (mTable->GetModel()->IsContainer(item))
	{
		if (mTable->IsExpanded(item))
			mTable->Collapse(item);
		else
			mTable->Expand(item);
	}
	else
	{
		const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
		const ICls64* cls = dynamic_cast<const ICls64*> (node);
		if (cls)
		{
			wxBusyCursor busyCursor;
			
			if(cls->GetId() == mParentCid)
				sigUp();
			else
			{
				// если убрать этот локер, выделится первый элемент,
				// даже в обратной сортировке
				wxWindowUpdateLocker lock(mTable);
				sigActivate(cls->GetId());
			}
				
		}
			
	}

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Expanding(wxDataViewEvent& evt)
{
	TEST_FUNC_TIME;

	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel || mTable->GetModel()->IsListModel())
		return;

	auto item = evt.GetItem();

	const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
	const ICls64* cls = dynamic_cast<const ICls64*> (node);
	if (cls)
	{
		mSortCol = mTable->GetSortingColumn();
		if (mSortCol)
		{
			mSortAsc = mSortCol->IsSortOrderAscending();
			mSortCol->UnsetAsSortKey();
		}

		wxBusyCursor busyCursor;
		wxWindowUpdateLocker lock(mTable);

		//sigActivate(cls->GetId());
		sigRefreshClsObjects(cls->GetId());
		const std::shared_ptr<const ICls64::ObjTable> ot = cls->GetObjTable();
		if (ot && ot->size())
		{
			std::vector<const IIdent64*> vec;
			for (const auto& obj : *ot)
				vec.emplace_back(obj.get());
			RebuildClsColumns(vec);
		}

	}

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Expanded(wxDataViewEvent& evt)
{
	TEST_FUNC_TIME;
	if (mSortCol)
	{
	
		mSortCol->SetSortable(true);
		mSortCol->SetSortOrder(mSortAsc);

		wxBusyCursor busyCursor;
		wxWindowUpdateLocker lock(mTable);

		mTable->GetModel()->Resort();
	}

	AutosizeColumns();

	auto item = evt.GetItem();
	const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
	const ICls64* cls = dynamic_cast<const ICls64*> (node);
	if (cls)
		mExpandedCls.insert(cls->GetId());
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Collapseded(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk())
	{
		auto model = mTable->GetModel();
		auto dvparent = evt.GetItem();
		wxDataViewItemArray arr;
		model->GetChildren(dvparent, arr);
		model->ItemsDeleted(dvparent, arr);

		const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
		const ICls64* cls = dynamic_cast<const ICls64*> (node);
		if (cls)
			mExpandedCls.erase(cls->GetId());
	}

}

//-----------------------------------------------------------------------------
const IIdent64* ViewTableBrowser::FindChildCls(const int64_t& id)const
{
	//wxDataViewItem dvparent(nullptr);
	//auto model = mTable->GetModel();
	//wxDataViewItemArray arr;
	//model->GetChildren(dvparent, arr);

	//for (size_t i = 0; i < arr.size() ; i++)
	//{
	//	const auto ident = static_cast<const IIdent64*> (arr[i].GetID());
	//	const auto cls = dynamic_cast<const ICls64*>(ident);
	//	if (cls && cls->GetId() == id)
	//		return cls;
	//}
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return nullptr;

	const auto& cls_list = dvmodel->GetClsList();


	if (cls_list.empty())
		return nullptr;
	auto it = std::find_if(cls_list.cbegin(), cls_list.cend()
		, [&id](const IIdent64* it)
		{ 
			return it->GetId() == id; 
		});

	if (cls_list.cend() != it)
		return (*it);
	
	return nullptr;
}
//-----------------------------------------------------------------------------
const IIdent64* ViewTableBrowser::GetTopChildCls()const
{
	//wxDataViewItem dvparent(nullptr);
	//auto model = mTable->GetModel();
	//wxDataViewItemArray arr;
	//model->GetChildren(dvparent, arr);

	//const auto ident = static_cast<const IIdent64*> (arr[0].GetID());
	//const auto cls = dynamic_cast<const ICls64*>(ident);
	//if (cls)
	//	return cls;
	//return nullptr;

	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return nullptr;


	if (1 < mParentCid)	//(mParent && 1 != mParent->GetId())
		return dvmodel->GetCurrentRoot();

	if (!dvmodel->GetClsList().empty())
		return dvmodel->GetClsList().front();
	return nullptr;
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::StoreSelect()
{
	mClsSelected = 0;
	mObjSelected = 0;

	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (ident)
	{
		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			mObjSelected = obj->GetId();
			mClsSelected = obj->GetCls()->GetId();
		}
		else
		{
			const auto& cls = dynamic_cast<const ICls64*>(ident);
			if (cls)
				mClsSelected = cls->GetId();
		}
	}//if (ident)

}

//-----------------------------------------------------------------------------
void ViewTableBrowser::RestoreSelect()
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;
	
	wxDataViewItem dvitem;

	if (dvmodel->IsListModel())
	{
		const IIdent64* ident = static_cast<const IIdent64*> (dvmodel->GetCurrentRoot());
		const auto& cls = dynamic_cast<const ICls64*>(ident);
		if (cls && ClsKind::Abstract == cls->GetKind())
		{
			if (mClsSelected)
			{
				const auto finded = FindChildCls(mClsSelected);
				if (finded)
					dvitem = wxDataViewItem((void*)finded);
			}
		}
		else
		{
			if (mObjSelected)
			{
				const auto finded = FindChildCls(mObjSelected);
				if (finded)
					dvitem = wxDataViewItem((void*)finded);
			}

		}
	}
	else
	{
		if (mClsSelected)
		{
			const auto finded = FindChildCls(mClsSelected);
			if (finded)
			{
				//auto id = finded->GetIdAsString();
				//auto title = finded->GetTitle();
				dvitem = wxDataViewItem((void*)finded);

				if (!mTable->GetModel()->IsListModel() && mObjSelected)
				{
					//sigActivate(finded);
					mTable->Expand(dvitem);// finded cls
					const auto& cls = dynamic_cast<const ICls64*>(finded);
					const auto objTable = cls->GetObjTable();
					auto it = std::find_if(objTable->cbegin(), objTable->cend()
						, [this](const std::shared_ptr<const IObj64>& it)
					{
						return it->GetId() == mObjSelected;
					});

					if (objTable->cend() != it)
						dvitem = wxDataViewItem((void*)(it->get()));
				}
			}//if (finded)
		}
	}

	if (!dvitem.IsOk())
	{
		const auto finded = GetTopChildCls();
		if (finded)
			dvitem = wxDataViewItem((void*)finded);
	}

	auto model = mTable->GetModel();
	
	mTable->UnselectAll();

	if (dvitem.IsOk())
	{
		mTable->EnsureVisible(dvitem);
		mTable->SetCurrentItem(dvitem);
		mTable->Select(dvitem);
	}

	//auto new_sel = mTable->GetCurrentItem();
	//const auto ident = static_cast<const IIdent64*> (new_sel.GetID());
	//const auto cls = dynamic_cast<const ICls64*>(ident);
	//if (cls)
	//{
	//	auto title = cls->GetTitle();
	//	auto id = cls->GetIdAsString();
	//}
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::AutosizeColumns()
{
	if (!mColAutosize)
		return;

	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
	//for (size_t i = 0; i < mTable->GetColumnCount(); i++)
	for (size_t i = 0; i < 4; i++)
	{
		auto col_pos = mTable->GetModelColumnIndex(i);
		auto col = mTable->GetColumn(col_pos);
		if (col)
		{
			auto bs = mTable->GetBestColumnWidth(i);
			if (bs > 300)
				bs = 300;
			col->SetWidth(bs);
		}
	}
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::ResetColumns()
{
	wxWindowUpdateLocker lock(mTable);

	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	auto table = mTable;

	if (4 == table->GetColumnCount() )
		return;

	table->ClearColumns();
	dvmodel->mActColumns.clear();
	dvmodel->mCPropColumns.clear();
	dvmodel->mOPropColumns.clear();

	auto renderer1 = new wxDataViewIconTextRenderer();
	//auto attr = renderer1->GetAttr();
	//attr.SetColour(*wxBLACK);
	//renderer1->SetAttr(attr);

	//table->AppendTextColumn("#", 0,wxDATAVIEW_CELL_INERT,-1, wxALIGN_LEFT
	//	, wxDATAVIEW_COL_RESIZABLE /*| wxDATAVIEW_COL_HIDDEN*/);

	auto col1 = new wxDataViewColumn("Имя"
		, renderer1, 0, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	table->AppendColumn(col1);
	//col1->SetSortOrder(true);

	auto col3 = table->AppendTextColumn("Тип",1, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	//col3->GetRenderer()->EnableEllipsize(wxELLIPSIZE_START);

	auto col2 = table->AppendTextColumn("Количество", 2, wxDATAVIEW_CELL_INERT, 150
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

	auto col4 = table->AppendTextColumn("Местоположение", 3, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	col4->GetRenderer()->EnableEllipsize(wxELLIPSIZE_START);
	
	table->SetExpanderColumn(col1);
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::RebuildClsColumns(const std::vector<const IIdent64*>& vec)
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	const auto col_qty = mTable->GetColumnCount();
	auto last_col = mTable->GetColumnAt(col_qty - 1);
	
	for (const auto& ident : vec)
	{
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
		{
			const auto& fav_cprop = cls->GetFavCPropValue();
			for (const auto& cprop : fav_cprop)
				AppendPropColumn(dvmodel->mCPropColumns, cprop);

			const auto& fav_act = cls->GetFavAProp();
			for (const auto& aprop : fav_act)
				AppendActColumn(aprop);


		}
		else
		{
			const auto obj = dynamic_cast<const IObj64*>(ident);
			if (obj)
			{
				const auto cls = obj->GetCls();

				const auto& fav_cprop = cls->GetFavCPropValue();
				for (const auto& cprop : fav_cprop)
					AppendPropColumn(dvmodel->mCPropColumns, cprop);

				const auto& fav_act = cls->GetFavAProp();
				for (const auto& aprop : fav_act)
					AppendActColumn(aprop);

				const auto& fav_prop = obj->GetFavOPropValue();
				for (const auto& oprop : fav_prop)
					AppendPropColumn(dvmodel->mOPropColumns, oprop);

				


			}//if (obj)
		}

	}//for (const auto& obj : *ot)

	if(col_qty != mTable->GetColumnCount())
		last_col->SetWidth(GetTitleWidth(last_col->GetTitle()));
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::AppendActColumn(const std::shared_ptr<const FavAProp>& aprop)
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	auto aid = aprop->mAct->GetId();
	FavAPropInfo info = aprop->mInfo;

	auto& idx0 = dvmodel->mActColumns.get<0>();
	auto it = idx0.find(boost::make_tuple(aid, info));
	if (idx0.end() == it)
	{
		const auto column_idx = mTable->GetColumnCount();
		const wxString title = wxString::Format("%s %s"
			, aprop->mAct->GetTitle()
			, FavAPropInfo2Text(info));

		auto col = AppendTableColumn(title, column_idx);

		const auto& ico = GetIcon(info);
		col->SetBitmap(ico);
		
		ActColumn acol(aid, info, column_idx);
		dvmodel->mActColumns.emplace(acol);
	}
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::AppendPropColumn(PropColumns& prop_column
	,const std::shared_ptr<const PropVal>& prop_val)
{
	const int64_t pid = prop_val->mProp->GetId();

	auto& pcolIdx = prop_column;
	auto it = pcolIdx.find(pid);
	if (pcolIdx.end() == it)
	{
		const auto column_idx = mTable->GetColumnCount();
		const wxString& title = prop_val->mProp->GetTitle().empty() ?
			prop_val->mProp->GetIdAsString()
			: prop_val->mProp->GetTitle();
		
		AppendTableColumn(title, column_idx);

		PropColumn prop_col(pid, column_idx);
		pcolIdx.emplace(prop_col);
	}

}
//-----------------------------------------------------------------------------
wxDataViewColumn* ViewTableBrowser::AppendTableColumn(const wxString& title, int model_id)
{
	auto col = mTable->AppendTextColumn(title, model_id
		, wxDATAVIEW_CELL_INERT
		, GetTitleWidth(title)
		, wxALIGN_NOT
		, wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	
	col->SetMinWidth(80);

	return col;
}
//-----------------------------------------------------------------------------
int ViewTableBrowser::GetTitleWidth(const wxString& title)const
{
	const int spw = mTable->GetTextExtent(" ").GetWidth();
	int hw = mTable->GetTextExtent(title).GetWidth()+ spw*4 + 24;
	if (hw < 80)
		hw = -1; // default width
	else if (hw > 300)
		hw = 300;

	return hw;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetBeforeRefreshCls(const std::vector<const IIdent64*>& vec
											, const IIdent64* parent
											, const wxString&
											, bool group_by_type
											) //override;
{
	TEST_FUNC_TIME;
	StoreSelect();

	/*
	mClsSelected = 0;
	mObjSelected = 0;
	if (parent && mParentCid)
	{
		auto curr_id = mParentCid;
		auto  new_id = parent->GetId();
		if(curr_id== new_id)
			StoreSelect();
		else
			mClsSelected = curr_id;
	}
	else if(parent)
	{
		mClsSelected = parent->GetId();
	}
	*/
		
	ResetColumns();
	mParentCid = 0;
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (dvmodel)
	{
		dvmodel->SetClsList(vec, parent, group_by_type);
	}
	
	
	//wxBusyCursor busyCursor;
	//wxWindowUpdateLocker lock(mTable);

	//OnCmd_Select(wxDataViewEvent());
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetAfterRefreshCls(const std::vector<const IIdent64*>& vec
											, const IIdent64* root
											, const wxString& ss
											, bool group_by_type
											) //override;
{
	TEST_FUNC_TIME;
	{
		wxBusyCursor busyCursor;
		wxWindowUpdateLocker lock(mTable);

		auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
		if (!dvmodel)
			return;

		auto col3 = mTable->GetColumn(1);
		if (root)
		{
			// and CLASS catalog 
			col3->SetHidden(true);
		}
		else
		{
			col3->SetHidden(!mTable->GetModel()->IsListModel());
		}


		mParentCid = root ? root->GetId() : 0;
		dvmodel->SetClsList(vec, (root && 1 != root->GetId()) ? root : nullptr, group_by_type);

		if (!dvmodel->IsListModel())
		{
			bool tmp_autosize = mColAutosize;
			mColAutosize = false;
			for (const auto& cid : mExpandedCls)
			{
				auto ident = FindChildCls(cid);
				if (ident)
				{
					wxDataViewItem item((void*)ident);
					mTable->Expand(item);
				}
			}//for (const auto& cid : mExpandedCls)
			mColAutosize = tmp_autosize;
		}
		//else
		{
			RebuildClsColumns(vec);
		}
		AutosizeColumns();
	}
	RestoreSelect();
	

}

//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetObjOperation(Operation op, const std::vector<const IIdent64*>& obj_list)// override;
{
	TEST_FUNC_TIME;


	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel || mTable->GetModel()->IsListModel())
		return;

	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);

	switch (op)
	{
	case Operation::BeforeInsert:	break;
	case Operation::AfterInsert:	
		// тормозит при добавлении большого количества элементов
		//for (const auto& item : obj_list)
		//{
		//	wxDataViewItem cls((void*)item->GetCls().get());
		//	wxDataViewItem obj((void*)item);
		//	dvmodel->ItemAdded(cls, obj);
		//}
		RebuildClsColumns(obj_list);
		break;
	case Operation::BeforeUpdate:	break;
	case Operation::AfterUpdate:	
		for (const auto& item : obj_list)
		{
			//wxDataViewItem cls((void*)item->GetCls().get());
			wxDataViewItem obj((void*)item);
			dvmodel->ItemChanged(obj);
		}
		break;
	case Operation::BeforeDelete:	
		for (const auto& item : obj_list)
		{
			const auto obj = dynamic_cast<const IObj64*>(item);
			wxDataViewItem item_cls((void*)obj->GetCls().get());
			wxDataViewItem item_obj((void*)item);
			dvmodel->ItemDeleted(item_cls, item_obj);
		}
		break;
	case Operation::AfterDelete:	break;
	default:	break;
	}

	//AutosizeColumns();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetAct()//override;
{
	auto item = mTable->GetCurrentItem();
	if (!item.IsOk())
		return;
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return;
	const auto& obj = dynamic_cast<const IObj64*>(ident);
	if (!obj)
		return;
	sigAct(obj->GetId(), obj->GetParentId());
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetMove()//override;
{
	auto item = mTable->GetCurrentItem();
	if (!item.IsOk())
		return;
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return;
	const auto& obj = dynamic_cast<const IObj64*>(ident);
	if (!obj)
		return;
	sigMove(obj->GetId(), obj->GetParentId() );
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetShowDetail()//override;
{
	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (ident)
	{
		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			sigShowDetail(obj->GetId(), obj->GetParentId());
		}
	}//if (ident)
}
//-----------------------------------------------------------------------------
void wh::ViewTableBrowser::SetShowFav()
{
	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (ident)
	{
		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
			sigShowFav(obj->GetClsId() );
		else
		{
			const auto& cls = dynamic_cast<const ICls64*>(ident);
			if (cls)
				sigShowFav(cls->GetId());
		}
	}//if (ident)
}
//-----------------------------------------------------------------------------
//virtual 
void wh::ViewTableBrowser::SetInsertType() const //override;
{
	int64_t parent_cid = 1;
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (dvmodel)
	{
		auto rootIdent = dvmodel->GetCurrentRoot();
		if (rootIdent)
		{
			const auto* cls = dynamic_cast<const ICls64*>(rootIdent);
			if(cls)
				parent_cid = cls->GetId();
		}
	}
	sigClsInsert(parent_cid);
}
//-----------------------------------------------------------------------------
//virtual 
void wh::ViewTableBrowser::SetInsertObj() const //override;
{
	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (!ident)
		return;
	const auto& cls = dynamic_cast<const ICls64*>(ident);
	if (cls)
	{
		int64_t cid = cls->GetId();
		sigObjInsert(cid);
	}
	const auto& obj = dynamic_cast<const IObj64*>(ident);
	if (obj)
	{
		int64_t cid = obj->GetCls()->GetId();
		sigObjInsert(cid);
	}
}
//-----------------------------------------------------------------------------
//virtual 
void wh::ViewTableBrowser::SetDeleteSelected() const //override;
{
	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (!ident)
		return;
	const auto& cls = dynamic_cast<const ICls64*>(ident);
	if (cls)
	{
		int64_t cid = cls->GetId();
		sigClsDelete(cid);
	}
	const auto& obj = dynamic_cast<const IObj64*>(ident);
	if (obj)
	{
		int64_t oid = obj->GetId();
		int64_t parent_oid = obj->GetParentId();
		sigObjDelete(oid, parent_oid);
	}


}
//-----------------------------------------------------------------------------
//virtual 
void wh::ViewTableBrowser::SetUpdateSelected() const //override;
{
	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (!ident)
		return;
	const auto& cls = dynamic_cast<const ICls64*>(ident);
	if (cls)
	{
		int64_t cid = cls->GetId();
		sigClsUpdate(cid);
	}
	else
	{
		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			int64_t oid = obj->GetId();
			int64_t parent_oid = obj->GetParentId();
			sigObjUpdate(oid, parent_oid);
		}
	}
	
}










//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewToolbarBrowser::ViewToolbarBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewToolbarBrowser(parent->GetWnd())
{}
//-----------------------------------------------------------------------------
ViewToolbarBrowser::ViewToolbarBrowser(wxWindow* parent)
{
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();

	auto mgr = ResMgr::GetInstance();

	long style = wxAUI_TB_DEFAULT_STYLE 
		| wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить (CTRL+R или CTRL+F5)");
	//tool_bar->AddTool(wxID_UP,"Вверх", mgr->m_ico_back24, "Вверх(BACKSPACE)");
	tool_bar->AddTool(wxID_REPLACE, "Переместить", mgr->m_ico_move24, "Переместить (F6)");
	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24, "Выполнить (F7)");
	tool_bar->AddTool(wxID_VIEW_DETAILS, "Подробно", mgr->m_ico_views24, "Подробно (CTRL+ENTER)");
	
	if ((int)currBaseGroup > (int)bgUser)
	{
		tool_bar->AddSeparator();

		auto add_tool = tool_bar->AddTool(wxID_ADD, "Создать", mgr->m_ico_plus24
			, "Создать тип или обьект (CTRL+INSERT)");
		add_tool->SetHasDropDown(true);

		tool_bar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN
			, [this, mgr, &currBaseGroup](wxCommandEvent& evt)
			{
				wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());
				tb->SetToolSticky(evt.GetId(), true);
				wxMenu add_menu;
				if ((int)currBaseGroup >= (int)bgTypeDesigner)
					AppendBitmapMenu(&add_menu, wxID_NEW_TYPE, "Создать тип (CTRL+T)", mgr->m_ico_add_type24);
				if ((int)currBaseGroup >= (int)bgObjDesigner)
					AppendBitmapMenu(&add_menu, wxID_NEW_OBJECT, "Создать объект (CTRL+O)", mgr->m_ico_add_obj24);
				wxRect rect = tb->GetToolRect(evt.GetId());
				wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
				pt = tb->ScreenToClient(pt);
				tb->PopupMenu(&add_menu, pt);
				tb->SetToolSticky(evt.GetId(), false);
				tb->Refresh();

			}
			, wxID_ADD);

		tool_bar->AddTool(wxID_DELETE, "Удалить", mgr->m_ico_delete24, "Удалить (F8)");
		tool_bar->AddTool(wxID_EDIT, "Редактировать", mgr->m_ico_edit24, "Редактировать (F4)");
	}
	tool_bar->AddSeparator();
	tool_bar->AddTool(wxID_VIEW_LIST, "Группировать", mgr->m_ico_group_by_type24, "Группировать по типу (CTRL+G)");
	tool_bar->AddTool(wxID_PROPERTIES, "Свойства", mgr->m_ico_favprop_select24, "Выбрать свойства (CTRL+P)");
	//tool_bar->AddTool(wxID_SETUP, "Настройки", mgr->m_ico_options24
	//		, "Настройки внешнего вида таблийы истории (CTRL+N)");
	tool_bar->AddTool(wxID_HELP_INDEX, "Справка", wxArtProvider::GetBitmap(wxART_HELP, wxART_TOOLBAR), "Справка (F1)");

	//tool_bar->AddSeparator();


	//auto mFindCtrl = new wxComboBtn(tool_bar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	//wxBitmap bmp(wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU));
	//mFindCtrl->SetButtonBitmaps(bmp, true);

	////mFindCtrl->Bind(wxEVT_COMMAND_TEXT_ENTER, fn);
	//tool_bar->AddControl(mFindCtrl, "Поиск");


	tool_bar->Realize();

	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigRefresh();}, wxID_REFRESH);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigUp();}, wxID_UP);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigAct();}, wxID_EXECUTE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigMove();}, wxID_REPLACE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigShowDetail();}, wxID_VIEW_DETAILS);
	
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigInsertType(); }, wxID_NEW_TYPE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigInsertObject(); }, wxID_NEW_OBJECT);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigDelete(); }, wxID_DELETE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigUpdate(); }, wxID_EDIT);

	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED
		, &ViewToolbarBrowser::OnCmd_GroupByType, this, wxID_VIEW_LIST);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{sigShowFav(); }, wxID_PROPERTIES);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ sigShowSettings(); }, wxID_SETUP);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{sigShowHelp("PageBrowserByType"); }, wxID_HELP_INDEX);

	mToolbar = tool_bar;

}
//-----------------------------------------------------------------------------
void ViewToolbarBrowser::OnCmd_GroupByType(wxCommandEvent& evt)
{
	int state = mToolbar->FindTool(wxID_VIEW_LIST)->GetState();
	int enable = state & wxAUI_BUTTON_STATE_CHECKED;
	sigGroupByType(enable ? false : true);

}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::SetVisibleFilters(bool enable)// override;
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::SetAfterRefreshCls(const std::vector<const IIdent64*>& vec
											, const IIdent64* root
											, const wxString& ss
											, bool group_by_type
										) //override;
{
	int show;
	show = group_by_type ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL;
	mToolbar->FindTool(wxID_VIEW_LIST)->SetState(show);
	mToolbar->Refresh();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewPathBrowser::ViewPathBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewPathBrowser(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------
ViewPathBrowser::ViewPathBrowser(wxWindow* parent)
{
	mPathCtrl = new wxTextCtrl(parent, wxID_ANY, wxEmptyString
		, wxDefaultPosition, wxDefaultSize, 0 | wxTE_READONLY /*| wxNO_BORDER*/);

	//auto window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	//mPathCtrl->SetBackgroundColour(window_colour);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPathBrowser::SetPathMode(const int mode)// override;
{
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPathBrowser::SetPathString(const ICls64& node)// override;
{
	wxString ret = "/";
	const auto* curr = &node;
	while (curr != nullptr && curr->GetId()!=1)
	{
		const auto& title = curr->GetTitle();

		if (wxNOT_FOUND == title.Find('/'))
			ret = wxString::Format("/%s%s", title, ret);
		else
			ret = wxString::Format("/[%s]%s", title, ret);

		curr = curr->GetParent().get();
	}

	mPathCtrl->SetValue(ret);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewBrowserPage::ViewBrowserPage(const std::shared_ptr<IViewWindow>& parent)
	:ViewBrowserPage(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------
ViewBrowserPage::ViewBrowserPage(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	auto panel = new wxAuiPanel(parent);
	mAuiMgr = &panel->mAuiMgr;
	//mAuiMgr->SetFlags(wxAUI_MGR_LIVE_RESIZE);
	mPanel = panel;

	auto window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, face_colour);
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BORDER_COLOUR, face_colour);
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, *wxRED);
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, *wxGREEN);
	//panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BORDER_COLOUR, *wxBLUE);
	//panel->mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE,2);
	panel->mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	// toolbar
	mViewToolbarBrowser = std::make_shared<ViewToolbarBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("BrowserToolBar"))
		.CaptionVisible(false).PaneBorder(false)
		.Top().Dock().Floatable(false).ToolbarPane().Gripper(false) );
	// path panel
	auto path_panel = new wxAuiPanel(panel);
	path_panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	path_panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, face_colour);
	// path
	mViewPathBrowser = std::make_shared<ViewPathBrowser>(path_panel);
	const auto path_bs = mViewPathBrowser->GetWnd()->GetBestSize();
	wxAuiPaneInfo path_pi;
	path_pi.Name(wxT("PathBrowser")).Top().CaptionVisible(false).Dock().PaneBorder(false)
		.Resizable().MinSize(path_bs).MaxSize(-1,path_bs.GetHeight());
	path_pi.dock_proportion = 7;
	path_panel->mAuiMgr.AddPane(mViewPathBrowser->GetWnd(), path_pi);
	// search
	mCtrlFind = new wxSearchCtrl(path_panel, wxID_ANY);
	mCtrlFind->ShowSearchButton(true);
	mCtrlFind->ShowCancelButton(true);
	mCtrlFind->SetDescriptiveText("Быстрый поиск");
	const auto search_bs = mCtrlFind->GetBestSize();
	wxAuiPaneInfo search_pi;
	search_pi.Name(wxT("SearchCtrl")).Top().CaptionVisible(false).Dock().PaneBorder(false)
		.MinSize(search_bs).MaxSize(-1, search_bs.GetHeight());
	search_pi.dock_proportion = 3;
	path_panel->mAuiMgr.AddPane(mCtrlFind, search_pi);
	// path_panel---> panel
	panel->mAuiMgr.AddPane(path_panel,
		wxAuiPaneInfo().Name("Splitter").Top().Row(1).CaptionVisible(false).Dock().PaneBorder(false)
		.Fixed().MinSize(-1, search_bs.GetHeight()).MaxSize(-1, search_bs.GetHeight()));
	path_panel->mAuiMgr.Update();
	// table
	mViewTableBrowser = std::make_shared<ViewTableBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewTableBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("TableBrowser"))
		.Center().CaptionVisible(false).Dock().CentrePane().PaneBorder(false) );
	//mViewTableBrowser->GetWnd()->SetFocus();
	panel->mAuiMgr.Update();

	mCtrlFind->Bind(wxEVT_SEARCH, &ViewBrowserPage::OnCmd_Find, this);
	mCtrlFind->Bind(wxEVT_SEARCH_CANCEL
		, [this](wxCommandEvent& event) 
		{
			sigFind(wxEmptyString);
		});
}
//-----------------------------------------------------------------------------
void ViewBrowserPage::OnCmd_Find(wxCommandEvent& evt)
{
	auto obj = evt.GetEventObject();
	wxString ss;
	wxTextCtrl* txtCtrl = wxDynamicCast(obj, wxTextCtrl);
	if (txtCtrl)
		ss = txtCtrl->GetValue();
	else
	{
		wxSearchCtrl* txtCtrl = wxDynamicCast(obj, wxSearchCtrl);
		if (txtCtrl)
			ss = txtCtrl->GetValue();

	}
		
	if(!ss.empty())
		sigFind(ss);
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewToolbarBrowser>	
ViewBrowserPage::GetViewToolbarBrowser()const// override;
{
	return mViewToolbarBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewPathBrowser>
ViewBrowserPage::GetViewPathBrowser()const// override;
{
	return mViewPathBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewTableBrowser>		
ViewBrowserPage::GetViewTableBrowser()const //override;
{
	return mViewTableBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewBrowserPage::SetPathMode(const int mode)// override;
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewBrowserPage::SetPathString(const ICls64& path_string) //override;
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewBrowserPage::SetAfterRefreshCls(const std::vector<const IIdent64*>& vec
	, const IIdent64* root
	, const wxString& ss
	, bool
	) //override;
{
	mCtrlFind->SetValue(ss);
	sigUpdateTitle();
}
