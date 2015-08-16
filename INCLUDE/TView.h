#ifndef __TVIEW_H
#define __TVIEW_H

#include "TModelArray.h"


namespace wh{

//-----------------------------------------------------------------------------
class T_View
{
public:
	virtual void SetModel(std::shared_ptr<IModel>& model) {}
	virtual void OnChangeModel(const IModel& model) {}
	
	virtual void UpdateModel()const {};
	virtual int  ShowModal() { return wxID_CANCEL;  };
};
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//template <	typename T_VecModel >
class T_VecView
{
public:
	virtual void SetModel(std::shared_ptr<IModel>) = 0;
	virtual void OnAppend(const IModel&, const std::vector<unsigned int>&) = 0;
	virtual void OnRemove(const IModel&, const std::vector<unsigned int>&) = 0;
	virtual void OnChange(const IModel&, const std::vector<unsigned int>&) = 0;

	virtual void GetSelected(std::vector<unsigned int>&) = 0;

	//virtual void Bind(const EventTag& eventType, const Functor &functor, int winid = wxID_ANY);
	


};


}//namespace wh{
#endif // __****_H
