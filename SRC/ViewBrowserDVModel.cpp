#include "_pch.h"
#include "ViewBrowserDVModel.h"

using namespace wh;

//-----------------------------------------------------------------------------
//virtual 
unsigned int wxDVTableBrowser::GetColumnCount() const //override
{
	return 4;
}
//-----------------------------------------------------------------------------
//virtual 
wxString wxDVTableBrowser::GetColumnType(unsigned int col) const //override
{
	switch (col)
	{
	case 0: return "wxDataViewIconText";
	default: break;
	}
	return "string";
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableBrowser::HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const //override
{
	return true;
}

//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableBrowser::IsContainer(const wxDataViewItem &item)const //override
{
	if (!item.IsOk())
		return true;

	const auto node = static_cast<const IIdent64*> (item.GetID());
	const auto cls = dynamic_cast<const ICls64*>(node);
	if (mGroupByType
		&& cls
		&& !IsTop(item)
		&& ClsKind::Abstract != cls->GetKind())
		return true;

	return false;
}
//-----------------------------------------------------------------------------
void wxDVTableBrowser::GetErrorValue(wxVariant &variant, unsigned int col) const
{
	if (0 == col)
		variant << wxDataViewIconText("*ERROR*", wxNullIcon);
	else
		variant = "*ERROR*";
	return;
}
//-----------------------------------------------------------------------------
void wxDVTableBrowser::GetClsValue(wxVariant &variant, unsigned int col
	, const ICls64& cls) const
{
	auto mgr = ResMgr::GetInstance();

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
			variant = wxString::Format("%s %s", cls.GetObjectsQty(), cls.GetMeasure());
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
						variant = wxString::Format("%g", dperiod);
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
		}//if (idxCol.end() != it)
		else
		{
			auto prop_val = GetPropVal(cls, col);
			if (prop_val)
				variant = prop_val->mValue;
		}
	}break;
	}//switch (col)
}
//-----------------------------------------------------------------------------
void wxDVTableBrowser::GetObjValue(wxVariant &variant, unsigned int col
	, const IObj64& obj) const
{
	const wxIcon*  ico(&wxNullIcon);
	auto mgr = ResMgr::GetInstance();

	switch (col)
	{
	case 0:{
		//const wxIcon  ico_sel = wxArtProvider::GetIcon(wxART_TICK_MARK, wxART_MENU);
		const wxIcon  ico_lock_obj("OBJ_LOCK_24", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
	
		//if (obj.IsSelected())
		//	ico = &ico_sel;
		if (!obj.GetLockUser().empty())
			ico = &ico_lock_obj;
	
		variant << wxDataViewIconText(obj.GetTitle(), *ico);
	}break;
	case 1: variant = obj.GetCls()->GetTitle();	break;
	case 2: { 
		switch (obj.GetCls()->GetKind())
		{
		case ClsKind::QtyByOne:
		case ClsKind::QtyByFloat:
			if (mEditableQtyCol)
			{
				ObjectKey key(obj.GetId(), obj.GetParentId());
				const auto& it = mEditedValue.find(key);
				if (mEditedValue.cend() == it)
				{
					variant = wxString::Format("%s %s"
						, obj.GetQty()
						, obj.GetCls()->GetMeasure());
				}
				else
				{
					variant = wxString::Format("%s %s"
						, it->second
						, obj.GetCls()->GetMeasure());
				}
			}
			else
			{
				variant = wxString::Format("%s %s"
					, obj.GetQty()
					, obj.GetCls()->GetMeasure());
			}
		default: break;
		}//switch (obj.GetCls()->GetKind())
	}break;
	case 3: if (obj.GetPath())
		variant = obj.GetPath()->AsString();
		break;

	default: {
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
		}//if (idxCol.end() != it)
		else
		{
			auto prop_val = GetPropVal(obj, col);
			if (prop_val)
				variant = prop_val->mValue;
		}

	}break;
	}//switch (col)
}

//-----------------------------------------------------------------------------
void wxDVTableBrowser::GetValueInternal(wxVariant &variant,
	const wxDataViewItem &dvitem, unsigned int col) const
{
	if (IsTop(dvitem))
	{
		const auto mgr = ResMgr::GetInstance();
		if (0 == col)
			variant << wxDataViewIconText("..", mgr->m_ico_back24);
		return;
	}

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

//-----------------------------------------------------------------------------
//virtual 
void wxDVTableBrowser::GetValue(wxVariant &variant,
	const wxDataViewItem &dvitem, unsigned int col) const //override
{
	GetValueInternal(variant, dvitem, col);
	if (variant.IsNull())
		variant = "";
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableBrowser::GetAttr(const wxDataViewItem &item, unsigned int col,
	wxDataViewItemAttr &attr) const //override
{
	if (!item.IsOk() || IsTop(item))
		return false;

	const auto node = static_cast<const IIdent64*> (item.GetID());
	const auto obj = dynamic_cast<const IObj64*>(node);
	if (obj)
	{
		switch (col)
		{
		case 0:{
			attr.SetBold(true);
		}break;
		case 2:{
			switch (obj->GetCls()->GetKind())
			{
			case ClsKind::QtyByOne:
			case ClsKind::QtyByFloat:
				attr.SetBold(true);
				if (mEditableQtyCol)
					attr.SetBackgroundColour(wxYELLOW->ChangeLightness(160));
			default: break;
			}
		}break;
		case 1:case 3: {
			attr.SetColour(*wxBLUE);
			return true;
		}break;
		default: {
			const auto& idxCol = mActColumns.get<1>();
			const auto it = idxCol.find(col);
			if (idxCol.end() != it)
			{
				switch (it->mAInfo) {
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
			else
			{
				auto prop_val = GetPropVal(*obj, col);
				if (prop_val && prop_val->mProp->IsLinkOrFile())
				{
					attr.SetColour(*wxBLUE);
					return true;
				}
			}
		}break;
		}//switch (col)
		return false;
	}

	const auto cls = dynamic_cast<const ICls64*>(node);
	if (cls)
	{
		auto prop_val = GetPropVal(*cls, col);
		if (prop_val && prop_val->mProp->IsLinkOrFile())
		{
			attr.SetColour(*wxBLUE);
			return true;
		}
	}//if (cls)
	return false;
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableBrowser::SetValue(const wxVariant &variant, const wxDataViewItem &item,
	unsigned int col)//override
{
	if(!mEditableQtyCol)
		return false;

	const auto node = static_cast<const IIdent64*> (item.GetID());
	const auto ident = node;
	const auto cls = dynamic_cast<const ICls64*>(ident);
	if (cls)
		return false;
	const auto obj = dynamic_cast<const IObj64*>(ident);
	if (!obj)
		return false;

	ClsKind kind = obj->GetCls()->GetKind();
	ObjectKey key(obj->GetId(), obj->GetParentId());
	wxString str_val = variant.GetString();
	switch (kind)
	{
	case ClsKind::QtyByOne: {
		unsigned long sval;
		unsigned long ival;
		if (str_val.ToCULong(&ival) && obj->GetQty().ToULong(&sval)
			&& sval >= ival && 0 < ival)
		{
			mEditedValue[key]=str_val;
			return true;
		}
		else
		{
			str_val.RemoveLast(obj->GetCls()->GetMeasure().size());
			str_val.Trim();
			if (str_val.ToCULong(&ival) && obj->GetQty().ToULong(&sval)
				&& sval >= ival && 0 < ival)
			{
				mEditedValue[key] = str_val;
				return true;
			}
		}
			
	}break;
	case ClsKind::QtyByFloat: {
		double sval;
		double ival;
		if (str_val.ToCDouble(&ival) && obj->GetQty().ToCDouble(&sval)
			&& sval >= ival && 0 < ival)
		{
			mEditedValue[key] = str_val;
			return true;
		}
		else
		{
			str_val.RemoveLast(obj->GetCls()->GetMeasure().size());
			str_val.Trim();
			if (str_val.ToCDouble(&ival) && obj->GetQty().ToCDouble(&sval)
				&& sval >= ival && 0 < ival)
			{
				mEditedValue[key] = str_val;
				return true;
			}
		}
	}break;
	default: break;
	}
	return false;
}

//-----------------------------------------------------------------------------
//virtual 
int wxDVTableBrowser::Compare(const wxDataViewItem &item1, const wxDataViewItem &item2
	, unsigned int column, bool ascending) const //override
{
	wxVariant value1, value2;
	GetValue(value1, item1, column);
	GetValue(value2, item2, column);

	if (mCurrentRoot)
	{
		if (static_cast<const IIdent64*>(item1.GetID()) == mCurrentRoot)
			return -1;
		else if (static_cast<const IIdent64*>(item2.GetID()) == mCurrentRoot)
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

//-----------------------------------------------------------------------------
//virtual 
wxDataViewItem wxDVTableBrowser::GetParent(const wxDataViewItem &item) const //override
{
	if (!item.IsOk() || IsTop(item))
		return wxDataViewItem(nullptr);

	if (IsListModel())
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

//-----------------------------------------------------------------------------
//virtual 
unsigned int wxDVTableBrowser::GetChildren(const wxDataViewItem &parent
	, wxDataViewItemArray &arr) const //override
{
	if (!parent.IsOk())
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
//-----------------------------------------------------------------------------
//virtual 
bool  wxDVTableBrowser::IsListModel() const //override
{
	return !mGroupByType;
}
//-----------------------------------------------------------------------------
const IIdent64* wxDVTableBrowser::GetCurrentRoot()const
{
	return mCurrentRoot;
}
//-----------------------------------------------------------------------------
void wxDVTableBrowser::SetClsList(const std::vector<const IIdent64*>& current
	, const IIdent64* curr, bool group_by_type, int mode)
{
	mGroupByType = group_by_type;
	mCurrentRoot = curr;
	mClsList = current;
	mMode = mode;
	mEditedValue.clear();

	Cleared();
}
//-----------------------------------------------------------------------------


const std::vector<const IIdent64*>& wxDVTableBrowser::GetClsList()const
{
	return mClsList;
}
//-----------------------------------------------------------------------------
int wxDVTableBrowser::GetMode()const
{
	return mMode;
}
//-----------------------------------------------------------------------------
std::shared_ptr<const PropVal> wxDVTableBrowser::GetPropVal(const IObj64& obj, int col_idx)const
{
	const auto& idxOPropCol = mOPropColumns.get<1>();
	auto prop_it = idxOPropCol.find(col_idx);
	if (idxOPropCol.end() != prop_it)
	{
		const auto favOPrpop = obj.GetFavOPropValue();
		auto val_it = favOPrpop.find(prop_it->mPid);
		if (favOPrpop.cend() != val_it)
			return *val_it;
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
std::shared_ptr<const PropVal> wxDVTableBrowser::GetPropVal(const ICls64& cls, int col_idx)const
{
	const auto& idxCPropCol = mCPropColumns.get<1>();
	auto prop_it = idxCPropCol.find(col_idx);
	if (idxCPropCol.end() != prop_it)
	{
		const auto favCPrpop = cls.GetFavCPropValue();
		auto val_it = favCPrpop.find(prop_it->mPid);
		if (favCPrpop.cend() != val_it)
			return *val_it;
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
std::shared_ptr<const PropVal> wxDVTableBrowser::GetPropVal(const wxDataViewItem &item, int col_idx)const
{
	const auto ident = static_cast<const IIdent64*> (item.GetID());
	const auto cls = dynamic_cast<const ICls64*>(ident);
	if (cls)
		return GetPropVal(*cls, col_idx);
	else
	{
		const auto obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
			return GetPropVal(*obj, col_idx);
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
inline bool wxDVTableBrowser::IsTop(const wxDataViewItem &item)const
{
	return item.GetID() == mCurrentRoot;
}

