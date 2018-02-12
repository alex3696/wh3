#include "_pch.h"
#include "ModelFav.h"
using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelFav::ModelFav()
	//:mCls(std::make_shared_ptr<ICls64>)
{
	
}
//---------------------------------------------------------------------------
void ModelFav::DoSetClsId(int64_t cid)
{
	UpdateTitle();
	DoRefresh();
}
//---------------------------------------------------------------------------
void ModelFav::DoRefresh()
{
}
//---------------------------------------------------------------------------
void ModelFav::DoAddClsProp(int64_t clsId, int64_t propId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddObjProp(int64_t clsId, int64_t propId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddPrevios(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddPeriod(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddNext(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddLeft(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveClsProp(int64_t clsId, int64_t propId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveObjProp(int64_t clsId, int64_t propId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemovePrevios(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemovePeriod(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveNext(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveLeft(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelFav::UpdateTitle() //override;
{
	wxString title = wxString::Format(
		"Выберите атрибуты %s для отображения"
		, mCls? mCls->GetTitle(): "***" );
	const auto& ico = ResMgr::GetInstance()->m_ico_favprop_select24;

	sigUpdateTitle(title, ico);
}
