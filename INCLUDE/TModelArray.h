#ifndef __TMODELARRAY_H
#define __TMODELARRAY_H

#include "TModel.h"

namespace wh{

//-----------------------------------------------------------------------------

template< typename T_Item >
class TModelArray
	: public IModel
{
public:
	using ItemType = T_Item;

	TModelArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
	:IModel(option)
	{
	}

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		return std::dynamic_pointer_cast<IModel>(MakeItem());
	}
	inline std::shared_ptr<T_Item>  CreateChild()const
	{
		return MakeItem();
	}

	std::shared_ptr<T_Item> at(size_t pos)const
	{
		return std::dynamic_pointer_cast<T_Item>(GetChild(pos));
	}


	virtual const std::vector<Field>& GetFieldVector()const override
	{
		auto tmpChild = std::make_shared<T_Item>();
		return tmpChild->GetFieldVector();
	}

	void DoSigClear()
	{

	}

	void DoSigAppendAll()
	{
		void DoSigClear();

		if (!mVec || mVec->empty())
			return;
		
		std::vector<unsigned int> sigVec;
		for (unsigned int i = 0; i < mVec->size(); ++i)
			sigVec.emplace_back(i);

		DoSigAppendChild(sigVec);
	}
protected:
	
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t pos)override
	{
		auto childModel = std::dynamic_pointer_cast<T_Item>(child);
		return childModel ? childModel->LoadThisDataFromDb(table, pos) : false;
	}

protected:

	std::shared_ptr<T_Item> MakeItem()const
	{
		auto child = std::make_shared < T_Item >();
		child->SetData(T_Item::T_Data());
		return child;
	}
	
};

//-----------------------------------------------------------------------------

}//namespace wh{
#endif // __****_H