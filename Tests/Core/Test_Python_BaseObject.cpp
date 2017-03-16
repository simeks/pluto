//#include <Testing/Framework.h>
//
//#include <Core/Python/BaseObject.h>
//#include <Core/Python/Class.h>
//#include <Core/Python/Function.h>
//#include <Core/Python/Module.h>
//#include <Core/Python/NumPy.h>
//#include <Core/Python/Object.h>
//
//#include "Test_Python.h"
//
//#define PYTHON_OBJECT(cls, super)
//
//class Ball : public python::BaseObject
//{
//	PYTHON_OBJECT(Ball, python::BaseObject);
//
//public:
//	Ball() {}
//	virtual ~Ball() {}
//
//	virtual int radius()
//	{
//		return 0;
//		/*python::Object method = attr("radius");
//		if (is_overriden(method))
//		{
//			method();
//		}*/
//	}
//};
//class CppBall : public Ball
//{
//public:
//	CppBall() {}
//	virtual ~CppBall() {}
//
//	int radius()
//	{
//		return 5;
//	}
//};
//
//
//PYTHON_MODULE(py_test_base_object)
//{
//	python::Class cls = python::make_class<Ball>("Ball");
//	module.def("Ball", cls);
//}
//
//
////
////TEST_CASE(python_base_object)
////{
////	PYTHON_TEST_PREPARE();
////	{
////		//python::Ref<Ball> ball = python::make_object<Ball>();
////
////
////
////	}
////	PYTHON_TEST_CLEANUP();
////}
