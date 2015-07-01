#include "_pch.h"
#include "ClassPropEditor.h"
//#include "ActPropInfoView.h"
#include "PropMetaPGProperty.h"


using namespace wh;
using namespace wh::view;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//GroupView
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ClassPropEditor::ClassPropEditor(	wxWindow*		parent, 
				wxWindowID		id ,
				const wxString& title,
				const wxPoint&	pos,
				const wxSize&	size, 
				long style ,
				const wxString& name  )
	:EditorDlg(parent,id,title,pos,size,style,name)
{
	SetTitle("�������������� ������ �������������");
	
	mPropGrid = new wxPropertyGrid(this);

	GetSizer()->Insert( 0 ,  mPropGrid, 1, wxALL|wxEXPAND, 0 );

	auto prop = new MetaPropProperty(L"��������" );
	prop->SetSelectOnly(true);
	mPropGrid->Append(prop);
	mPropGrid->Append(new wxStringProperty(L"��������" ));

	this->Layout(  );

}
//---------------------------------------------------------------------------
ClassPropEditor::~ClassPropEditor()
{

}
//---------------------------------------------------------------------------
void ClassPropEditor::DoRefresh()
{
	auto new_model = GetModel();
	if(new_model)
	{
		rec::clsprop cprop = new_model->GetProperty();
		const rec::metaprop& mprop=*((rec::metaprop*)&cprop);
		
		mPropGrid->GetPropertyByLabel(L"��������")->SetValue( wxVariant()<<mprop );
		mPropGrid->GetPropertyByLabel(L"��������")->SetValueFromString( cprop.mVal );
	}
}
//---------------------------------------------------------------------------
//model::ClsProp* ClassPropEditor::GetModelData()const
void  ClassPropEditor::DoEditModel()
{
	rec::clsprop edit_rec;
	
	wxVariant value	=	mPropGrid->GetPropertyByLabel("��������")->GetValue();
	rec::metaprop& mprop = *((rec::metaprop*)&edit_rec);
	mprop<<value;//= value.As<rec::clsprop>();

	edit_rec.mVal	=	mPropGrid->GetPropertyByLabel("��������")->GetValueAsString();
	
	GetModel()->SetProperty(edit_rec);
}
