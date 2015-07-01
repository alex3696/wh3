#ifndef __EVENT_H
#define __EVENT_H

#include "_pch.h"
#include "whDB.h"


namespace wh{


//---------------------------------------------------------------------------
class whEvent
{
	bool mSkip;
public:
	whEvent()
		:mSkip(false)
	{}
	virtual ~whEvent()
	{}

	void Skip(bool enable=true)
	{
		mSkip=enable;
	}
	bool IsSkip()const 
	{
		return mSkip;
	}

	virtual unsigned int GetEventType()const
	{
		return 0;
	}
};
//---------------------------------------------------------------------------
///
class IHandler
{
public:
	virtual ~IHandler(){}

	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)=0;
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )=0;

	template<typename EventType> 
	void Bind(boost::function<void(EventType*)> functor)
	{
		boost::function<void(whEvent*)>	vecFunctor = [functor](whEvent* evt)
			{ 
				EventType* realType = dynamic_cast<EventType*>(evt);
				if(realType)
					functor(realType);
			};
		DoAddHandler(typeid(EventType), vecFunctor );
	}
	
	template<typename Class , typename EventType>  
	void Bind ( void(Class::*method)(EventType* ), Class *handler)
	{
		boost::function<void(EventType*)>	newHandler;
		newHandler = boost::bind( method,handler, _1 );
		
		Bind(newHandler);
	}//bind

	template<typename EventType> 
	void ProcessEvent(EventType* evt)
	{
		DoProcessEvent( typeid(EventType), evt );
	}
};
//---------------------------------------------------------------------------
///
class VecHandler
	:public IHandler
{
private:
	std::set< IHandler* >  mHandler;
public: 
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor) override 
	{
		for(auto it=mHandler.begin(); it != mHandler.end(); ++it )
			(*it)->DoAddHandler(eventType,functor);
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt ) override
	{
		for(auto it=mHandler.begin(); it != mHandler.end(); ++it )
		{
			if ( !evt->IsSkip() )
				(*it)->DoProcessEvent(eventType,evt);
		}
	}
	
	void  AppendHandler( IHandler* handler)
	{
		mHandler.insert(handler);
	}
	void  RemoveHandler( IHandler* handler)
	{
		mHandler.erase(handler);
	}

};
//---------------------------------------------------------------------------
///
class MultiHandler
	:public IHandler
{
private:
	typedef std::map< std::type_index, boost::function<void(whEvent*)> >	HandlerMap;
	
	HandlerMap  mHandler;
public:
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
		mHandler[ eventType ] = functor;
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override
	{
		HandlerMap::iterator it = mHandler.find( eventType );
		if(it!=mHandler.end() )
		{
			boost::function<void(whEvent*)> eventFoo = it->second;
			eventFoo(evt);
		}
	}//ProcessEvent

};

//---------------------------------------------------------------------------
///
class MediatorHandler
	:public IHandler
{
	IHandler** mTargetHandler;
public:
	MediatorHandler(IHandler** modelHandler)
		: mTargetHandler(modelHandler)
	{}

	void SetTargetHandler(IHandler** modelHandler)
	{
		mTargetHandler = modelHandler;
	}

	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override 
	{
		if(*mTargetHandler)
			(*mTargetHandler)->DoProcessEvent(eventType,evt);	
	}
	
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override
	{
		if(*mTargetHandler)
			(*mTargetHandler)->DoAddHandler(eventType,functor);	
	}
};










//---------------------------------------------------------------------------
///
/*
class SingleEventHandler
	: public IHandler
	, std::pair<std::type_index, boost::function<void(whEvent*)> >
{
public:
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override final
	{
		first = eventType;
		second = functor;
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override final
	{
		if ( first == eventType && second)
			second(evt);
	}//ProcessEvent

};
//---------------------------------------------------------------------------
//

class MultiEventHandler
	:public IHandler
{
private:
	typedef std::map< std::type_index, IHandler* >	HandlerMap;
	
	HandlerMap  mHandler;
public:
	virtual void DoAddHandler(std::type_index eventType, boost::function<void(whEvent*)> functor)override final
	{
		SingleEventHandler sh;
		sh.DoAddHandler(eventType,functor);
		mHandler.insert( std::make_pair(eventType,sh) );
	}
	virtual void DoProcessEvent(std::type_index eventType, whEvent* evt )override final
	{
		HandlerMap::iterator it = mHandler.find( eventType );
		if(it!=mHandler.end() )
			it->second->ProcessEvent(evt);
	}//ProcessEvent

};
*/




}//namespace wh
#endif // __IMVP_H


/*
//---------------------------------------------------------------------------
///
template < typename t_Data	> 
class TEventView
{
public:
	struct error:	virtual exception_base {};
	virtual  ~TEventView(){}

	virtual void		OnChangeModelData(const t_Data& )=0;
	virtual void		OnChangeModelState( ModelState )=0;

	void OnChangeModelData(whEvent* evt)
	{
		auto dataEvt = dynamic_cast < whEvtData<t_Data,whEVT_CMD_SET_DATA>*>(evt);
		if(dataEvt)
			OnChangeModelData(dataEvt->GetData() );
	}
	void OnChangeModelState(whEvent* evt)
	{
		auto dataEvt = dynamic_cast < whEvtData< ModelState,whEVT_CMD_SET_STATE>*>(evt);
		if(dataEvt)
			OnChangeModelState(dataEvt->GetData() );
	}



	boost::function<void(whEvent*)>		ModelHandler; 
	void SetModelHandler( boost::function<void(whEvent*)> handler )
	{
		ModelHandler=handler;
	}

	void ProcessEvent(whEvent* evt)
	{
		auto evtType = evt->GetEventType();
		switch()
		{
		default:
			break;
		case whEVT_CMD_SET_DATA:
			OnChangeModelData(evt);
			break;
		case whEVT_CMD_SET_STATE:
			OnChangeModelState(evt);
			break;
		}//switch
	}// HandleEvent

	const t_Data&		GetModelData()
	{
		if(!ModelHandler)
			BOOST_THROW_EXCEPTION( error()<<wxstr("No model event handler") );

		whEvtData<t_Data,whEVT_CMD_GET_DATA> evt();
		ModelHandler( &evt );
		return evt.GetData();
	}
	 ModelState		GetModelState()
	{
		if(!ModelHandler)
			BOOST_THROW_EXCEPTION( error()<<wxstr("No model event handler") );
		whEvtData<t_Data,whEVT_CMD_GET_STATE> evt();
		ModelHandler( &evt );
		return evt.GetData()
	}



};//TEventView
//---------------------------------------------------------------------------
/// шаблон презентера
template < typename t_Data > 
class TEventPresenter
{
private:
	TDbRecord<t_Data>*					mModel;
	boost::function<void(whEvent*)>*	mViewHandler;
public:
	boost::function<void(whEvent*)>		EventHandler; 
	
	void ProcessEvent(whEvent* evt)
	{
		auto evtType = evt->GetEventType();
		switch()
		{
		default:
			break;
		case whEVT_CMD_SET_DATA:
			SetModelData(evt);
			break;
		case whEVT_CMD_SET_STATE:
			SetModelState(evt);
			break;
		case whEVT_CMD_MODEL_SAVE:
			SaveModel(evt);
			break;
		case whEVT_CMD_MODEL_LOAD:
			LoadModel(evt);
			break;
		}//switch

	}// HandleEvent

	void SetViewHandler( boost::function<void(whEvent*)> handler )
	{
		mViewHandler=handler;
		if(mModel)
			mView->OnChangeModel( GetModel() );
	}

	void SetModelData(whEvent* evt)
	{
		auto dataEvt = dynamic_cast < whEvtData<t_Data,whEVT_CMD_SET_DATA>*>(evt);
		if(dataEvt)
			SetModelData(dataEvt->GetData() );
	}
	void SetModelState(whEvent* evt)
	{
		auto dataEvt = dynamic_cast < whEvtData< ModelState,whEVT_CMD_SET_STATE>*>(evt);
		if(dataEvt)
			SetModelState(dataEvt->GetData() );
	}
	void LoadModel(whEvent* evt=nullptr)
	{
		if( mModel )
		{
			mModel->ExecLoad();
			ChangeViewData();
			ChangeViewState();	
		}
	}
	void SaveModel(whEvent* evt=nullptr)
	{
		if( mModel )
		{
			mModel->ExecSave();
			ChangeViewState();	
		}
	}


	void SetModelData(const t_Data& data)
	{
		if( mModel )
			mModel->SetProperty( data );
		ChangeViewData();
	}
	void SetModelState( ModelState state)
	{
		if( mModel )
			mModel->SetState( state );
		ChangeViewState();
	}

	void ChangeViewData()
	{
		if(mViewHandler && mModel)
			mViewHandler( whEvtModelUpdateData<t_Data>( mModel->GetProperty() ) );
	}
	void ChangeViewState()
	{
		if(mViewHandler && mModel)
			mViewHandler( whEvtModelUpdateState<t_Data>( mModel->GetState() ) );
	}



};//TEventPresenter
*/
