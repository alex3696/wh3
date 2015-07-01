#ifndef __FAVORITES_H
#define __FAVORITES_H

#include "_pch.h"
#include "whDB.h"

namespace wh{
namespace favorites{

//----------------------------------------------------------------------------	

//----------------------------------------------------------------------------		
struct Link: boost::noncopyable
{
	enum 
	{
		lnkFolder		= 0,
		lnkClass		= 1,
		lnkObjNum		= 2,
		lnkObjNumFilter	= 3,
	};

	typedef std::tr1::shared_ptr<Link>					Sptr;
	
	unsigned int	mID;
	unsigned int	mPID;
	wxString		mLabel;

	wxString		mClassFilter;
	wxString		mObjName;
	wxString		mObjClass;

	Link(unsigned int	id=0,unsigned int pid=0,wxString lbl=wxEmptyString)	
		:mID(id),mPID(pid),mLabel(lbl)
	{}
	virtual ~Link(){}

	unsigned int GetLinkType()const
	{
		if ( mObjName.IsEmpty() && mObjClass.IsEmpty() && !mClassFilter.IsEmpty() )
			return lnkClass;
		else if ( !mObjName.IsEmpty() && !mObjClass.IsEmpty() && mClassFilter.IsEmpty() )
			return lnkObjNum;
		else if ( !mObjName.IsEmpty() && !mObjClass.IsEmpty() && !mClassFilter.IsEmpty() )
			return lnkObjNumFilter;
		//else if( mObjName.IsEmpty() && mObjClass.IsEmpty() && mClassFilter.IsEmpty())
		return lnkFolder;
	}

	inline bool IsFolder(){	return GetLinkType()==Link::lnkFolder;	}

	wxString GetLabel()const
	{
		wxString lbl;
		
		if(!mLabel.IsEmpty())
			lbl=mLabel;
		else
			switch (GetLinkType())
			{
				default: break;
				case lnkClass:lbl=mClassFilter;break;
				case lnkObjNum:lbl="["+mObjClass+"]"+mObjName;break;
				case lnkObjNumFilter:lbl="["+mObjClass+"]"+mObjName+"/"+mClassFilter;break;
			}
		return lbl;
	}


	Link* clone()const
	{
		Link* lnk=new Link(this->mID,this->mPID, this->mLabel );
		lnk->mClassFilter=this->mClassFilter;
		lnk->mObjName=this->mObjName;
		lnk->mObjClass=this->mObjClass;
		return lnk;
	}
	void copy(const Link& lnk)
	{
		mID=lnk.mID;
		mPID=lnk.mPID;
		mLabel=lnk.mLabel;
		mClassFilter=lnk.mClassFilter;
		mObjName=lnk.mObjName;
		mObjClass=lnk.mObjClass;
	}

};




//----------------------------------------------------------------------------
/** Модель данных для избранного */
class DataModel: public wxDataViewModel, public ctrlWithResMgr
{
protected:	
	struct LinkNode: public Link
	{
		typedef boost::ptr_vector<LinkNode>	_IDChild;

		LinkNode*	mParent;
		_IDChild	mChild;

		LinkNode(LinkNode*	lnk=NULL):mParent(lnk){}
	};

	LinkNode*				mData;	

	unsigned int LoadNode(unsigned int pid,LinkNode* node,wxDataViewItemArray &array)const;
public:

	DataModel();
	~DataModel();

	// implementation of base class virtuals to define model
	virtual unsigned int GetColumnCount() const					{ return 1;					}
	virtual wxString GetColumnType( unsigned int col ) const    { return !col ? "wxDataViewIconText" : "string"; }
	
	virtual bool IsContainer( const wxDataViewItem &item )const	
	{ 
		Link* itemLnk = static_cast<Link*>(item.GetID());
		return item.IsOk() ? itemLnk->IsFolder() : true;
	}
	
	virtual void GetValue( wxVariant &variant,const wxDataViewItem &item, unsigned int col ) const;
	virtual bool SetValue( const wxVariant &variant,const wxDataViewItem &item, unsigned int col )	{	return false;	}

	virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &parent,wxDataViewItemArray &array ) const;

    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,unsigned int column, bool ascending ) const
	{
		wxVariant var1,var2;
		GetValue(var1,item1,0);
		GetValue(var2,item2,0);
		
		wxDataViewIconText dat1;
		dat1<<var1;
		wxString str1=dat1.GetText();
		
		wxDataViewIconText dat2;
		dat2<<var2;
		wxString str2=dat2.GetText();

		int res = str1.CmpNoCase(str2);
		res = ascending? (-1)*res : res ;
		return res;
	}

	// 
	int MakeFolder	(const wxDataViewItem &item,const wxString& label);
	int InsertLink	(const Link& lnk);
	int DeleteLink	(const wxDataViewItem &item);
	int UpdateLabel	(const wxDataViewItem &item, const wxString& label);
	int MoveLink	(const wxDataViewItem &item, const wxDataViewItem &new_parent, bool new_parent_expanded=true);

	wxString GetLinkLabel(const wxDataViewItem &item)const;
	Link* GetLink(const wxDataViewItem &item)const;
	

};//class DataModel: public wxDataViewModel, public ctrlWithResMgr
//----------------------------------------------------------------------------





/** Панель для отображения дерева избранного, 
модель данных избранного в глобальных переменных в единственном экземпляре*/
class Panel: public wxPanel, public ctrlWithResMgr
{
private:
	
protected:
	DataModel*			m_Model;
	wxDataViewCtrl*		m_View;
	wxToolBar*			m_ToolBar;
	
	
	void InitDataView();
	void InitToolBar();

	void OnToolBarBtn_MakeFoder(wxCommandEvent& evt=wxCommandEvent());
	void OnToolBarBtn_Delete(wxCommandEvent& evt=wxCommandEvent());
	void OnToolBarBtn_UpdateLabel(wxCommandEvent& evt=wxCommandEvent());
	void OnActivated( wxDataViewEvent &event );

	void OnBeginDrag	( wxDataViewEvent &event );
	void OnDropPossible	( wxDataViewEvent &event );
	void OnDrop			( wxDataViewEvent &event );
public:
	Panel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL,const wxString& name = wxDialogNameStr );
	~Panel();
	
};
//----------------------------------------------------------------------------	







//----------------------------------------------------------------------------
}//namespace favorites

}//namespace wh






#endif//__FAVORITES_H