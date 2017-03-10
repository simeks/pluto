#include <Testing/Framework.h>

#include <Core/Python/BaseObject.h>
#include <Core/Python/Class.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>
#include <Core/Python/NumPy.h>
#include <Core/Python/Object.h>

#include "Test_Python.h"

#define PYTHON_OBJECT(cls, super)

class Ball : public python::BaseObject
{
	PYTHON_OBJECT(Ball, python::BaseObject);

public:
	Ball();
	~Ball();

};


PYTHON_MODULE(py_test_base_object)
{
	python::Class cls = python::make_class<Ball>("Ball");
	module.def("Ball", cls);
}



TEST_CASE(python_class_unique_ptr)
{
	PYTHON_TEST_PREPARE();
	{

	}
	PYTHON_TEST_CLEANUP();
}
