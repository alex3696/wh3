#include "_pch.h"
#include "ViewBrowser.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"
#include "wxComboBtn.h"
#include "config.h"

using namespace wh;

//-----------------------------------------------------------------------------
struct ActInfoColumn
{
	int64_t		mAid;
	//wxString	mActTitle;
	char		mAcol;
	int			mIndex;
	
	ActInfoColumn() 
		:mAid(0), mAcol(0), mIndex(0)
	{}
	ActInfoColumn(const int64_t& aid, const char acol,int idx)
		:mAid(aid), mAcol(acol), mIndex(idx)
	{}
};
//-----------------------------------------------------------------------------
using Columns = boost::multi_index_container
<
	ActInfoColumn,
	indexed_by
	<
		ordered_unique < 
							composite_key
							<
								ActInfoColumn
								, member<ActInfoColumn, int64_t, &ActInfoColumn::mAid>
								, member<ActInfoColumn, char, &ActInfoColumn::mAcol>
							> 
						>
		, ordered_unique<member<ActInfoColumn, int, &ActInfoColumn::mIndex>>
	>
>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVTableBrowser
	: public wxDataViewModel
{
	const IIdent64* mCurrentRoot = nullptr;
	const	std::vector<const IIdent64*>* mClsList = nullptr;
	bool	mGroupByType = true;
	//wxRegEx mStartNum = "(^[0-9]{1,9})";
	//wxRegEx mStartNum = "^(-?)(0|([1-9][0-9]*))((\\.|\\,)[0-9]+)?$";
	wxRegEx mStartNum = "^(-?)([0-9]*)((\\.|\\,)[0-9]+)?";

	const wxString format_d = wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
	const wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
	const wxString mActNotExecuted="не выполнялось";
public:
	Columns			mActColumns;

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
				if (it->mAcol & 2)
				{
					wxString period;
					if (cls.GetActPeriod(it->mAid, period))
					{
						double dperiod;
						if (period.ToCDouble(&dperiod))
							variant = wxString::Format("%g", dperiod / 86400);
					}
				}
			}//if (idxCol.end() != it)
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
				
				switch (it->mAcol)
				{
				case 1: {
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
				case 2: if (!mGroupByType) {
					wxString period;
					if (obj.GetActPeriod(it->mAid, period))
					{
						double dperiod;
						if (period.ToCDouble(&dperiod))
						{
							variant = wxString::Format("%g", dperiod / 86400);
							//variant = dperiod / 86400;
						}
					}
				} break;
				case 4: {
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
				case 8: {
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
					variant = "*error*";
				}break;
				}//switch (it->mAcol)
			}
		}break;
		}//switch (col)
	}

	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
		const auto node = static_cast<const IIdent64*> (dvitem.GetID());
		const auto ident = node;
		if (!ident)
		{
			GetErrorValue(variant, col);
			return;
		}
		
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
				switch (it->mAcol){
				default: break;
				case 4: { //next
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
				case 8: { 
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
				}//switch (it->mAcol)
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
			if (!value2.IsNull())
				return -1;
		else
			if (!value2.IsNull())
				return 1;

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
					}//if (mStartNum.Matches(str1))
				}//if (match && str1.substr(start, len).ToLong(&num1))
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
		if (!parent.IsOk() && mClsList)
		{
			if (mCurrentRoot && 1 < mCurrentRoot->GetId())
			{
				wxDataViewItem dvitem((void*)mCurrentRoot);
				arr.push_back(dvitem);
			}
			for (const auto& child_cls : *mClsList)
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

	void SetClsList(const std::vector<const IIdent64*>* current, const IIdent64* curr, bool group_by_type)
	{
		mGroupByType = group_by_type;
		mCurrentRoot = curr;
		mClsList = current;
		Cleared();
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
		 | wxDV_MULTIPLE
		);
	mTable = table;

	auto dv_model = new wxDVTableBrowser();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	int ch = table->GetCharHeight();
	table->SetRowHeight(ch * 1.6 );
	//int ch = table->GetCharHeight();
	//table->SetRowHeight(ch * 4 + 2);
	ResetColumns();

	table->GetTargetWindow()->SetToolTip("ToolTip");
	table->GetTargetWindow()->Bind(wxEVT_MOTION, &ViewTableBrowser::OnCmd_MouseMove, this);

	table->Bind(wxEVT_DATAVIEW_COLUMN_SORTED
		, [this](wxDataViewEvent& evt) { RestoreSelect(); });

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
	//tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigInsertType(); }, wxID_NEW_TYPE);
	//tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigInsertObject(); }, wxID_NEW_OBJECT);
	//tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigDelete(); }, wxID_DELETE);
	//tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) {sigUpdate(); }, wxID_EDIT);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) 
		{ sigToggleGroupByType(); }, wxID_VIEW_LIST);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ SetShowFav(); }, wxID_PROPERTIES);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{ sigShowSettings(); }, wxID_HELP_INDEX);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{sigShowHelp("ViewBrowserPage"); }, wxID_HELP_INDEX);

	

	wxAcceleratorEntry entries[15];
	char i = 0;
	entries[i++].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F5,   wxID_REFRESH);
	entries[i++].Set(wxACCEL_NORMAL, WXK_BACK, wxID_UP);
	
	entries[i++].Set(wxACCEL_NORMAL, WXK_F6, wxID_REPLACE);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F7, wxID_EXECUTE);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F9, wxID_VIEW_DETAILS);

	entries[i++].Set(wxACCEL_CTRL, WXK_INSERT, wxID_ADD);
	entries[i++].Set(wxACCEL_CTRL, (int) 'T', wxID_NEW_TYPE);
	entries[i++].Set(wxACCEL_CTRL, (int) 'O', wxID_NEW_OBJECT);
	entries[i++].Set(wxACCEL_CTRL, WXK_DELETE, wxID_DELETE);
	entries[i++].Set(wxACCEL_CTRL, WXK_RETURN, wxID_EDIT);

	entries[i++].Set(wxACCEL_CTRL, (int) 'G', wxID_VIEW_LIST);
	entries[i++].Set(wxACCEL_CTRL, (int) 'P', wxID_PROPERTIES);
	entries[i++].Set(wxACCEL_CTRL, (int) 'N', wxID_SETUP);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F1, wxID_HELP_INDEX);

	wxAcceleratorTable accel(15, entries);
	table->SetAcceleratorTable(accel);

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_MouseMove(wxMouseEvent& evt)
{
	auto pos = evt.GetPosition();

	wxDataViewItem item(nullptr);
	wxDataViewColumn* col = nullptr;
	
	mTable->HitTest(pos, item, col);
	if (!col || !item.IsOk())
		return;
	
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());

	if (!ident)
		return;
	
	mTable->GetTargetWindow()->GetToolTip()->SetTip(ident->GetIdAsString());
	
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

	if (mClsList.empty())
		return nullptr;
	auto it = std::find_if(mClsList.cbegin(), mClsList.cend()
		, [&id](const IIdent64* it)
		{ 
			return it->GetId() == id; 
		});

	if (mClsList.cend() != it)
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

	if (1 < mParentCid)	//(mParent && 1 != mParent->GetId())
	{
		auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
		if (dvmodel)
			return dvmodel->GetCurrentRoot();
	}
		
	if (!mClsList.empty())
		return mClsList.front();
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
		if(col)
			col->SetWidth(mTable->GetBestColumnWidth(i));
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

	if (dvmodel->mActColumns.empty() && 4 == table->GetColumnCount() )
		return;

	table->ClearColumns();
	dvmodel->mActColumns.clear();

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

	for (const auto& ident : vec)
	{
		const auto obj = dynamic_cast<const IObj64*>(ident);
		if (!obj)
			continue;

		const auto& fav_act = obj->GetCls()->GetFavActInfo();
		for (const auto& fa_it : fav_act)
		{
			auto aid = fa_it.mAct->GetId();
			char visible = fa_it.mVisible;
			for (char v = 1; v <= 8; v <<= 1)
			{
				if (visible & v)
				{
					auto& idx0 = dvmodel->mActColumns.get<0>();
					auto it = idx0.find(boost::make_tuple(aid, (char)(visible & v)));
					if (idx0.end() == it)
					{
						ActInfoColumn acol(aid
							, (char)(visible & v)
							, mTable->GetColumnCount());

						dvmodel->mActColumns.emplace(acol);

						wxString str;
						switch (acol.mAcol)
						{
						case 2: str = " период(сут.)"; break;
						case 4: str = " след."; break;
						case 8: str = " осталось(сут.)"; break;
						default:str = " пред."; break;
						}

						wxString title;
						title << fa_it.mAct->GetTitle() << str;
						auto col = this->mTable->AppendTextColumn(title, acol.mIndex
							, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT
							, wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
						auto col_pos = mTable->GetModelColumnIndex(acol.mIndex);
						col->SetWidth(mTable->GetBestColumnWidth(col_pos));

					}
				}//if (visible & v)
			}//for (char v = 1; v <= 8; v<<=1)
		}

		/*
		auto begin = obj->GetActInfo().begin();
		auto end = obj->GetActInfo().end();
		while (begin != end)
		{
			auto act_col_begin = begin->second.begin();
			auto act_col_end = begin->second.end();
			wxString str_aid = begin->first.data();
			int64_t aid;
			if (str_aid.ToLongLong(&aid))
			{
				while (act_col_begin != act_col_end)
				{
					wxString str_ainfo = act_col_begin->first.data();
					long ainfo;
					if (str_ainfo.ToLong(&ainfo))
					{
						auto& idx0 = dvmodel->mActColumns.get<0>();
						auto it = idx0.find(boost::make_tuple(aid, (char)ainfo));
						if (idx0.end() == it)
						{
							ActInfoColumn acol(aid
								, (char)ainfo
								, mTable->GetColumnCount() + 1);

							dvmodel->mActColumns.emplace(acol);

							wxString str;
							str << acol.mAid << ":" << (int)acol.mAcol;
							this->mTable->AppendTextColumn(str, acol.mIndex
								, wxDATAVIEW_CELL_INERT, -1
								, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
						}

					}//if (str_ainfo.ToLong(&ainfo))
					++act_col_begin;
				}
			}//if (str_aid.ToLongLong(&aid))
			++begin;
		}//while (begin != end)
		*/
	}//for (const auto& obj : *ot)
	
}

//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetBeforeRefreshCls(const std::vector<const IIdent64*>& 
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
	mClsList.clear();
	mParentCid = 0;
	//mParent = nullptr;
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (dvmodel)
	{
		dvmodel->SetClsList(nullptr, nullptr, group_by_type);
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
		mClsList = vec;
		dvmodel->SetClsList(&mClsList, (root && 1 != root->GetId()) ? root : nullptr, group_by_type);

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
		else
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
	const auto& obj = dynamic_cast<const IObj64*>(ident);
	if (obj)
	{
		int64_t oid = obj->GetId();
		int64_t parent_oid = obj->GetParentId();
		sigObjUpdate(oid, parent_oid);
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
		//| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить(CTRL+R или CTRL+F5)");
	//tool_bar->AddTool(wxID_UP,"Вверх", mgr->m_ico_back24, "Вверх(BACKSPACE)");
	tool_bar->AddTool(wxID_REPLACE, "Переместить", mgr->m_ico_move24, "Переместить(F6)");
	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24, "Выполнить(F7)");
	tool_bar->AddTool(wxID_VIEW_DETAILS, "Подробно", mgr->m_ico_views24, "Подробно(F9)");
	
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
					AppendBitmapMenu(&add_menu, wxID_NEW_TYPE, "Создать тип", mgr->m_ico_add_type24);
				if ((int)currBaseGroup >= (int)bgObjDesigner)
					AppendBitmapMenu(&add_menu, wxID_NEW_OBJECT, "Создать объект", mgr->m_ico_add_obj24);
				wxRect rect = tb->GetToolRect(evt.GetId());
				wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
				pt = tb->ScreenToClient(pt);
				tb->PopupMenu(&add_menu, pt);
				tb->SetToolSticky(evt.GetId(), false);
				tb->Refresh();

			}
			, wxID_ADD);

		tool_bar->AddTool(wxID_DELETE, "Удалить", mgr->m_ico_delete24, "Удалить(CTRL+DELETE)");
		tool_bar->AddTool(wxID_EDIT, "Редактировать", mgr->m_ico_edit24, "Редактировать(CTRL+ENTER)");
		tool_bar->AddSeparator();
	}

	tool_bar->AddTool(wxID_VIEW_LIST, "Группировать", mgr->m_ico_group_by_type24, "Группировать по типу(CTRL+G)");
	tool_bar->AddTool(wxID_PROPERTIES, "Свойства", mgr->m_ico_favprop_select24, "Выбрать свойства(CTRL+P)");
	tool_bar->AddTool(wxID_SETUP, "Настройки", mgr->m_ico_options24
		, "Настройки внешнего вида таблийы истории(CTRL+N)");
	tool_bar->AddTool(wxID_HELP_INDEX, "Справка", wxArtProvider::GetBitmap(wxART_HELP, wxART_TOOLBAR), "Справка(F1)");

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
		{ sigShowSettings(); }, wxID_HELP_INDEX);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&)
		{sigShowHelp("ViewBrowserPage"); }, wxID_HELP_INDEX);

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
	mPanel = panel;

	auto window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	panel->mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE,5);
	panel->mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);

	

	mViewToolbarBrowser = std::make_shared<ViewToolbarBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("BrowserToolBar"))
		.CaptionVisible(false)
		.Top().Dock().Fixed().Floatable(false).Row(0).Layer(0).ToolbarPane()
		.Fixed()
		);
	
	long style = wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND;
	//auto navigateToolBar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	//navigateToolBar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh16, "Обновить(CTRL+R)");
	//navigateToolBar->AddTool(wxID_UP,"Вверх", mgr->m_ico_sort_asc16, "Вверх(BACKSPACE)");
	//navigateToolBar->Realize();
	//panel->mAuiMgr.AddPane(navigateToolBar, wxAuiPaneInfo().Name(wxT("navigateToolBar"))
	//	.Top().CaptionVisible(false).Dock().Fixed().DockFixed(true)
	//	.Row(1).Layer(0)
	//);

	mViewPathBrowser = std::make_shared<ViewPathBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewPathBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("PathBrowser"))
		.Top().CaptionVisible(false).Dock().Resizable()
		.Row(1).Layer(0)
	);

	mCtrlFind = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
									wxDefaultSize, wxTE_PROCESS_ENTER);
	panel->mAuiMgr.AddPane(mCtrlFind, wxAuiPaneInfo().Name("mCtrlFind")
		.Top().CaptionVisible(false).Dock().Fixed()
		.Row(1).Layer(0)
	);

	auto findToolBar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	//auto mCtrlFind = new wxTextCtrl(findToolBar, wxID_ANY);
	//findToolBar->AddControl(mCtrlFind, "Поиск");
	findToolBar->AddTool(wxID_FIND, "Поиск", wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU), "Поиск(CTRL+F)");
	findToolBar->Realize();
	panel->mAuiMgr.AddPane(findToolBar, wxAuiPaneInfo().Name(wxT("findToolBar"))
		.Top().CaptionVisible(false).Dock().Fixed().DockFixed(true)
		.Row(1).Layer(0)
	);


	mViewTableBrowser = std::make_shared<ViewTableBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewTableBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("TableBrowser"))
		.CaptionVisible(false)
		.CenterPane()
		);



	mViewTableBrowser->GetWnd()->SetFocus();
	panel->mAuiMgr.Update();


	mCtrlFind->Bind(wxEVT_COMMAND_TEXT_ENTER, &ViewBrowserPage::OnCmd_Find, this);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewBrowserPage::OnCmd_Find, this, wxID_FIND);
}
//-----------------------------------------------------------------------------
void ViewBrowserPage::OnCmd_Find(wxCommandEvent& evt)
{
	sigFind(mCtrlFind->GetValue());
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
