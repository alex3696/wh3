#include "_pch.h"
#include "VTable.h"

using namespace wh;
//-----------------------------------------------------------------------------
VTable::VTable(wxWindow*		parent,
		wxWindowID		id ,
		const wxPoint&	pos ,
		const wxSize &	size ,
		long			style )
		: BaseTable(parent, id, pos, size, style)
{
	SetRowHeight(24);
	Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, &VTable::OnColumnHeaderlClick, this);
}
//-----------------------------------------------------------------------------
void VTable::SetModel(std::shared_ptr<ITable> model)
{
	if (model && mModel != model)
	{
		namespace ph = std::placeholders;

		mConnRowAI.disconnect();;
		mConnRowBR.disconnect();;
		mConnRowAR.disconnect();;
		mConnRowAC.disconnect();;

		mConnFieldAI.disconnect();
		mConnFieldBR.disconnect();
		mConnFieldAC.disconnect();

		this->mDataViewModel->Reset(0);
		this->ClearColumns();
		mModel = model;

		auto fnAI = std::bind(&VTable::OnRowAfterInsert, this, ph::_1, ph::_2, ph::_3);
		auto fnBR = std::bind(&VTable::OnRowBeforeRemove, this, ph::_1, ph::_2);
		auto fnAR = std::bind(&VTable::OnRowAfterRemove, this, ph::_1, ph::_2);
		auto fnAC = std::bind(&VTable::OnRowAfterChange, this, ph::_1, ph::_2);
		mConnRowAI = mModel->GetDataArr()->ConnAfterInsert(fnAI);
		mConnRowBR = mModel->GetDataArr()->ConnectBeforeRemove(fnBR);
		mConnRowAR = mModel->GetDataArr()->ConnectAfterRemove(fnAR);
		mConnRowAC = mModel->GetDataArr()->ConnectChangeSlot(fnAC);
		
		auto fnFieldAI = std::bind(&VTable::OnFieldAfterInsert, this, ph::_1, ph::_2, ph::_3);
		auto fnFieldBR = std::bind(&VTable::OnFieldBeforeRemove, this, ph::_1, ph::_2);
		auto fnFieldAC = std::bind(&VTable::OnFieldInfoChange, this, ph::_1, ph::_2);
		mConnFieldAI = mModel->mFieldVec->ConnAfterInsert(fnFieldAI);
		mConnFieldBR = mModel->mFieldVec->ConnectBeforeRemove(fnFieldBR);
		mConnFieldAC = mModel->mFieldVec->ConnectChangeSlot(fnFieldAC);
				
		std::vector<SptrIModel> newItems;
		const auto& fvec = mModel->mFieldVec;
		for (unsigned int i = 0; i < fvec->GetChildQty(); ++i)
			newItems.emplace_back(fvec->at(i));
		OnFieldAfterInsert(*fvec, newItems, nullptr);
				
		OnResize(wxSizeEvent());
		
		//if (0 == mModel->size())
		//	mModel->Load();
		
		mDataViewModel->Reset(mModel->GetDataArr()->GetChildQty());
		OnChangeVecState(mModel->GetState());
	}


};
//-----------------------------------------------------------------------------
void VTable::OnRowAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndLockUpdater(this);

	if (itemBefore)
	{
		size_t pos;
		if (vec.GetItemPosition(itemBefore, pos))
		{
			for (const auto& curr : newItems)
				mDataViewModel->RowInserted(pos++);
		}
	}
	else
	{
		for (const auto& curr : newItems)
			mDataViewModel->RowAppended();
	}
	OnChangeVecState(vec.GetState());
}//OnAppend
//-----------------------------------------------------------------------------
void VTable::OnRowBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndLockUpdater(this);
	
	if (vec.size() != remVec.size() )
	{
		size_t pos;
		wxArrayInt	itemsArray;
		for (const auto& remItem : remVec)
		{
			if (vec.GetItemPosition(remItem, pos))
				itemsArray.push_back(pos);
		}
		mDataViewModel->RowsDeleted(itemsArray);
	}
	else
		mDataViewModel->Reset(0);
}//OnRemove
//-----------------------------------------------------------------------------
void VTable::OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	OnChangeVecState(vec.GetState());
}
//-----------------------------------------------------------------------------
void VTable::OnRowAfterChange(const IModel& newVec, const std::vector<unsigned int>& itemVec)
{
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndLockUpdater(this);

	auto changedQty = itemVec.size();
	bool changeAll = newVec.GetChildQty() == itemVec.size();

	if (!changeAll && changedQty < 100)
	{
		for (const unsigned int& item : itemVec)
			mDataViewModel->RowChanged(item);
	}
	else
	{
		mDataViewModel->Reset(changedQty);
		this->Refresh();
	}
	OnChangeVecState(newVec.GetState());
}//OnChange
//-----------------------------------------------------------------------------
void VTable::OnChangeVecState(ModelState state)
{
	/*
	if (msExist == state)
		this->SetBackgroundColour(wxColour(230, 250, 255));
	else
		this->SetBackgroundColour(wxColour(255, 250, 250));
	*/
}
//-----------------------------------------------------------------------------
bool VTable::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr) const 
{
	if (!mModel)
		return false;
	return mModel->GetAttrByRow(row, col, attr);
}
//-----------------------------------------------------------------------------
void VTable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col) 
{
	if (!mModel)
		return;
	mModel->GetValueByRow(val,row,col);
}
//-----------------------------------------------------------------------------
void VTable::OnFieldAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	if (!mModel)
		return;
	wxWindowUpdateLocker	wndLockUpdater(this);
	const auto& fvec = mModel->mFieldVec;
	for (const auto& new_item : newItems)
	{
		size_t model_idx = -1;
		if (vec.GetItemPosition(new_item, model_idx))
		{
			const auto& field = fvec->at(model_idx)->GetData();
			if (field.mGuiShow)
			{
				wxString name = field.mTitle;
				auto width = GetColumnWidthBy(field.mType);

				//auto col = AppendTextColumn(name, model_idx, wxDATAVIEW_CELL_INERT, width,
				//	wxALIGN_NOT, /*wxDATAVIEW_COL_SORTABLE | */wxDATAVIEW_COL_RESIZABLE);

				class wxDataViewMLTextRenderer : public wxDataViewTextRenderer
				{
				public:
					wxDataViewMLTextRenderer(const wxString &  varianttype = wxT("string"), //GetDefaultType(),
						wxDataViewCellMode  mode = wxDATAVIEW_CELL_INERT,
						int  align = wxDVR_DEFAULT_ALIGNMENT )
						:wxDataViewTextRenderer(varianttype, mode, align)
					{}
					void RenderText(const wxString& text,
						int xoffset,
						wxRect rect,
						wxDC *dc,
						int state) override
					{
						wxCoord widthText, heightText, heightLine;
						dc->GetMultiLineTextExtent(text, &widthText, &heightText, &heightLine); 

						if (heightLine && heightText > heightLine)
						{
							int qty = heightText / heightLine -1;
							int rh = static_cast<VTable*>(GetView())->GetRowHeight();
							int max_offset = (rh - heightLine) / 2;
							int calc_offset = qty* (heightLine / 2);
							int curr_offset = (calc_offset > max_offset) ? max_offset : calc_offset;
							rect.y = rect.y - curr_offset;
							rect.height = heightText;
							rect.width = widthText;
						}
						wxDataViewTextRenderer::RenderText(text, xoffset, rect, dc, state);
						/*
						wxRect rectText = rect;
						rectText.x += xoffset;
						rectText.width -= xoffset;

						// check if we want to ellipsize the text if it doesn't fit
						wxString ellipsizedText;
						if (GetEllipsizeMode() != wxELLIPSIZE_NONE)
						{
							ellipsizedText = wxControl::Ellipsize
								(
								text,
								*dc,
								GetEllipsizeMode(),
								rectText.width,
								wxELLIPSIZE_FLAGS_NONE
								);
						}

						// get the alignment to use
						dc->DrawLabel(ellipsizedText.empty() ? text : ellipsizedText,
							rectText, GetEffectiveAlignment());
						*/
					}
					
				};

				auto renderer = new wxDataViewMLTextRenderer();
				if(FieldType::ftText == field.mType)
					renderer->EnableEllipsize(wxELLIPSIZE_START);

				wxDataViewColumn * col = new wxDataViewColumn(name
				    , renderer
					, model_idx
					, width
					, wxALIGN_NOT
					, wxDATAVIEW_COL_RESIZABLE);
				this->AppendColumn(col);

				auto col_idx = GetColumnIndex(col);

				if (ftText == field.mType)
					EnableAutosizeColumn(col_idx);

				SetGuiColumnSort(field, col);
			}//if (field.mGuiShow)

		}
	}
}
//-----------------------------------------------------------------------------
void VTable::OnFieldBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	if (!mModel)
		return;
	wxWindowUpdateLocker	wndLockUpdater(this);
	for (const auto& field : remVec)
	{
		size_t model_idx = 0;
		if (vec.GetItemPosition(field, model_idx))
		{
			auto col_idx = GetModelColumnIndex(model_idx);
			auto col = GetColumn(col_idx);
			if (col)
				this->DeleteColumn(col);
		}
	}
}
//-----------------------------------------------------------------------------
void VTable::OnFieldInfoChange(const IModel& newVec, const std::vector<unsigned int>& itemVec)
{
	auto fields_array = dynamic_cast<const IFieldArray*>(&newVec);
	if (!fields_array)
		return;
	wxWindowUpdateLocker	wndLockUpdater(this);
	for (const auto& model_idx: itemVec )
	{
		auto field = fields_array->at(model_idx)->GetData();
		auto col_idx = GetModelColumnIndex(model_idx);
		if (-1 == col_idx)
			return;
		wxDataViewColumn* curr_column = GetColumn(col_idx);
		
		SetGuiColumnSort(field, curr_column);
	}//for (const auto& model_idx: itemVec )
	
}
//-----------------------------------------------------------------------------
void VTable::SetGuiColumnSort(const wh::Field& field, wxDataViewColumn* col)
{
	if (!col)
		return;
	if (0 == field.mSort)
	{
		col->SetTitle(field.mTitle);
		col->SetBitmap(wxNullBitmap);
	}
	else
	{
		if (1 < abs(field.mSort))
			col->SetTitle(wxString::Format("(%d)", abs(field.mSort)) + field.mTitle);
		if (0 > field.mSort)
			col->SetBitmap(m_ResMgr->m_ico_sort_desc16);
		else //(0 < field.mSort)
			col->SetBitmap(m_ResMgr->m_ico_sort_asc16);
	}
}
//-----------------------------------------------------------------------------
void VTable::OnColumnHeaderlClick(wxDataViewEvent &event)
{
	if (!mModel)
		return;
	auto clicked_column_no = event.GetColumn();
	auto clicked_column = event.GetDataViewColumn();
	// очистить всю сортировку и сделать сортировку в выбранном столбце

	for (unsigned int i = 0; i < mModel->mFieldVec->GetChildQty(); ++i)
	{
		auto field = mModel->mFieldVec->at(i)->GetData();
		if (i == clicked_column->GetModelColumn())
		{
			switch (field.mSort)
			{
			case -1:	field.mSort = 0;	break;
			case 0:		field.mSort = 1;	break;
			case 1:		field.mSort = -1;	break;
			default:break;
			}
			mModel->mFieldVec->at(i)->SetData(field);
		}
		else if (field.mSort != 0)
		{
			field.mSort = 0;
			mModel->mFieldVec->at(i)->SetData(field);
		}
	}

	//auto itemLimit = mModel->GetClientSize().GetHeight() / mTableView->GetRowHeight() - 1;
	//mModel->mPageLimit->SetData(itemLimit, true);
	mModel->mPageNo->SetData(0, true);
	mModel->Load();
	//OnCmdLoad(wxCommandEvent(wxID_REFRESH));

}
//-----------------------------------------------------------------------------
void VTable::OnResize(wxSizeEvent& evt)
{
	BaseTable::OnResize(evt);
	if (mModel)
	{
		auto client_hight = GetClientSize().GetHeight();
		auto row_hight = GetRowHeight();
		auto itemLimit = client_hight / row_hight - 2;
		if (itemLimit < 5)
			itemLimit = 5;
		mModel->mPageLimit->SetData(itemLimit, true);
	}
}