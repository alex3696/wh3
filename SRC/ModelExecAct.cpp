#include "_pch.h"
#include "ModelExecAct.h"

using namespace wh;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelActExecWindow::ModelActExecWindow()
	:mModelActBrowser(std::make_shared<ModelActBrowserWindow>())
	, mModelObjBrowser(std::make_shared<ModelBrowser>())
{
}
//---------------------------------------------------------------------------
void ModelActExecWindow::SetObjects(const std::set<ObjectKey>& obj)
{
	mModelObjBrowser->DoSetObjects(obj);
	// Execut lock and transfer acts to ModelActBrowserModel

	//mModelActBrowser->SetActs();
}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::UpdateTitle()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Show()
{ 
	sigShow(); 
}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Init()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Load(const boost::property_tree::wptree& page_val)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Save(boost::property_tree::wptree& page_val)
{

}
//---------------------------------------------------------------------------