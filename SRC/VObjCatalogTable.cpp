#include "_pch.h"
#include "VObjCatalogTable.h"




using namespace wh;
using namespace view;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VObjCatalogTable::VObjCatalogTable(wxWindow*		parent,
	wxWindowID		id,
	const wxPoint&	pos,
	const wxSize &	size,
	long			style)
	: wxDataViewCtrl(parent, id, pos, size, style 
	/*| wxDV_ROW_LINES*/ | wxDV_VERT_RULES | wxDV_HORIZ_RULES)
{
	this->SetRowHeight(26);

	mDataViewModel = new VObjCatalogDataViewModel();
	wxDataViewCtrl::AssociateModel(mDataViewModel);
	mDataViewModel->DecRef();


	Bind(wxEVT_SIZE, &VObjCatalogTable::OnResize, this);

	this->SetBackgroundColour(wxColour(245, 245, 245));


	AppendIconTextColumn("���", 1, wxDATAVIEW_CELL_INERT, 300,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("����������", 2, wxDATAVIEW_CELL_INERT, 80,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("ID", 3, wxDATAVIEW_CELL_INERT, 50,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("PID", 4, wxDATAVIEW_CELL_INERT, 50,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("LastLogId", 5, wxDATAVIEW_CELL_INERT, 100,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	auto col = AppendTextColumn("��������������", 6, wxDATAVIEW_CELL_INERT, 100,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	col->GetRenderer()->EnableEllipsize(wxELLIPSIZE_START);

}

//---------------------------------------------------------------------------
void VObjCatalogTable::SetModel(std::shared_ptr<IModel> model)
{
	if (model && mCatalogModel != model)
	{
		namespace sph = std::placeholders;

		mConnAppend.disconnect();
		mConnRemove.disconnect();
		mConnChange.disconnect();

		//this->mDataViewModel->Cleared();
		//this->ClearColumns();
		mCatalogModel = std::dynamic_pointer_cast<wh::object_catalog::MObjCatalog>(model);
		mDataViewModel->SetModel(model);
		
		auto funcOnChange = std::bind(&VObjCatalogTable::OnAfterUpdateFavProp,
			this, std::placeholders::_1, std::placeholders::_2);


		mCatalogUpdate 
			= mCatalogModel->mFavProps->DoConnect(moAfterUpdate, funcOnChange);

		/*
		mConnAppend = mCatalogModel->mTypeArray->ConnectAppendSlot(
			std::bind(&VObjCatalogTable::OnAppend, this, sph::_1, sph::_2));
		mConnRemove = mCatalogModel->mTypeArray->ConnectRemoveSlot(
			std::bind(&VObjCatalogTable::OnRemove, this, sph::_1, sph::_2));
		mConnChange = mCatalogModel->mTypeArray->ConnectChangeSlot(
			std::bind(&VObjCatalogTable::OnChange, this, sph::_1, sph::_2));

		*/
		
		auto typeQty = mCatalogModel->mTypeArray->GetChildQty();
		std::vector<unsigned int> addedItems(typeQty);
		for (unsigned int i = 0; i < typeQty; i++)
			addedItems[i] = i;

		OnAfterUpdateFavProp(mCatalogModel->mFavProps.get(), nullptr);
		OnAppend(*model, addedItems);
		
		//mDataViewModel->Reset();
		
	}
}
//---------------------------------------------------------------------------
void VObjCatalogTable::OnAfterUpdateFavProp(const IModel* model,
	const object_catalog::MFavProp::T_Data* data)
{
	ResetColumns();
	BuildColumns();
}

//---------------------------------------------------------------------------
void VObjCatalogTable::ResetColumns()
{
	while (this->GetColumnCount() > 6)
	{
		auto column = GetColumn(6);
		DeleteColumn(column);
	}

	if (mCatalogModel)
	{
		bool hidden = !mCatalogModel->GetData().mShowDebugColumns;
		GetColumn(2)->SetHidden(hidden);
		GetColumn(3)->SetHidden(hidden);
		GetColumn(4)->SetHidden(hidden);

		if (mCatalogModel->IsObjTree())
		{
			GetColumn(5)->SetHidden(true);
			this->DisableAutosizeColumn(5);
		}
		else
		{
			GetColumn(5)->SetHidden(false);
			EnableAutosizeColumn(5);
		}
			
	}
}
//---------------------------------------------------------------------------
void VObjCatalogTable::BuildColumns()
{
	

	if (msNull != mCatalogModel->mFavProps->GetState())
	{
		int colIndex = 7;
		for (const auto& prop : mCatalogModel->GetFavProps())
		{
			wxString name = prop.mLabel;
			unsigned int width = 50;

			auto fieldType = ToFieldType(prop.mType);
			if (ftText != fieldType)
			{
				wxWindowDC dc(this);
				auto dcWidth = dc.GetTextExtent(name).GetWidth() + 20;
				auto typeWidth = GetColumnWidthBy(fieldType);
				//width = (dcWidth > typeWidth && dcWidth < typeWidth*3) ? dcWidth : typeWidth;
				width = (typeWidth * 3 - dcWidth > 0) ? dcWidth : typeWidth;
			}
			else
				EnableAutosizeColumn(colIndex - 1);

			AppendTextColumn(name, colIndex++, wxDATAVIEW_CELL_INERT, width,
				wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
		}
	}
	
	this->OnResize(wxSizeEvent());

}


//---------------------------------------------------------------------------
void VObjCatalogTable::RebuildColumns()
{
	ResetColumns();
	BuildColumns();
}

//---------------------------------------------------------------------------
void VObjCatalogTable::GetSelected(std::vector<unsigned int>& selected)
{
	wxDataViewItemArray viewSelArr;
	this->GetSelections(viewSelArr);
	for (const wxDataViewItem& viewItem : viewSelArr)
	{
		if (viewItem.IsOk())
		{
			unsigned int pos = this->GetRowByItem(viewItem);
			selected.emplace_back(pos);
		}
	}
}
//---------------------------------------------------------------------------
void VObjCatalogTable::OnAppend(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	for (const unsigned int& i : itemVec)
	{
		auto typeModel = std::dynamic_pointer_cast<object_catalog::MTypeItem>
			(mCatalogModel->mTypeArray->GetChild(i));
		
		wxDataViewItem typeItem(typeModel.get());
		mDataViewModel->ItemAdded(wxDataViewItem(NULL), typeItem);

		auto objArray = typeModel->mObjArray;
		wxDataViewItemArray objItemArray;
		for (unsigned int j = 0; j < objArray->GetChildQty(); ++j)
		{
			wxDataViewItem objItem(objArray->GetChild(j).get());
			objItemArray.Add(objItem);
		}
		mDataViewModel->ItemsAdded(typeItem, objItemArray);

		this->Expand(typeItem);

	}

	this->Select(wxDataViewItem(nullptr));

}//OnAppend
//---------------------------------------------------------------------------
void VObjCatalogTable::OnRemove(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = mCatalogModel->mTypeArray->GetChild(i);
		itemArray.Add(wxDataViewItem(item.get()));
	}
	this->mDataViewModel->ItemsDeleted(wxDataViewItem(NULL), itemArray);


}//OnRemove
//---------------------------------------------------------------------------
void VObjCatalogTable::OnChange(const IModel& newVec,
	const std::vector<unsigned int>& itemVec)
{
	wxDataViewItemArray itemArray;
	for (const unsigned int& i : itemVec)
	{
		auto item = mCatalogModel->mTypeArray->GetChild(i);
		itemArray.Add(wxDataViewItem(item.get()));
	}
	this->mDataViewModel->ItemsChanged(itemArray);

}//OnChange

//---------------------------------------------------------------------------
void VObjCatalogTable::ExpandAll()
{
	wxDataViewItemArray itemArray;
	mDataViewModel->GetChildren(wxDataViewItem(NULL), itemArray);

	//std::for_each(itemArray.begin(), itemArray.end(),
	//	std::bind(&VObjCatalogTable::Expand, this, std::placeholders::_1) );
	
	//for (const auto& item : itemArray)
	//	Expand(item);

}

//---------------------------------------------------------------------------
void VObjCatalogTable::OnResize(wxSizeEvent& evt)
{
	if (!mAutosizeColumn.empty())
	{
		wxWindowUpdateLocker  wndStopRefresh(this);
		//this->Freeze();
		int sum = 0, asum = 0;
		for (unsigned int i = 0; i<GetColumnCount(); i++)
			if (mAutosizeColumn.end() == mAutosizeColumn.find(i))
				sum += GetColumn(i)->IsHidden() ? 0 : GetColumn(i)->GetWidth();
			else
				asum = wxDVC_DEFAULT_WIDTH;

		int ctrl_width = this->GetClientSize().GetWidth();
		int auto_width = (ctrl_width >(sum + asum)) ? ((ctrl_width - sum) / mAutosizeColumn.size()) : wxDVC_DEFAULT_WIDTH;

		for (auto it = mAutosizeColumn.begin(); it != mAutosizeColumn.end(); ++it) {
			if (*it < GetColumnCount())
				GetColumn(*it)->SetWidth(auto_width);
		}
		//this->Thaw();
	}//if ( mAutosizeColumn.size() )
	evt.Skip();
}//virtual void OnResize

//---------------------------------------------------------------------------
void VObjCatalogTable::EnableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.insert(columnNo);
}
//---------------------------------------------------------------------------
void VObjCatalogTable::DisableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.erase(columnNo);
}
//---------------------------------------------------------------------------
void VObjCatalogTable::SetMultiselect(bool enabled)
{
	long currentFlags = this->GetWindowStyleFlag();
	currentFlags = enabled ? currentFlags | wxDV_MULTIPLE : currentFlags & ~wxDV_MULTIPLE;
	this->SetWindowStyleFlag(currentFlags);
}
