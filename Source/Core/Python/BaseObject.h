#ifndef __PYTHON_BASE_OBJECT_H__
#define __PYTHON_BASE_OBJECT_H__

/*
BaseObject:
This is an alternative to the typical way of exposing a class where the PyObject and the class instance
	is very loosely based. For example, converting a class object to Python will result in a new PyObject
	with a borrowed pointer to the object being created.
BaseObject will take care of its own PyObject, only creating a single one. Converting between C++ and Python
	will perform no unnecessary object allocations and the Python GC will be the one responsible for the 
	objects life-time. In C++ the user can explicitly manage the reference count.
*/

namespace python
{
	/// Wrapper around PyObject* that lets users expose C++ classes to Python 
	///		while inheriting some Python functionality such as GC.
	/// See addref() and release() for reference management
	class BaseObject
	{
	public:
		BaseObject();
		virtual ~BaseObject();

		/// Increases the reference count of the object
		void addref();
		/// Decreases the reference count of the object
		void release();

		/// Returns the wrapped PyObject*
		PyObject* ptr();
	
	private:
		PyObject* _obj;
	};


}

#endif // __PYTHON_BASE_OBJECT_H__
