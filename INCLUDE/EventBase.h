#ifndef __EVENTBASE_H
#define __EVENTBASE_H

#include "_pch.h"

#include "EVENT.h"



namespace wh{


//---------------------------------------------------------------------------
class BaseModel;
//---------------------------------------------------------------------------
/*
class whEvent
{
	bool	mSkip;
public:
	whEvent()
		:mSkip(false)
	{}
	virtual ~whEvent()
	{}

	void Skip(bool enable=true)
	{
		mSkip=true;
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
*/

//---------------------------------------------------------------------------
#define DEFINE_NOTIFY_EVENT( NAME )	\
	class NAME :public whEvent \
	{ \
		public: \
		const BaseModel* mSender; \
		NAME(const BaseModel* sender) \
			:whEvent(),mSender(sender) \
		{} \
	};

DEFINE_NOTIFY_EVENT( NotifyDestroed )
DEFINE_NOTIFY_EVENT( NotyfyUpdated )

static std::type_index  gNotifyDestroedTypeIndex( typeid (NotifyDestroed) );
//---------------------------------------------------------------------------

#define DEFINE_VECNOTIFY_EVENT( NAME )	\
	class NAME :public whEvent \
	{ \
		public: \
		const BaseModel* mSender; \
		unsigned int		mRow; \
		NAME(const BaseModel* sender,unsigned int row ) \
			:whEvent(),mSender(sender), mRow(row) \
		{} \
	};

DEFINE_VECNOTIFY_EVENT( NotyfyItemAppend )
DEFINE_VECNOTIFY_EVENT( NotyfyItemRemoved )
DEFINE_VECNOTIFY_EVENT( NotyfyItemUpdated )

//---------------------------------------------------------------------------
class BaseModel
{
public:
	struct error:	virtual exception_base {};
	bool mStopNotify;

	BaseModel()
		:mStopNotify(false)
	{}

	virtual ~BaseModel()
	{
		/*
		std::cout<<std::endl<<"Subscribers: ";		
		for(auto &evtIt: mSubscribers)
			for(auto &subsIt: evtIt.second)
				std::cout<<" "<<(int)subsIt.mSubscriber;		
		*/
		if(!mStopNotify)
		{
			NotifyDestroed modelDestroed(this);
			Notify(&modelDestroed);
		}
	}

	template<typename Class , typename EventType>  
	void Subscribe( void(Class::*method)(EventType* ),Class* subscriber)
	{
		std::function<void(whEvent*)>	functor = [method,subscriber](whEvent* evt)
			{
				std::bind(method, subscriber, (EventType*)evt )();
			};
		//mSubscribers[typeid(EventType)].insert( SubscribrerItem( (BaseModel*)subscriber,functor) );
		BaseModel* baseSubscriber = static_cast<BaseModel*>(subscriber);
		mSubscribers[typeid(EventType)].emplace( baseSubscriber, functor);
		
		if( gNotifyDestroedTypeIndex != typeid(EventType) )
		{
			//Subscribe( &BaseModel::OnDestroySubscriber, (BaseModel*)subscriber ); // подписываемся на уведомления об удалении
			std::function<void(whEvent*)>	functor2=[this,subscriber](whEvent* evt) 
				{
					NotifyDestroed* evtNotifyDestroed = static_cast<NotifyDestroed*>(evt);
					std::bind(&BaseModel::OnDestroySubscriber, subscriber, evtNotifyDestroed)();
				};
			mSubscribers[gNotifyDestroedTypeIndex].emplace( baseSubscriber, functor2);

			baseSubscriber->Subscribe(&BaseModel::OnDestroySubscriber, this); // подписываем на уведомления об удалении
		}

		if(!mStopNotify)
		{
			NotyfyUpdated modelUpdated(this);
			Notify(&modelUpdated);
		}
		
	}//bind	

	void Unsubscribe( BaseModel* subscriber)
	{
		auto it = mSubscribers.begin();
		while( mSubscribers.end()!=it)
		{
			SubscriberContainer& subscribers = it->second;
			subscribers.erase( SubscribrerItem(	subscriber) );
				
			if( subscribers.size() )
				++it;
			else
				it=mSubscribers.erase(it);
		}//while
	}



	template<typename Class , typename EventType>  
	void Unsubscribe( void(Class::*method)(EventType* ),BaseModel* subscriber)
	{
		auto it = mSubscribers.find(typeid(EventType));
		if( mSubscribers.end() != it )
		{
			it->second.erase( SubscribrerItem(subscriber) );
			if( !it->second.size() )
				mSubscribers.erase(it);
		}
	}
	
	template<typename EventType>  
	void Notify(EventType* evt)const 
	{
		if(mStopNotify)
			return;
		
		SubscriberMap::const_iterator it = mSubscribers.find( typeid(EventType) );
		if(it!=mSubscribers.end() )
		{
			const SubscriberContainer& subscribers  = it->second;
			for(auto itemit=subscribers.cbegin(); itemit!=subscribers.cend() && !evt->IsSkip() ; ++itemit)
			{
				//auto item = itemit->mSubscriber;
				itemit->mFunctor(evt);
			}
				
		}
	}//ProcessEvent

	void OnDestroySubscriber(NotifyDestroed* evt)
	{
		if (evt && evt->mSender != this)
		{
			const BaseModel* baseSender = evt->mSender;
			BaseModel* baseSubscriber = (BaseModel*)(baseSender);
			Unsubscribe(baseSubscriber);
		}
		
	}

protected:
	inline void ClearHandlers()
	{
		mSubscribers.clear();
	}
private:
// определения типов
	struct SubscribrerItem
	{
		BaseModel* mSubscriber;
		const  std::function<void(whEvent*)> mFunctor;

		SubscribrerItem(BaseModel* subscriber)
			:mSubscriber(subscriber)
		{}

		SubscribrerItem(BaseModel* subscriber,const std::function<void(whEvent*)>& functor)
			:mSubscriber(subscriber), mFunctor(functor)
		{}

		inline bool operator<(const SubscribrerItem& sub)const
		{
			return mSubscriber<sub.mSubscriber;
		}
	};

	typedef std::set<SubscribrerItem>	SubscriberContainer;
	typedef std::map< std::type_index, SubscriberContainer >	SubscriberMap;

	SubscriberMap	mSubscribers;

};




















}//namespace wh
#endif // 
