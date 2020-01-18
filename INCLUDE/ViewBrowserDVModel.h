#ifndef __VIEW_BROWSER_DVMODEL_H
#define __VIEW_BROWSER_DVMODEL_H

#include "ModelBrowserData.h"
namespace wh {
//-----------------------------------------------------------------------------
struct ActColumn
{
	int64_t			mAid;
	FavAPropInfo	mAInfo;
	int				mColumn;
	
	ActColumn() 
		:mAid(0), mAInfo(FavAPropInfo::UnnownProp), mColumn(0)
	{}
	ActColumn(const int64_t& aid, FavAPropInfo acol,int idx)
		:mAid(aid), mAInfo(acol), mColumn(idx)
	{}
};
//-----------------------------------------------------------------------------
using ActColumns = boost::multi_index_container
<
	ActColumn,
	indexed_by
	<
		ordered_unique < 
							composite_key
							<
								ActColumn
								, member<ActColumn, int64_t, &ActColumn::mAid>
								, member<ActColumn, FavAPropInfo, &ActColumn::mAInfo>
							> 
						>
		, ordered_unique<member<ActColumn, int, &ActColumn::mColumn>>
	>
>;
//-----------------------------------------------------------------------------
struct PropColumn
{
	int64_t			mPid;
	int				mColumn;

	PropColumn()
		:mPid(0), mColumn(0)
	{}
	PropColumn(const int64_t& pid,  int idx)
		:mPid(pid), mColumn(idx)
	{}
};
//-----------------------------------------------------------------------------
using PropColumns = boost::multi_index_container
<
	PropColumn,
	indexed_by
	<
		  ordered_unique<member<PropColumn, int64_t, &PropColumn::mPid>>
		, ordered_unique<member<PropColumn, int, &PropColumn::mColumn>>
	>
>;

//-----------------------------------------------------------------------------
class wxDVTableBrowser
	: public wxDataViewModel
{
	bool mEditableQtyCol = false;
	
	const IIdent64* mCurrentRoot = nullptr;
	std::vector<const IIdent64*>	mClsList;
	int mMode = 0;


	bool	mGroupByType = true;
	//wxRegEx mStartNum = "(^[0-9]{1,9})";
	//wxRegEx mStartNum = "^(-?)(0|([1-9][0-9]*))((\\.|\\,)[0-9]+)?$";
	wxRegEx mStartNum = "^(-?)([0-9]*)((\\.|\\,)[0-9]+)?";

	const wxString format_d = wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
	const wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
	const wxString mActNotExecuted = "не выполнялось";

	void GetValueInternal(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const;
public:
	ActColumns		mActColumns;
	PropColumns		mCPropColumns;
	PropColumns		mOPropColumns;

	wxDVTableBrowser() {};
	~wxDVTableBrowser() {};

	virtual unsigned int	GetColumnCount() const override;
	virtual wxString		GetColumnType(unsigned int col) const override;
	virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const override;

	virtual bool IsContainer(const wxDataViewItem &item)const override;

	void GetErrorValue(wxVariant &variant, unsigned int col) const;

	void GetClsValue(wxVariant &variant, unsigned int col
		, const ICls64& cls) const;

	void GetObjValue(wxVariant &variant, unsigned int col
		, const IObj64& obj) const;

	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override;
	virtual bool GetAttr(const wxDataViewItem &item, unsigned int col,
		wxDataViewItemAttr &attr) const override;
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col)override;

	virtual int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2
		, unsigned int column, bool ascending) const override;

	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override;

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &arr) const override;

	const IIdent64* GetCurrentRoot()const;
	virtual bool  IsListModel() const override;

	void SetClsList(const std::vector<const IIdent64*>& current
		, const IIdent64* curr, bool group_by_type, int mode);

	const std::vector<const IIdent64*>& GetClsList()const;
	int GetMode()const;
	std::shared_ptr<const PropVal> GetPropVal(const IObj64& obj, int col_idx)const;
	std::shared_ptr<const PropVal> GetPropVal(const ICls64& cls, int col_idx)const;
	std::shared_ptr<const PropVal> GetPropVal(const wxDataViewItem &item, int col_idx)const;
	inline bool IsTop(const wxDataViewItem &item)const;
	void SetEditableQty(bool editable) { mEditableQtyCol = editable; }
	bool IsEditableQty()const { return mEditableQtyCol; }

	sig::signal<bool(const ObjectKey&, const wxString&)> sigSetQty;
};


//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H