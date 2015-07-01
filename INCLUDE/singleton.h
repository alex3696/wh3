#ifndef __TSingletonH__
#define __TSingletonH__

/** Шаблон cинглетона(одиночка) - инкапсулирует в себе методы и свойства данной сущности,
может быть доступен из любого места программы благодаря методу GetInstance(),
не позволяет создать иерархию классов во главе с классом-интерфейсом, 
в которой некоторые из классов-наследников реализуют паттерн проектирования Singleton, а некоторые нет.
В производных классах следует объявить базовый класс (Singleton) дружественным.
*/
template 
	<class T>
class TSingleton
{
	static T*		_self;
	static int		_refcount;

protected:
	TSingleton(){}
	virtual ~TSingleton()
	{
		_self=NULL;
	}

public:
	static T*	GetInstance();
	void		FreeInst();
	static int	GetRefCount()
	{
		return _refcount;
	}

	/*
	class Ref
	{
		TSingleton* m_Ref;	
	public:
		Ref(TSingleton* ref):m_Ref(ref)	{}
		~Ref()	{ m_Ref->FreeInst(); }
		T* operator->()const { return m_Ref->_self ;}
	};

	static Ref GetRefInst()
	{
		return Ref( GetInstance() );
	}
	*/

};

template <class T>
T*  TSingleton<T>::_self = NULL;

template <class T>
int  TSingleton<T>::_refcount=0;

template <class T>
T*  TSingleton<T>::GetInstance()
{
	if(!_self)
	    _self=new T;
	_refcount++;
	return _self;
}

template <class T>
void  TSingleton<T>::FreeInst()
{
	if(--_refcount==0)
		delete this;
}

//-----------------------------------------------------------------------------------
template 
	<class T>
class TSingletonSptr
{
protected:	
	static std::shared_ptr<T>	_self;

	TSingletonSptr(){}
public:
	virtual ~TSingletonSptr()	{_self.reset();	}
	static std::shared_ptr<T>  Instance()
	{
		if(!_self)
			_self=std::shared_ptr<T>(new T);
		return _self;
	}
};


template <class T> std::shared_ptr<T>   TSingletonSptr<T>::_self = std::shared_ptr<T>(NULL);



#endif //__TSingletonH__