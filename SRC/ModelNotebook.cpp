#include "_pch.h"
#include "ModelNotebook.h"
#include "globaldata.h"
#include "config.h"
#include "ReportListPresenter.h"

using namespace wh;


std::shared_ptr<ICtrlWindow> ModelNotebook::MkCtrl(const wxString& name)
{
	namespace ph = std::placeholders;

	auto controller = whDataMgr::GetInstance()->mContainer;
	std::shared_ptr<ICtrlWindow> ctrl
		= controller->GetObject<ICtrlWindow>(name);

	if (!ctrl)
		return ctrl;

	auto& ptrIdx = mWindowList.get<2>();
	auto it = ptrIdx.find(ctrl.get());
	if (ptrIdx.end() != it)
		return ctrl;
	
	auto wnd_model = ctrl->GetModel();
	if (!wnd_model)
	{
		wxLogWarning("can`t create, model is empty/");
		BOOST_THROW_EXCEPTION(error() << wxstr("model is empty"));
	}
		

	auto item = std::make_shared<WindowItem>(ctrl);
	item->connModelChTitle = wnd_model->sigUpdateTitle
		.connect(std::bind(std::ref(sigAfterChWindow), ctrl.get(), ph::_1, ph::_2));
	item->connModelShow = wnd_model->sigShow
		.connect(std::bind(std::ref(sigShowWindow), ctrl.get()));
	item->connModelClose = wnd_model->sigClose
		.connect(std::bind(&ModelNotebook::RmCtrl, this, ctrl.get()));

	ctrl->MkView();
	mWindowList.emplace_back(item);
	sigAfterMkWindow(ctrl.get());
	return ctrl;
}
//-----------------------------------------------------------------------------
void ModelNotebook::MkWindow(const wxString& factory)
{
	auto ctrl = MkCtrl(factory);
	if (ctrl)
	{
		ctrl->Init();
		ctrl->UpdateTitle();
		ctrl->Show();	
	}
}
//-----------------------------------------------------------------------------
void ModelNotebook::RmCtrl(ICtrlWindow* ctrl)
{
	sigBeforeRmWindow(ctrl);
	mWindowList.get<2>().erase(ctrl);

	ctrl->RmView();
	
}
//-----------------------------------------------------------------------------
void ModelNotebook::RmAll()
{
	while (mWindowList.size())
	{
		const auto& item = *mWindowList.cbegin();
		//item->connModelChTitle.disconnect();
		//item->connModelClose.disconnect();
		//item->connModelShow.disconnect();
		auto ctrl = item->mCtrlWindow;
		sigBeforeRmWindow(ctrl.get());
		ctrl->RmView();
		mWindowList.pop_front();
	}

}
//-----------------------------------------------------------------------------
void ModelNotebook::RmWindow(wxWindow* wnd)
{
	auto& wndIdx = mWindowList.get<1>();

	auto it = wndIdx.find(wnd);
	if (wndIdx.end() != it)
	{
		auto ctrl = (*it)->mCtrlWindow;
		RmCtrl(ctrl.get());
	}
	
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

void ModelNotebook::Load(const boost::property_tree::wptree& app_cfg)
{
	TEST_FUNC_TIME;
try{
	using ptree = boost::property_tree::wptree;

	auto it = app_cfg.find(L"CtrlNotebook");
	if (app_cfg.not_found() == it)
		return;
		
	auto notebook= it->second;

	if (notebook.not_found() != notebook.find(L"Pages"))
	{
		for (const ptree::value_type &v : notebook.get_child(L"Pages"))
		{
			ptree::value_type page = *v.second.begin();
			auto name = page.first.c_str();
			//auto name = v.second.get<std::string>("Name", "");
			auto ctrl = MkCtrl(name);
			if (ctrl)
			{
				ctrl->Load(v.second);
				ctrl->Init();
				ctrl->UpdateTitle();
			}
		}//for 
	}
	auto active_page = notebook.get<int>(L"ActivePage", 0);
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
void ModelNotebook::Save(boost::property_tree::wptree& app_cfg)
{
	TEST_FUNC_TIME;
try{
	using ptree = boost::property_tree::wptree;
	
	ptree notebook;
	ptree pages;

	for (const auto& item : mWindowList)
	{
		ptree page;
		auto ctrl = item->mCtrlWindow;
		ctrl->Save(page);
		pages.push_back(std::make_pair(L"", page));
	}

	notebook.put_child(L"Pages", pages);
	notebook.put(L"ActivePage", L"0");
	app_cfg.add_child(L"CtrlNotebook", notebook);

	

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
