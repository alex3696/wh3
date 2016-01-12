#ifndef __TPRESENTER_H
#define __TPRESENTER_H

#include "TModelArray.h"
#include "TView.h"


namespace wh{

//-----------------------------------------------------------------------------

class T_AddFunctor
{
public:
	void operator()(std::shared_ptr<IModel>	vecModel,
					T_View&					itemEditor,
					bool mExecSave = 1)const
	{
		auto newItem = vecModel->CreateChild();
		#ifdef __DEBUG
		ModelState vecState;
		vecState = vecModel->GetState();
		#endif
		vecModel->Insert(newItem);
		itemEditor.SetModel(newItem);
		#ifdef __DEBUG
		vecState = vecModel->GetState();
		#endif
		if (wxID_OK == itemEditor.ShowModal())
		{
			itemEditor.UpdateModel();
			#ifdef __DEBUG
			vecState = vecModel->GetState();
			#endif
			if (mExecSave)
				vecModel->Save();
		}//if (wxID_OK == editor.ShowModal())
		else
		{
			auto state = newItem->GetState();
			if (state == msCreated || state == msNull)
				vecModel->DelChild(newItem);
		}//else if (wxID_OK == editor.ShowModal())

		#ifdef __DEBUG
		vecState = vecModel->GetState();
		#endif

	}
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class T_EditFunctor
{
public:
	void operator()(std::shared_ptr<IModel>		vecModel,
					T_View&						itemEditor,
					T_VecView&					vecView,
					bool mExecSave = 1)const
	{
		std::vector<unsigned int> items;
		vecView.GetSelected(items);
		if (!items.empty())
		{
			auto item = vecModel->GetChild(*items.begin());
			itemEditor.SetModel(item);
			if (wxID_OK == itemEditor.ShowModal())
			{
				itemEditor.UpdateModel();
				if (mExecSave)
					vecModel->Save();
			}// if (wxID_OK
		}// if (!items.empty())
	}// void operator()
};
//-----------------------------------------------------------------------------
class T_RemoveFunctor
{
public:
	void operator()(std::shared_ptr<IModel>	vecModel,
		T_VecView&							vecView,
		bool mExecSave = 1)const
	{

		std::vector<unsigned int> selected;
		std::vector<std::shared_ptr<IModel> > toDelete;

		vecView.GetSelected(selected);
		if (!selected.empty())
		{
			int res = wxMessageBox("Вы действительно ходите удалить?",
				"Подтверждение", wxYES_NO);
			if (wxYES != res)
				return;


			for (auto it = selected.begin(); it != selected.end(); ++it)
			{
				auto model = vecModel->GetChild(*it);
				ModelState state = model->GetState();
				switch (state)
				{
				default: case msNull: case msCreated:
					toDelete.emplace_back(model);
					break;
				case msExist: case msUpdated:
					model->MarkDeleted();
				case msDeleted: 
					break;
				} //switch (state)
			}//for 
			if (mExecSave)
				vecModel->Save();
			else
				for (auto item : toDelete)
					vecModel->DelChild(item);


		}// if (!items.empty())
	}// void operator()
};


//-----------------------------------------------------------------------------
template <	typename T_Model >
class T_PresenterEditor
{
	typedef T_View	T_View;
	
	std::shared_ptr<T_Model>	mModel;
	T_View*						mView = nullptr;
	
	sig::scoped_connection				mChangeConnection;
	//const typename T_Model::SlotChange	mChaneSlot;//	const std::function<void(const T_Model&)>

	void Clear()
	{
		mChangeConnection.disconnect();
		mModel = nullptr;
		mView = nullptr;
	}
public:
	T_PresenterEditor(std::shared_ptr<T_Model> model, T_View* view)
		//:mChaneSlot([this](const T_Model& data){ SetModelToView(); })
	{
		Present(model, view);
	}
	
	void Present(std::shared_ptr<T_Model> model, T_View* view)
	{
		Clear();
		mModel = model;
		mView = view;
		if (mModel && mView)
		{
			mView->SetModel(mModel);

			mChangeConnection = mModel->ConnectChangeSlot(
				std::bind(&T_View::OnChangeModel, mView, std::placeholders::_1),
				true);
			
		}
	}

};




}//namespace wh{
#endif // __****_H