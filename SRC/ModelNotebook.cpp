#include "_pch.h"
#include "ModelNotebook.h"
#include "globaldata.h"
#include "config.h"

using namespace wh;


std::shared_ptr<ICtrlWindow> ModelNotebook::MkWindowNoSignal(const wxString& name)
{
	namespace ph = std::placeholders;

	auto controller = whDataMgr::GetInstance()->mContainer;
	std::shared_ptr<ICtrlWindow> ctrl
		= controller->GetObject<ICtrlWindow>(name);

	if (!ctrl)
		return ctrl;
	
	auto item = std::make_shared<WindowItem>(ctrl);

	//item->connModelChTitle = ctrl->sigUpdateTitle.connect
	//	(std::bind(&WindowListModel::OnSigUpdate, this, ph::_1, ph::_2, ph::_3));
	//item->connModelShow = ctrl->sigShow.connect
	//	(std::bind(&WindowListModel::OnSigShow, this, ph::_1));
	//item->connModelClose = ctrl->sigClose.connect
	//	(std::bind(&WindowListModel::OnSigClose, this, ph::_1));

	item->connModelChTitle = ctrl->sigUpdateTitle.connect(sigAfterChWindow);
	item->connModelShow = ctrl->sigShow.connect(sigShowWindow);
	item->connModelClose = ctrl->sigClose.connect(sigBeforeRmWindow);
	mWindowList.emplace_back(item);
	return ctrl;
}
//-----------------------------------------------------------------------------
void ModelNotebook::MkWindow(const wxString& factory)
{
	auto ctrl = MkWindowNoSignal(factory);
	if (ctrl)
	{
		wxWindow* wnd = ctrl->GetView()->GetWnd();
		sigAfterMkWindow(ctrl->GetView()->GetWnd());
		ctrl->UpdateTitle();
		ctrl->Show();	
	}

}
//-----------------------------------------------------------------------------
void ModelNotebook::RmWindow(wxWindow* wnd)
{
	auto& wndIdx = mWindowList.get<1>();

	auto it = wndIdx.find(wnd);
	if (wndIdx.end() != it)
	{
		(*it)->mCtrlWindow->Close();
		wndIdx.erase(it);
	}
	/*

	auto it = mWindowList.begin();
	auto it_end = mWindowList.end();
	while (it != it_end)
	{
		if ((*it)->mCtrlWindow->GetView()->GetWnd() == wnd)
		{
			(*it)->mCtrlWindow->Close();
			//sigBeforeRmWindow(wnd);
			mWindowList.erase(it);
			break;
		}
		++it;
	}
	*/
	
}
//-----------------------------------------------------------------------------
void ModelNotebook::ShowWindow(wxWindow* wnd)
{
	auto& wndIdx = mWindowList.get<1>();

	auto it = wndIdx.find(wnd);
	if (wndIdx.end() != it)
	{
		(*it)->mCtrlWindow->Show();
	}
}
//-----------------------------------------------------------------------------
void ModelNotebook::ShowWindowByPos(size_t pos)
{
	if (mWindowList.size() > pos)
		mWindowList[pos]->mCtrlWindow->Show();
}
//-----------------------------------------------------------------------------

void ModelNotebook::Load(const boost::property_tree::ptree& notebook)
{
try{
	using ptree = boost::property_tree::ptree;

	if (notebook.not_found() != notebook.find("Pages"))
	{
		for (const ptree::value_type &v : notebook.get_child("Pages"))
		{
			ptree::value_type page = *v.second.begin();
			auto name = page.first.c_str();
			//auto name = v.second.get<std::string>("Name", "");
			auto ctrl = MkWindowNoSignal(name);
			if (ctrl)
			{
				ctrl->Load(v.second);
				wxWindow* wnd = ctrl->GetView()->GetWnd();
				sigAfterMkWindow(ctrl->GetView()->GetWnd());
				ctrl->UpdateTitle();
			}
		}//for 
	}
	auto active_page = notebook.get<int>("ActivePage", 0);
	ShowWindowByPos(active_page);

}//try
catch (boost::exception & e)
{
	whDataMgr::GetDB().RollBack();
	wxLogWarning(wxString(diagnostic_information(e)));
}///catch(boost::exception & e)
catch (...)
{
	wxLogWarning(wxString("������ �������� ������������"));
}//catch(...)	
	
}
//-----------------------------------------------------------------------------
void ModelNotebook::Save(boost::property_tree::ptree& notebook)
{
try{
	using ptree = boost::property_tree::ptree;
	
	ptree pages;

	for (const auto& item : mWindowList)
	{
		ptree page;
		auto ctrl = item->mCtrlWindow;
		ctrl->Save(page);
		pages.push_back(std::make_pair("", page));
	}

	notebook.put_child("Pages", pages);
	notebook.put("ActivePage", "0");
	
	

}//try
catch (boost::exception & e)
{
	whDataMgr::GetDB().RollBack();
	wxLogWarning(wxString(diagnostic_information(e)));
}///catch(boost::exception & e)
catch (...)
{
	wxLogWarning(wxString("������ ���������� ������������"));
}//catch(...)	
}
