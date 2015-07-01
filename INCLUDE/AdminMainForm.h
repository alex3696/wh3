#ifndef __ADMINMAINFORM_H
#define __ADMINMAINFORM_H

#include "globaldata.h"




namespace wh{

namespace gui{

/*
class UserGrpupStrProp: public wxStringProperty
{
public: 
	class View: public  TView<rec::group> 
	{
		public:
		friend class UserGrpupStrProp;
		UserGrpupStrProp& mOwnerProp;	
		
		virtual void modelChanged(const  TModel<rec::group> * model)
		{
			if(!model)
				return;
		
			auto prop = model->GetProperty();
		
			mOwnerProp.m_label=prop.mLabel;//this->SetLabel(prop.mLabel)
			mOwnerProp.m_value=prop.mComment;

			switch(model->GetState())
			{
			default:break;
			case  msCreated:	mOwnerProp.SetBackgroundColour(wxColour(210,230,255));break;//blue
			case  msExist:		mOwnerProp.SetBackgroundColour(wxColour(230,255,230));break;//green
			case  msUpdated:	mOwnerProp.SetBackgroundColour(wxColour(255,240,170));break;//red
			case  msDeleted:	mOwnerProp.SetBackgroundColour(wxColour(255,240,240));break;//yellow
			}
		}//void modelChanged(const TModel<t_whclass_prop> * model)
	
		virtual void OnSetValue()
		{
			auto prop_model=getModel();
			if(prop_model)
			{
				auto prop = prop_model->GetProperty();
				prop.mComment = mOwnerProp.GetValueAsString();
				edit( prop );
			}
		}//void OnSetValue()

		View(UserGrpupStrProp& prop)
			:mOwnerProp(prop){}
	};

	View mView;

    UserGrpupStrProp()
		:wxStringProperty(),mView(*this)
	{}
};
*/


class AdminMainForm: public wxDialog
{
private:
	wxAuiManager	m_AuiMgr;
	/// GUI дерево навигации 
	wxTreeCtrl*		m_ObjTree;

	/// GUI панель редактирования данных
	wxPanel*		m_pnl_content;

	//wxPropertyGrid*				m_propertyGrid1;
	// TVecModel<rec::group>	mModel;


	void OnSelectCategoryTreeItem(wxTreeEvent& evt);

public:
	
	AdminMainForm(	wxWindow* parent=NULL, 
					wxWindowID id = wxID_ANY,
					const wxString& title=wxEmptyString,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxSize( 700,500 ),//wxDefaultSize,
					long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,
					const wxString& name = wxDialogNameStr);

	~AdminMainForm();



};











}//namespace gui
}//namespace wh


#endif //__ADMINMAINFORM_H






