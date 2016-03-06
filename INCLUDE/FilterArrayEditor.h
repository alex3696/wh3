#ifndef __FILTERARRAYEDITOR_H
#define __FILTERARRAYEDITOR_H

#include "MTable.h"

namespace wh{
//-------------------------------------------------------------------------
class FilterArrayEditor
	:public wxScrolledWindow
{
public:
	FilterArrayEditor(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL,
		const wxString& name = wxPanelNameStr);

	void SetModel(std::shared_ptr<ITable>& newModel);

protected:
	std::shared_ptr<ITable>			mMTable;
	//std::shared_ptr<IFieldArray>	mModel;
	wxPropertyGrid*					mPropGrid;

	void OnApply(wxCommandEvent& evt);
	void OnClear(wxCommandEvent& evt);

	static wxPGProperty* MakeProperty(const wh::Field& field);
	bool GetFilterValue(wh::Field& field);

	sig::scoped_connection		mConnFieldAI;
	sig::scoped_connection		mConnFieldBR;
	sig::scoped_connection		mConnFieldAC;

	void OnFieldAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnFieldBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnFieldInfoChange(const IModel& newVec, const std::vector<unsigned int>& itemVec);

};
//-------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H
