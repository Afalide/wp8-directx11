
#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <cassert>

#define register_singleton(class_name) template <> class_name* singleton<class_name>::_instance = NULL;

template <typename T>
class singleton
{

public:

    singleton();
    virtual ~singleton();

    static T* get_instance();
    //static void delete_instance();

protected:

    static T* _instance;
};

template <typename T>
singleton<T>::singleton()
{
    assert(NULL == _instance);
    _instance = static_cast<T*>(this);
}

template <typename T>
singleton<T>::~singleton()
{
    assert(NULL != _instance);
    _instance = NULL;
}

template <typename T>
T*
singleton<T>::get_instance()
{
//    if(NULL == _instance)
//        _instance = new T;

    return _instance;
}

//template <typename T>
//void
//singleton<T>::delete_instance()
//{
//    if(NULL == _instance)
//        return;
//
//    delete _instance;
//    _instance = NULL;
//}

#endif // SINGLETON_HPP

