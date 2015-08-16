#ifndef __TMODELARRAY_H
#define __TMODELARRAY_H

#include "TModel.h"

namespace wh{

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
		auto child = std::make_shared < T_Item > ();
		child->SetData(T_Item::T_Data());
		return child;
		/*
		auto child = new T_Item;
		T_Item::T_Data data;
		child->SetData(data);
		return std::shared_ptr<IModel>(child);
		*/
	}

	virtual const std::vector<Field>& GetFieldVector()const override
	{
		auto tmpChild = std::make_shared<T_Item>();
		return tmpChild->GetFieldVector();
	}
protected:
	
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t pos)override
	{
		auto childModel = std::dynamic_pointer_cast<T_Item>(child);
		if (childModel)
			return childModel->LoadThisDataFromDb(table, pos);
		return false;
	}

	
};



}//namespace wh{
#endif // __****_H