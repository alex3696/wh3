#ifndef __IOCSTRCONTAINER_H
#define __IOCSTRCONTAINER_H

#include "_pch.h"

//-----------------------------------------------------------------------------
template<typename TString>
class IOCStrContainer
{
	class FactoryRoot
	{
	public:
		virtual ~FactoryRoot() {}
	};

	template<typename T>
	class CFactory : public FactoryRoot
	{
	protected:
		std::function<std::shared_ptr<T>()> m_functor;
	public:
		~CFactory() {}

		explicit CFactory(std::function<std::shared_ptr<T>()> functor)
			:m_functor(functor)
		{
		}

		virtual std::shared_ptr<T> GetObject()
		{
			return m_functor();
		}
	};

	template<typename T>
	class CFactoryDeffered : public CFactory<T>
	{
		std::shared_ptr<T>					mItem;
	public:

		explicit CFactoryDeffered(std::function<std::shared_ptr<T>()> functor)
			:CFactory(functor)
		{
		}

		virtual std::shared_ptr<T> GetObject()override
		{
			if (!mItem)
				mItem = m_functor();
			return mItem;
		}
	};


	std::map<TString, std::shared_ptr<FactoryRoot>> mStrFactory;

public:
	~IOCStrContainer()
	{
		mStrFactory.clear();
	}
	void Clear(){ mStrFactory.clear(); }

	void Erase(const TString& name)
	{ 
		mStrFactory.erase(name);
	}
	bool IsExist(const TString& name)const
	{
		return mStrFactory.cend() != mStrFactory.find(name);
	}

	template<typename T>
	std::shared_ptr<T> GetObject(const TString& name)
	{
		auto it = mStrFactory.find(name);
		if (mStrFactory.end() != it)
		{
			auto factory = std::static_pointer_cast<CFactory<T>>(it->second);
			return factory->GetObject();
		}
		return std::shared_ptr<T>(nullptr);
	}

	//Most basic implementation - register a functor
	template<typename TInterface, typename ...TS, typename ...N>
	void RegFunctor(const TString& name
		, std::function<std::shared_ptr<TInterface>(std::shared_ptr<TS> ...ts)> functor
		, N... names
		)
	{
		mStrFactory[name]
			= std::make_shared<CFactory<TInterface>>([=]{return functor(GetObject<TS>(names)...); });
	}

	//A factory that will return one instance for every request
	template<typename TInterface, typename TConcrete, typename ...T, typename ...N>
	void RegInstance(const TString& name, N... names)
	{
		std::shared_ptr<TInterface> t
			= std::make_shared<TConcrete>(GetObject<T>(names)...);
		RegInstance<TInterface>(name, t);
	}

	//Reg one instance of an object
	template<typename TInterface>
	void RegInstance(const TString& name, std::shared_ptr<TInterface> t)
	{
		mStrFactory[name] = std::make_shared<CFactory<TInterface>>([=]{return t; });
	}

	//A factory that will call the constructor, per instance required
	template<typename TInterface, typename TConcrete, typename ...TArguments, typename ...N>
	void RegFactory(const TString& name, N... names)
	{
		std::function<std::shared_ptr<TInterface>(std::shared_ptr<TArguments>...)>
			functor = [](std::shared_ptr<TArguments>...arguments)->std::shared_ptr<TInterface>
		{
			return std::make_shared<TConcrete>(
				std::forward<std::shared_ptr<TArguments>>(arguments)...);
		};
		RegFunctor(name, functor,names...);
	}

	template<typename TConcrete, typename ...TArguments, typename ...N>
	void RegFactoryNI(const TString& name, N... names)
	{
		RegFactory<TConcrete, TConcrete, TArguments...>(name, names...);
	}

	template<typename TInterface,typename TConcrete, typename ...TStaticArguments>
	void RegFactoryBind(const TString& name, TStaticArguments... sa)
	{
		std::function<std::shared_ptr<TInterface>(TStaticArguments...)>
			functor = [](TStaticArguments... sa)->std::shared_ptr<TInterface>
		{
			return std::make_shared<TConcrete>(std::forward<TStaticArguments>(sa)...);
		};

		mStrFactory[name]
			= std::make_shared<CFactory<TInterface>>([=]{return functor(sa...); });

	}

	template<typename TConcrete, typename ...T, typename ...N>
	void RegInstanceNI(const TString& name, N... names)
	{
		RegInstance<TConcrete, TConcrete, T...>(name, names...);
	}

	template<typename TInterface, typename TConcrete, typename ...T, typename ...N>
	void RegInstanceDeferred(const TString& name, N... names)
	{


		std::function<std::shared_ptr<TInterface>(std::shared_ptr<T>...)>
			functor = [](std::shared_ptr<T>...arguments)->std::shared_ptr<TInterface>
		{
			return std::make_shared<TConcrete>(
				std::forward<std::shared_ptr<T>>(arguments)...);
		};


		mStrFactory[name]
			= std::make_shared<CFactoryDeffered<TInterface>>([=]{return functor(GetObject<T>(names)...); });

	}

	template<typename TConcrete, typename ...T, typename ...N>
	void RegInstanceDeferredNI(const TString& name, N... names)
	{
		RegInstanceDeferred<TConcrete, TConcrete, T...>(name, names...);
	}




};
//-----------------------------------------------------------------------------
#endif // __APP_H
