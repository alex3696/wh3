#include "_pch.h"
#include "ModelObjPropList.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelObjPropList::ModelObjPropList()
	:IModelWindow()
{
}
//---------------------------------------------------------------------------

void ModelObjPropList::Update()
{
	//sigUpdatePropList(const PropValTable& rt, const IAct*);
}
//---------------------------------------------------------------------------
//virtual 
void ModelObjPropList::UpdateTitle() //override;
{
	sigUpdateTitle(mTitle, mIco);
	Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelObjPropList::Show()//override;
{
	//Update();
	sigShow();
}
//---------------------------------------------------------------------------
//virtual 
void ModelObjPropList::Load(const boost::property_tree::ptree& page_val)//override;
{
	//size_t offset = page_val.get<size_t>("CtrlPageHistory.Offset", 0);
	//size_t limit = page_val.get<size_t>("CtrlPageHistory.Limit", 20);
	//mDataModel.SetRowsOffset(offset);
	//mDataModel.SetRowsLimit(limit);
	//Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelObjPropList::Save(boost::property_tree::ptree& page_val)//override;
{
	//using ptree = boost::property_tree::ptree;
	//ptree content;
	//content.put("Offset", mDataModel.GetRowsOffset());
	//content.put("Limit", mDataModel.GetRowsLimit());
	//page_val.push_back(std::make_pair("CtrlPageHistory", content));
	//page_val.put("CtrlPageLogList.id", 33);

}
//---------------------------------------------------------------------------