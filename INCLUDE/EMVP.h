#ifndef __EMVP_H
#define __EMVP_H

#include "_pch.h"

#include "EVENT.h"

namespace wh{

//---------------------------------------------------------------------------
/// базовый класс для любого объекта сохраняемого в базе

class IDbObject
	: public boost::noncopyable
{
public:
	struct error:	virtual exception_base {};
	virtual ~IDbObject(){}
	virtual  ModelState	GetState()const =0;
	virtual void SetState( ModelState state)=0;
	virtual void	ExecLoad()=0;
	virtual void	ExecSave()=0;
};

//---------------------------------------------------------------------------
template < class t_Data > 
class IDbRec
	: public IDbObject
{
public:
	virtual t_Data	GetData()const=0;
	virtual void	SetData(const t_Data& data)=0;
};//IDbRecord










//---------------------------------------------------------------------------0,
/// базовый класс для любого объекта сохраняемого в базе
class DbObject
	: public IDbObject
{
	 ModelState			mState;
public:
	DbObject(  ModelState state= msNull )
		:mState(state)
	{}
	virtual  ModelState	GetState()const	override
	{	
		return mState;	
	}
	virtual void SetState( ModelState state)override
	{	
		mState=state;	
	}
};

//---------------------------------------------------------------------------


template < class t_Data > 
class IDbRecord
	: virtual public IDbRec<t_Data>
{
private:
	 ModelState			mState;
	t_Data					mData;
public:
	IDbRecord (const t_Data& data=t_Data(),  ModelState state= msNull )
		: mState(state),
		  mData(data)
	{}

	virtual t_Data	GetData()const override final///Получить свойство модели
	{
		return mData;
	}//GetProperty
	virtual void SetData(const t_Data& data)  override final
	{
		switch( GetState() ) // обновляем состояние элемента
		{
		default:
			BOOST_THROW_EXCEPTION( error()<<wxstr("unknown model state") );
			break;
		case  msExist:
			SetState( msUpdated);
		case  msCreated:
		case  msUpdated:
			mData = data;
			break;
		case  msDeleted:
			break;
		}//switch				
	}//SetProperty
	void	Copy(const IDbRecord* model)
	{
		SetData ( model->GetData() );
		SetState( model->GetState());
	}//copy
	virtual IDbRecord*	Clone()const
	{
		auto clone_model = new  IDbRecord;
		clone_model->Copy(this);
		return clone_model;
	}//clone

	virtual void	ExecLoad()override
	{
	}
	virtual void	ExecSave()override
	{
	}

	virtual  ModelState	GetState()const	override final
	{	
		return mState;	
	}
	virtual void SetState( ModelState state)override final
	{	
		mState=state;	
	}
};//IDbRecord


//---------------------------------------------------------------------------
template < class t_Data > 
class IDbRecordVec
	: public DbObject
{
	typedef IDbRec<t_Data>	t_ItemModel;
	std::vector < std::unique_ptr<t_ItemModel> >	mVec;

	inline void CheckIndex(unsigned int row)const 
	{
		if( mVec.size() <= row )
			BOOST_THROW_EXCEPTION( error()<<wxstr("row > size") );
	}
public:
	IDbRecordVec( ModelState state= msCreated )
		: DbObject(state)
	{}
	
	virtual IDbRec<t_Data>* Make(const t_Data& newData=t_Data(),  ModelState newState= msCreated)const 
	{
		return new IDbRecord<t_Data>(newData,newState);
	}

	 ModelState	GetState()const 
	{
		return 	DbObject::GetState();
	}
	void 	SetState( ModelState state) 
	{
		DbObject::SetState(state);
	}
	
	// по аналогии с IDbRecord только для каждого элемента
	 ModelState	GetState(unsigned int row)
	{
		CheckIndex(row);
		return mVec.at(row)->GetState();
	}
	void			SetState(unsigned int row,  ModelState state )
	{
		CheckIndex(row);
		 ModelState vecState =  DbObject::GetState();
		switch( vecState )
		{
		default:
			BOOST_THROW_EXCEPTION( error()<<wxstr("unknown model state") );
			break;
		case  msExist:
			DbObject::SetState( msUpdated);
		case  msCreated:
		case  msUpdated:
			mVec.at(row)->SetState(state);
		case  msDeleted:
			break;
		}//switch
		if(  msExist==state && vecState== msUpdated )
		{
			// пересчитать элементы возможно состояние вектора тоже станет Exist
			auto it=mVec.begin();
			 ModelState currState = (*it)->GetState();
			while(it!=mVec.end() && currState== msExist)
			{
				currState = (*it)->GetState();
				++it;
			}//while
			if(currState== msExist)
				DbObject::SetState( msExist);

		}
	}//SetState( ModelState state, unsigned int row)
	t_Data				GetData (unsigned int row)
	{
		CheckIndex(row);
		return mVec.at(row)->GetData();
	}	
	 ModelState		SetData( unsigned int row, const t_Data& data )
	{
		CheckIndex(row);
		const std::unique_ptr<t_ItemModel>&  item = mVec.at(row);
		
		// если состояние вектора "сущствует" - обновляем данные
		 ModelState vecState = DbObject::GetState();
		if( vecState== msExist || vecState== msUpdated || vecState== msCreated )
			item->SetData(data);
		
		if( vecState== msExist && item->GetState()!= msExist) 
			DbObject::SetState( msUpdated);
		
		return item->GetState();
	}
	
	void Reserve(size_t n)
	{
		mVec.reserve(n);
	}

	 ModelState		Append( const t_Data& newData=t_Data(),  ModelState newState= msCreated/*, bool appendToBack=true*/)
	{
		 ModelState newItemState =  msNull;
		 ModelState vecState = DbObject::GetState();

		t_ItemModel* newItemModel(nullptr);
		switch( vecState )
		{
		default:
			BOOST_THROW_EXCEPTION( error()<<wxstr("unknown model state") );
			break;
		case  msExist:
			if(  msExist != newState )
				DbObject::SetState( msUpdated);
		case  msUpdated:
		case  msCreated:
			newItemModel = Make(newData,newState);
			mVec.emplace_back( newItemModel ) ;
			//if(appendToBack)
				//mVec.push_back( std::unique_ptr<t_ItemModel>( newItemModel ) );
			//else
				//mVec.push_front( std::unique_ptr<t_ItemModel>( newItemModel ) );
			newItemState = newState;
			break;
		case  msDeleted:
			break;
		}		
		return newItemState;
	}//Append
	 ModelState		Delete( unsigned int row)
	{
		 ModelState vecState =  DbObject::GetState();
		if( vecState!= msExist && vecState!= msUpdated && vecState!= msCreated )
			return  msDeleted;
		CheckIndex(row);

		 ModelState itemState = mVec.at(row)->GetState();
		if (  msCreated == itemState )
		{
			mVec.erase( mVec.begin()+row);

			// пересчитать элементы возможно состояние вектора тоже станет Exist
			 ModelState currState =  msExist;

			auto it=mVec.begin();
			while(it!=mVec.end() && currState== msExist)
			{
				currState = (*it)->GetState();
				++it;
			}//while

			if(currState== msExist)
				DbObject::SetState( msExist);	
			itemState =  msNull;
		}
		else if (  msExist==itemState ||  msUpdated==itemState )
		{
			SetState(row,  msDeleted);
			itemState =  msDeleted;
		}
		return itemState;
	}//Delete
	
	unsigned int GetSize()const
	{
		return mVec.size();
	}
	void Clear()
	{
		mVec.clear();
	}

	void	Copy(const IDbRecordVec* model)
	{
		/*
		for( auto i=0; i< model->GetSize(); ++i)
		{
			auto data = model->GetData(i);
			auto state = model->GetState(i);
			auto clodedModel = this->Make(data, state);
			mVec.push_back( std::unique_ptr<t_ItemModel>( clodedModel ) );
		}
		*/
	}//copy
	virtual IDbRecordVec*	Clone()const
	{
		auto clone_model = new  IDbRecordVec( DbObject::GetState() );
		clone_model->Copy(this);
		return clone_model;
	}//clone

	virtual void	ExecLoad(unsigned int row)
	{
		CheckIndex(row);
		auto currentIterator = mVec.begin()+row;
		const std::unique_ptr< t_ItemModel >& currentObj = *currentIterator;
		currentObj->ExecLoad();
	}

	virtual void	ExecLoad()override
	{
	}

	virtual void	ExecSave(unsigned int row)
	{
		CheckIndex(row);

		auto currentIterator = mVec.begin()+row;
		const std::unique_ptr< t_ItemModel >& currentObj = *currentIterator;
		currentObj->ExecSave();
			if( currentObj->GetState() ==  msNull )
				currentIterator = mVec.erase( currentIterator ); 
	}

	virtual void	ExecSave()override
	{
		const  ModelState vecState =  DbObject::GetState();
		if( vecState== msCreated || vecState== msUpdated )
		{
			auto currentIterator = mVec.begin();
			while( currentIterator != mVec.end() )
			{
				const std::unique_ptr< t_ItemModel >& currentObj = *currentIterator;
				currentObj->ExecSave();
				if( currentObj->GetState() ==  msNull )
					currentIterator = mVec.erase( currentIterator ); 
				else
					++currentIterator;
			}//while
			DbObject::SetState( msExist);
		}//if( GetState()== msCreated || GetState()== msExist )
		
	}


};//IDbRecordSet

//---------------------------------------------------------------------------
/// состояние модели
/*
enum  ModelState
{
	 msNull = 0,
	msCreated,	//NEW созданная(новая) модель
	msExist,	//модель, данные которой синхронизированы с хранилищем
	msUpdated,	//измененная модель
	msDeleted,	//удаленная модель

};
*/
//using namespace mvc;


//---------------------------------------------------------------------------
/// события БД модели
enum ModelEvent
{
	meSetData	= 11,
	meGetData	= 12,
	meSetState	= 21,
	meGetState	= 22,
	meSave		= 31,
	meLoad		= 32,
/// события вектора БД модели
	meSetItemData	= 41,
	meGetItemData	= 42,
	meSetItemState	= 51,
	meGetItemState	= 52,
	meAppendItem	= 61,
	meDeleteItem	= 71,

	meUpdateItems	= 81,
	meGetSize		= 91,

	meGetSelected		= 101,

	
	meSetDataItems		= 11,
	meGetDataItems		= 12,
	meSetStateItems		= 21,
	meGetStateItems		= 22,

	meCmdUpdateData		= 201,
	meCmdUpdateState    = 202,
	meCmdUpdateItemData	= 203,
	meCmdUpdateItemState= 204,

	meCmdUpdateView		= 301,
	meCmdUpdateModel	= 302,

};

//---------------------------------------------------------------------------
// базовый класс событий ModelAction
template < ModelEvent EventType > 
class whModelEvent :public whEvent
{
public:
	virtual unsigned int GetEventType()const	{	return EventType;	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
typedef whModelEvent<meSave>	whEvtModelSave;
//---------------------------------------------------------------------------
///
typedef whModelEvent<meLoad>	whEvtModelLoad;
//---------------------------------------------------------------------------
///
template < typename t_Data>
class whEvtSetData
	: public whModelEvent<meSetData>
{
	const t_Data	mData;
public:
	whEvtSetData(const t_Data& data=t_Data() )	:mData(data)	{}
	const t_Data&	GetData()const		{	return mData;	}
};
//---------------------------------------------------------------------------
///
template < typename t_Data>
class whEvtGetData
	:public whModelEvent<meGetData>
{
	t_Data	mData;
public:
	whEvtGetData(const t_Data& data=t_Data() )	:mData(data)	{}
	void SetData(const t_Data& data)	{	mData = data;	}
	const t_Data&	GetData()const		{	return mData;	}

};
//---------------------------------------------------------------------------
///
class whEvtSetState
	:public whModelEvent<meSetState>
{
	 ModelState	mState;
public:
	whEvtSetState( ModelState state= msNull)		:mState(state) {}
	 ModelState GetState()const 					{ return mState;	}
};
//---------------------------------------------------------------------------
///
class whEvtGetState
	:public whModelEvent<meGetState>
{
	 ModelState	mState;
public:
	whEvtGetState( ModelState state= msNull)		:mState(state) {}
	void SetState( ModelState state)		{ mState = state;	}
	 ModelState GetState()const 			{ return mState;	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
///
template < typename t_Data>
class whEvtAppendItem
	: public whModelEvent<meAppendItem>
{
	t_Data			mData;
	 ModelState	mState;
public:
	whEvtAppendItem(const t_Data& data,  ModelState state =  msCreated)
		:mData(data),mState(state)
	{}
	const t_Data&	GetData()const		{ return mData;	}
	 ModelState GetState()const 	{ return mState;	}
};
//---------------------------------------------------------------------------
///
class whEvtDeleteItem
	: public whModelEvent<meDeleteItem>
{
	unsigned int	mRow;
public:
	whEvtDeleteItem(unsigned int row=0)			:mRow(row)	{}
	unsigned int	GetIndex()const 			{ return mRow;	}
};
//---------------------------------------------------------------------------
///
template < typename t_Data>
class whEvtSetItemData
	:public whModelEvent<meSetItemData>
{
	unsigned int	mRow;
	t_Data			mData;
public:
	whEvtSetItemData(const t_Data& data = t_Data(),unsigned int row = 0)
		: mRow(row), mData(data)
	{}
	unsigned int	GetIndex()const 	{ return mRow;	}
	const t_Data&	GetData()const		{ return mData;	}
};
//---------------------------------------------------------------------------
///
template < typename t_Data>
class whEvtGetItemData
	:public whModelEvent<meGetItemData>
{
	unsigned int	mRow;
	t_Data			mData;
public:
	whEvtGetItemData(unsigned int row = 0, const t_Data& data = t_Data() )
		: mRow(row), mData(data)
	{}
	
	const t_Data&	GetData()const				{ return mData;	}
	unsigned int	GetIndex()const 			{ return mRow;	}
	
	void SetData(const t_Data& data)			{ mData = data;	}
	void			SetIndex(unsigned int row)	{ mRow = row;	}

};
//---------------------------------------------------------------------------
///
class whEvtSetItemState
	:public whModelEvent<meSetItemState>
{
	 ModelState	mState;
	unsigned int	mRow;
public:
	whEvtSetItemState( ModelState state =  msNull ,unsigned int row = 0)
		:mState(state),  mRow(row)
	{}
	 ModelState GetState()const 			{ return mState;	}
	unsigned int	GetIndex()const 			{ return mRow;	}

};
//---------------------------------------------------------------------------
///
class whEvtGetItemState
	:public whModelEvent<meGetItemState>
{
	 ModelState	mState;
	unsigned int	mRow;
public:
	whEvtGetItemState(unsigned int row = 0,  ModelState state =  msNull )
		:mState(state),  mRow(row)
	{}

	void SetState( ModelState state)		{ mState = state;	}
	 ModelState GetState()const 			{ return mState;	}
	void			SetIndex(unsigned int row)	{ mRow = row;	}
	unsigned int	GetIndex()const 			{ return mRow;	}


};
//---------------------------------------------------------------------------
///
class whEvtGetSize
	:public whModelEvent<meGetSize>
{
	unsigned int	mSize;
public:
	whEvtGetSize(unsigned int size=0)			:mSize(size) {}
	void			SetSize(unsigned int size)	{ mSize = size;	}
	unsigned int	GetSize()const 				{ return mSize;	}
};
//---------------------------------------------------------------------------
///
class whEvtGetSelected
	:public whModelEvent<meGetSelected>
{
	std::vector<unsigned int>&	mSelected;
public:
	whEvtGetSelected(std::vector<unsigned int>& selected)				:mSelected(selected) {}
	void			SetSelected(std::vector<unsigned int>& selected)	{ mSelected = selected;	}
	std::vector<unsigned int>& GetSelected()const 						{ return mSelected;	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
///
class IEventView
	: public IHandler //view handler
{
public:
	virtual void		SetModelHandler(IHandler* handler)=0;
};



//---------------------------------------------------------------------------
///
class EView
	: public IEventView
{
public:
	EView(IHandler* modelHandler=nullptr )
		:mModelHandler(modelHandler)
	{
	}

	virtual void SetModelHandler(IHandler* handler) override
	{
		mModelHandler = handler;
	}

protected:
	IHandler*		mModelHandler;

	MultiHandler	mViewHandler;
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override 
	{
		mViewHandler.DoProcessEvent(eventType,evt);
	}
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
		mViewHandler.DoAddHandler(eventType,functor);
	}




};


//---------------------------------------------------------------------------
///
class EMultiView
	: public IEventView
{
protected:
	typedef std::set<IEventView*>	IEventViewSet;

	IHandler*		mModelHandler;
	IEventViewSet	mViewSet;
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override 
	{
		for(auto it = mViewSet.begin(); it != mViewSet.end(); ++it)
			(*it)->DoProcessEvent(eventType,evt);
	}
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
		
	}
public:
	EMultiView(IHandler* modelHandler=nullptr)
		:mModelHandler(modelHandler)
	{
	}

	void  AppendView( IEventView* view)
	{
		if(view)
		{
			mViewSet.insert(view);
			view->SetModelHandler(mModelHandler);
		}
	}
	void  RemoveView( IEventView* view)
	{
		mViewSet.erase(view);
	}

	virtual void		SetModelHandler(IHandler* handler) override
	{
		mModelHandler = handler;
		for(auto it = mViewSet.begin(); it != mViewSet.end(); ++it)
			(*it)->SetModelHandler(mModelHandler);
	}




};

//---------------------------------------------------------------------------
class IEventPresenter
	: public IHandler //presenter handler
{
public:
	virtual void SetViewHandler(IHandler* handler)=0;

};
//---------------------------------------------------------------------------
///
template < typename t_Data > 
class EPresenter
	:public IEventPresenter
{
protected:
	virtual void SetViewHandler(IHandler* handler) override
	{
		mViewHandler = handler;
	}

public:
	struct error:	virtual exception_base {};


	EPresenter( IDbRecord<t_Data>* model=nullptr, IEventView* view=nullptr)
		:IEventPresenter()
		,mModel(model)
		,mViewHandler(nullptr)
	{
		// события-уведомления изменяющие модель
		Bind( (boost::function<void(whEvtSetData<t_Data>*)> )
			[this](whEvtSetData<t_Data>* evt)
			{ 
				SetModelData( evt->GetData() ); 
			});
		Bind( (boost::function<void(whEvtSetState*)> )
			[this](whEvtSetState* evt)			
			{
				SetModelState( evt->GetState() ); 
			});
		Bind( (boost::function<void(whEvtModelLoad*)> )
			[this](whEvtModelLoad* evt)			
		{
			LoadModel();
		});
		Bind( (boost::function<void(whEvtModelSave*)> )
			[this](whEvtModelSave* evt)			
		{
			SaveModel();
		});		
		// события-запросы получающие данные из модели немедленно
		// необходимы для запроса данных из вьюшки в основном для контролов типа wxDataView
		Bind( (boost::function<void(whEvtGetData<t_Data>*)> )
			[this](whEvtGetData<t_Data>* evt)			
		{
			if(evt && mModel)
				evt->SetData( mModel->GetData() );
		});		
		Bind( (boost::function<void(whEvtGetState*)>)
			[this](whEvtGetState* evt)			
		{
			if(evt && mModel)
				evt->SetState( mModel->GetState() );
		});		

		SetView( view );
		SetModel( model );		
	}

	// функционал привязки
	void SetView( IEventView* view)
	{
		if(view)
			view->SetModelHandler(this);
		SetViewHandler( view );
		
		SetModelDataToView();	// уведомляем о изменении данных
		SetModelStateToView();	// уведомляем о изменении статуса
	}
	void SetModel(IDbRecord<t_Data>* model)
	{
		mModel = model;
		SetModelDataToView();	// уведомляем о изменении данных
		SetModelStateToView();	// уведомляем о изменении статуса
	}

	// функционал модели
	bool SetModelData(const t_Data& data)
	{
		bool result = false;
		if(mModel)
		{
			mModel->SetData(data);
			SetModelDataToView();
			result = true;
		}
		return result;	
	}
	bool SetModelState(const  ModelState state)
	{
		bool result = false;
		if(mModel)
		{
			mModel->SetState(state);
			SetModelStateToView();
			result = true;
		}
		return result;		
	}	
	
	bool GetModelData(t_Data& data) const
	{
		bool result = false;
		if(mModel)
		{
			data = mModel->GetData();
			result = true;
		}
		return result;
	}
	bool GetModelState( ModelState& state) const
	{
		bool result = false;
		if(mModel)
		{
			state = mModel->GetState();
			result = true;
		}
		return result;	
	}

	void LoadModel()
	{
		if( mModel )
		{
			mModel->ExecLoad();
			SetModelDataToView();
			SetModelStateToView();	
		}
	}
	void SaveModel()
	{
		if( mModel )
		{
			mModel->ExecSave();
			SetModelStateToView();	
		}	
	}


	// уведомления для обновления вьюшки
	void SetModelDataToView()	// уведомляем вьюшку о изменении данных
	{
		if(mModel && mViewHandler)
		{
			whEvtSetData<t_Data> evt( mModel->GetData() );
			mViewHandler->ProcessEvent(	&evt );
		}
	}//SetModelDataToView
	void SetModelStateToView()	// уведомляем вьюшку о изменении статуса
	{
		if(mModel && mViewHandler)
		{
			whEvtSetState	evt( mModel->GetState() );
			mViewHandler->ProcessEvent(	&evt );
		}	
	}//SetModelStateToView
	
	// запросы к вьюшке на изменение данных
	void GetModelDataFromView()
	{
		if(mModel && mViewHandler)
		{
			whEvtGetData<t_Data> evt(mModel->GetData());
			mViewHandler->ProcessEvent(	&evt );		// запрос данных с вьюшки
			mModel->SetData( evt.GetData() );	// установка данных с вьюшки в модель
			SetModelDataToView();					// уведомляем о изменении данных
		}	
	}
	void GetModelStateFromView()
	{
		if(mModel && mViewHandler)
		{
			whEvtGetState	evt(mModel->GetState());
			mViewHandler->ProcessEvent(	&evt );	// запрос статуса с вьюшки
			mModel->SetState( evt.GetState() );	// установка статуса с вьюшки в модель
			SetModelStateToView();				// уведомляем о изменении статуса
		}		
	}
	


private:
	IDbRecord<t_Data>*		mModel;
	IHandler*				mViewHandler;
	MultiHandler			mModelHandler;

	// реализация интерфейсов IHandler
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
		mModelHandler.DoAddHandler(eventType, functor);
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override
	{
		mModelHandler.DoProcessEvent(eventType, evt);
	}//ProcessEvent


};

//---------------------------------------------------------------------------
///
template < typename t_Data > 
class EVecPresenter
	:public IEventPresenter
{
protected:
	virtual void SetViewHandler(IHandler* handler) override
	{
		mViewHandler = handler;
	}
public:
	struct error:	virtual exception_base {};

	EVecPresenter()
		: mVecModel(nullptr), mViewHandler(nullptr)
	{
		// события-уведомления изменяющие модель
		Bind( &EVecPresenter::OnAppendItem,this );		// добавить модель
		Bind( &EVecPresenter::OnDeleteItem,this );		// удалить модель
		Bind( &EVecPresenter::OnSetItemData,this );	// установить значение модели
		Bind( &EVecPresenter::OnSetItemState,this );	// установить статус модели
		Bind( &EVecPresenter::OnLoadModel,this );		// запрос загрузки
		Bind( &EVecPresenter::OnSaveModel,this );		// запрос сохранения
		// не принимаются обновления вектора целиком, только поэлементно... пока так

		// события-запросы получающие данные из модели немедленно
		// необходимы для запроса данных из вьюшки в основном для контролов типа wxDataView
		Bind( &EVecPresenter::OnGetItemData,this );	// запрос значения элемента-модели
		Bind( &EVecPresenter::OnGetItemState,this );	// запрос статуса элемента-модели
		Bind( &EVecPresenter::OnGetSize,this );		// запрос количества элементов 

		//srand (time(NULL));
	}
	~EVecPresenter(){}

	void SetView( IEventView* view)
	{
		if(view)
			view->SetModelHandler(this);
		SetViewHandler( view );
		
		SetModelDataToView();
	}
	void SetModel(IDbRecordVec<t_Data>* vecModel)
	{
		mVecModel = vecModel;
		SetModelDataToView();
	}

	// уведомления для обновления вьюшки, всех элементов(статус и данные)
	void SetModelDataToView()
	{
		if(mVecModel && mViewHandler)
		{
			//whEvtUpdateItems evt;
			whEvtSetData< IDbRecordVec<t_Data>* >	evt(mVecModel);
			mViewHandler->ProcessEvent(	&evt );
		}
	}
	void SetModelStateToView()
	{
		if(mVecModel && mViewHandler)
		{
			whEvtSetState	evt( mVecModel->GetState() );
			mViewHandler->ProcessEvent(	&evt );
		}		
	}
	void GetModelDataFromView()
	{
		if(mVecModel && mViewHandler)
		{
			whEvtGetData< IDbRecordVec<t_Data>* >	evt(mVecModel);
			mViewHandler->ProcessEvent(	&evt );
		}	
	}
	void GetModelStateFromView()
	{
		if(mVecModel && mViewHandler)
		{
			whEvtGetState	evt(mVecModel->GetState());
			mViewHandler->ProcessEvent(	&evt );	// запрос статуса с вьюшки
			mVecModel->SetState( evt.GetState() );	// установка статуса с вьюшки в модель
			SetModelStateToView();				// уведомляем о изменении статуса
		}		
	}
	
	void GetSelected(std::vector<unsigned int>& selected)
	{
		whEvtGetSelected	evt(selected);
		if(mViewHandler)
			mViewHandler->ProcessEvent( &evt );
	}
	// запросы к вьюшке на изменение данных
	// изменение вектора целиком пока что не планируется
	// изменение элементов делается не самой вьюшкой вектора,потому просить у неё изменения смысла нет

	// функционал модели
	void AppendItem(const t_Data& data,  ModelState state=  msCreated)
	{
		if(mVecModel)
		{
			mVecModel->Append(data,state);				// добавляем модель
			if(mViewHandler)
			{
				whEvtAppendItem<t_Data> evt(data);
				mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о добавлении
			}
		}
	
	}//AppendItem
	void DeleteItem(unsigned int row)
	{
		if(mVecModel)
		{
			const  ModelState itemState = mVecModel->GetState(row);
			if(itemState ==	 msCreated )
			{
				mVecModel->Delete(row);
				if(mViewHandler)
				{
					whEvtDeleteItem	evt(row);
					mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о удалении
				}
			}
			else if (itemState== msExist || itemState== msUpdated || itemState== msDeleted )
			{
				mVecModel->SetState(row,  msDeleted);
				if(mViewHandler)
				{
					whEvtSetItemState evt( msDeleted,row);
					mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о удалении
				}
			}
			else
				BOOST_THROW_EXCEPTION( error()<<wxstr("unknown model state") );

		}//if(mVecModel)
	}//DeleteItem
	void SetData(unsigned int row, const t_Data& data)
	{
		if(!mVecModel)
			return;
		const  ModelState itemState = mVecModel->GetState(row);

		if(itemState ==	 msCreated )
		{
			mVecModel->SetData( row, data );
			mVecModel->SetState( msCreated);
			if(mViewHandler)
			{
				whEvtSetItemData<t_Data>	evt(data,row);
				mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о изменении данных элемента
			}
			
		}
		else if (itemState== msExist || itemState== msUpdated || itemState== msDeleted )
		{
			mVecModel->SetData( row, data);
			mVecModel->SetState( msUpdated);
			if(mViewHandler)
			{
				whEvtSetItemData<t_Data>	evt(data,row);
				mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о изменении данных элемента
				whEvtSetItemState	evtSetStatus( msUpdated,row);
				mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о изменении статуса элемента
			}
		}
		else
			BOOST_THROW_EXCEPTION( error()<<wxstr("unknown model state") );
	}
	void SetState(unsigned int row, const  ModelState state)
	{
		if(!mVecModel)
			return;
		mVecModel->SetState( row,state );
		whEvtSetItemState	evt(state, row);
		mViewHandler->ProcessEvent(	&evt );	// уведомляем вьюшку о изменении статуса элемента
	}
	bool GetData(unsigned int row, t_Data& data)
	{
		bool result = false;
		if(mVecModel)
		{
			data = mVecModel->GetData(row);
			result = true;
		}
		return result;
	}
	bool GetState(unsigned int row,  ModelState& state)
	{
		bool result = false;
		if(mVecModel)
		{
			state = mVecModel->GetState(row);
			result = true;
		}
		return result;	
	}

	void LoadModel()
	{
		if( mVecModel )
		{
			mVecModel->ExecLoad();
			SetModelDataToView();
		}
	}
	void SaveModel()
	{
		if( mVecModel )
		{
			mVecModel->ExecSave();
			SetModelDataToView();
		}	
	}

protected:
	typedef IDbRecordVec<t_Data>						t_VecModel;
	
	t_VecModel*				mVecModel;
	IHandler*				mViewHandler;
	MultiHandler			mModelHandler;

// реализация интерфейсов IHandler
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
		mModelHandler.DoAddHandler(eventType, functor);
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override
	{
		mModelHandler.DoProcessEvent(eventType, evt);
	}//ProcessEvent

	// события-уведомления изменяющие модель
	void OnAppendItem( whEvtAppendItem<t_Data>* evt)
	{
		if(evt)
			AppendItem( evt->GetData(), evt->GetState() );
	}
	void OnDeleteItem( whEvtDeleteItem* evt)
	{
		if(evt)	
			DeleteItem( evt->GetIndex() );
	}
	void OnSetItemData( whEvtSetItemData<t_Data>* evt)
	{
		if(evt)
			SetData( evt->GetIndex(), evt->GetData() );
	}
	void OnSetItemState( whEvtSetItemState* evt)
	{
		if(evt)
			SetState( evt->GetIndex(), evt->GetState() );
	}
	void OnLoadModel(whEvtModelLoad* evt)
	{
		LoadModel();
	}
	void OnSaveModel(whEvtModelSave* evt)
	{
		SaveModel();
	}

	// события-запросы получающие данные из модели немедленно
	// необходимы для запроса данных из вьюшки в основном для контролов типа wxDataView
	void OnGetItemData( whEvtGetItemData<t_Data>* evt)
	{
		if(evt && mVecModel && mVecModel->GetSize() )
		{
			unsigned int row = evt->GetIndex();
			//unsigned int row = rand() % mVecModel->GetSize();
			evt->SetData( mVecModel->GetData(row) );
		}
	}
	void OnGetItemState( whEvtGetItemState* evt)
	{
		if(evt && mVecModel && mVecModel->GetSize() )
		{
			unsigned int row = evt->GetIndex();
			//unsigned int row = rand() % mVecModel->GetSize();
			evt->SetState( mVecModel->GetState(row) );
			
		}
	}
	void OnGetSize( whEvtGetSize* evt)
	{
		if(evt && mVecModel)
			evt->SetSize( mVecModel->GetSize() );
			//evt->SetSize( 1000 );
	}


};//EVecPresenter



typedef whModelEvent<meCmdUpdateView>	whEvtCmdUpdateView;
typedef whModelEvent<meCmdUpdateModel>	whEvtCmdUpdateModel;



typedef whModelEvent<meCmdUpdateData>	whEvtCmdUpdateData;
typedef whModelEvent<meCmdUpdateState>	whEvtCmdUpdateState;

class whEvtCmdUpdateItemData
	: public whModelEvent<meCmdUpdateItemData>
{
	unsigned int mRow;
public:
	whEvtCmdUpdateItemData(unsigned int row)	:mRow(row)	{}
	const unsigned int GetRow()const			{	return mRow;	}
};

class whEvtCmdUpdateItemState
	: public whModelEvent<meCmdUpdateItemState>
{
	unsigned int mRow;
public:
	whEvtCmdUpdateItemState(unsigned int row)	:mRow(row)	{}
	const unsigned int GetRow()const			{	return mRow;	}
};




class IMultiPresenter
	:public IEventPresenter
{
public:
	IMultiPresenter()
		:mViewHandler(nullptr)
	{}
	
	void AppendPresenter(IEventPresenter* presenter)
	{ 
		if(presenter)
		{
			mPresenterSet.insert(presenter);
			presenter->SetViewHandler(mViewHandler);
		}
	}
	void RemovePresenter(IEventPresenter* presenter)
	{
		mPresenterSet.erase(presenter);
	}

	virtual void SetViewHandler(IHandler* handler)override
	{
		mViewHandler = handler;
		for(auto it = mPresenterSet.begin(); it != mPresenterSet.end(); ++it)
			(*it)->SetViewHandler(mViewHandler);	
	}


private:
	typedef std::set<IEventPresenter*>	PresenterSet;
	PresenterSet		mPresenterSet;
	IHandler*			mViewHandler;

// реализация интерфейсов IHandler
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override
	{
		for(auto it=mPresenterSet.begin(); it!=mPresenterSet.end(); ++it)
			(*it)->DoProcessEvent(eventType,evt);
	}

};//IMultiPresenter





}//namespace wh
#endif // __IMVP_H