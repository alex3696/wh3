#ifndef __TVIEWTABLE_H
#define __TVIEWTABLE_H


#include "BaseOkCancelDialog.h"
#include "BaseRefControlPanel.h"
#include "BaseControlPanel.h"
#include "BaseTable.h"

#include "TView.h"

namespace wh{
namespace view{



//-----------------------------------------------------------------------------
//template < typename T_VecModel >
class TViewTable
	: public BaseTable
	, public T_VecView
{
public:
	//typedef T_VecModel						T_VecModel;
	//typedef typename T_VecModel::T_Item		T_Item;

	TViewTable(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 | wxDV_MULTIPLE)
		: BaseTable(parent, id, pos, size, style)
	{
	}

	virtual void SetModel(std::shared_ptr<IModel> model)override
	{
		if (model && mModel != model)
		{
			mConnAppend.disconnect();
			mConnRemove.disconnect();
			mConnChange.disconnect();
			
			this->mDataViewModel->Reset(0);
			this->ClearColumns();



			
			//mModel = std::dynamic_pointer_cast<T_VecModel>(model);
			mModel = model;
			const std::vector<Field>& fvec = mModel->GetFieldVector();

			int colIndex = 1;
			for (const auto& field : fvec)
			{
				if (field.mShow)
				{
					wxString name = field.mName;
					auto width = GetColumnWidthBy(field.mType);

					if (ftText == field.mType)
						EnableAutosizeColumn(colIndex - 1);

					if (1 == colIndex)
						AppendIconTextColumn(name, colIndex++, wxDATAVIEW_CELL_INERT, width,
						wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
					else
						AppendTextColumn(name, colIndex++, wxDATAVIEW_CELL_INERT, width,
						wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);


				}
			}


			//T_Item::T_Data::TableSetupColumn(this);
			//EnableAutosizeColumn(0);
			

			namespace sph = std::placeholders;

			mConnAppend = mModel->ConnectAppendSlot(
				std::bind(&TViewTable::OnAppend, this, sph::_1, sph::_2) );
			mConnRemove = mModel->ConnectRemoveSlot(
				std::bind(&TViewTable::OnRemove, this, sph::_1, sph::_2));
			mConnChange = mModel->ConnectChangeSlot(
				std::bind(&TViewTable::OnChange, this, sph::_1, sph::_2));

			mDataViewModel->Reset(mModel->GetChildQty());
			OnChangeVecState(mModel->GetState());
		}


	};


	virtual void OnAppend(const IModel& newVec,
		const std::vector<unsigned int>& itemVec)override
	{
		if (itemVec.size() > 100)
			mDataViewModel->Reset(itemVec.size());
		else
			for (size_t i = 0; i < itemVec.size(); i++)
				mDataViewModel->RowAppended();
		OnChangeVecState(newVec.GetState());
	}//OnAppend
	virtual void OnRemove(const IModel& newVec,
		const std::vector<unsigned int>& itemVec)override
	{
		auto removeQty = itemVec.size();

		//GetChildQty- новый размер(после удаления элемента/элементов)
		bool removeAll = newVec.GetChildQty() == 0 ; 

		if (!removeAll && removeQty<100)
		{
			wxArrayInt	itemsArray;
			for (const unsigned int& item : itemVec)
				itemsArray.push_back(item);
			mDataViewModel->RowsDeleted(itemsArray);
		}
		else
			mDataViewModel->Reset(0);
		OnChangeVecState(newVec.GetState());
	}//OnRemove
	virtual void OnChange(const IModel& newVec,
		const std::vector<unsigned int>& itemVec)override
	{
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

	virtual void GetSelected(std::vector<unsigned int>& selected)override
	{
		wxDataViewItemArray viewSelArr;
		this->GetSelections(viewSelArr);
		for (const wxDataViewItem& viewItem : viewSelArr)
		{
			if (viewItem.IsOk())
			{
				unsigned int pos = GetRow(viewItem);
				selected.emplace_back(pos);
			}
		}
	}

	void OnChangeVecState(ModelState state)
	{
		if (msExist == state)
			this->SetBackgroundColour(wxColour(240, 255, 245));
		else
			this->SetBackgroundColour(wxColour(255, 250, 250));
	}

protected:
	std::shared_ptr<IModel>		mModel;
	sig::scoped_connection		mConnAppend;
	sig::scoped_connection		mConnRemove;
	sig::scoped_connection		mConnChange;



	virtual bool GetAttrByRow(unsigned int row, unsigned int WXUNUSED(col), 
		wxDataViewItemAttr &attr) const override
	{
		if (mModel)
		{
			const ModelState state = mModel->GetChild(row)->GetState();
			switch (state)
			{
				//msNull
			default:  break;
			case msCreated:
				attr.SetBold(true);
				attr.SetColour(*wxBLUE);
				break;
			case msExist:
				attr.SetBold(false);
				attr.SetColour(*wxBLACK);
				break;
			case msUpdated:
				attr.SetBold(true);
				attr.SetColour(wxColour(128, 64, 0));
				break;
			case msDeleted:
				attr.SetBold(true);
				attr.SetColour(*wxRED);
				break;
			}//switch
			return true;
		}

		return false;
	}
	virtual void GetValueByRow(wxVariant& val, 
		unsigned int row, unsigned int col) override final
	{
		bool resulOk = false;
		if (mModel)
		{
			std::shared_ptr<IModel> child = mModel->GetChild(row);

			try
			{
				resulOk = child->GetFieldValue(col, val);
			}
			catch (...)
			{
				resulOk = false;
			}
		}
		
		if (!resulOk)
		{
			if (1 == col)
				val << wxDataViewIconText(L"**error**");
			else
				val << wxDataViewIconText(wxEmptyString);
		}
		
			
	}


};
//-----------------------------------------------------------------------------






}//namespace view
}//namespace wh
#endif //__*_H