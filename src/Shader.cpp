#include "Types.hpp"

PyObject * MGLShader_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLShader * self = (MGLShader *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLShader_tp_dealloc(MGLShader * self) {
	MGLShader_Type.tp_free((PyObject *)self);
}

PyObject * MGLShader_release(MGLShader * self) {
	MGLShader_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLShader_tp_methods[] = {
	{"release", (PyCFunction)MGLShader_release, METH_NOARGS, 0},
	{0},
};

PyTypeObject MGLShader_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Shader",                                           // tp_name
	sizeof(MGLShader),                                      // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLShader_tp_dealloc,                       // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	0,                                                      // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	0,                                                      // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT,                                     // tp_flags
	0,                                                      // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLShader_tp_methods,                                   // tp_methods
	0,                                                      // tp_members
	0,                                                      // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLShader_tp_new,                                       // tp_new
};

void MGLShader_Invalidate(MGLShader * shader) {
	if (Py_TYPE(shader) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	const GLMethods & gl = shader->context->gl;
	gl.DeleteShader(shader->shader_obj);

	Py_TYPE(shader) = &MGLInvalidObject_Type;
	Py_DECREF(shader);
}

void MGLShader_Compile(MGLShader * shader) {
	const GLMethods & gl = shader->context->gl;

	const char * source = PyUnicode_AsUTF8(shader->source);

	int obj = gl.CreateShader(shader->shader_type);

	if (!obj) {
		MGLError_Set("cannot create shader");
		return;
	}

	gl.ShaderSource(obj, 1, &source, 0);
	gl.CompileShader(obj);

	int compiled = GL_FALSE;
	gl.GetShaderiv(obj, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		const char * message = "GLSL Compiler failed";
		const char * title = SHADER_NAME[shader->shader_slot];
		const char * underline = SHADER_NAME_UNDERLINE[shader->shader_slot];

		int log_len = 0;
		gl.GetShaderiv(obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetShaderInfoLog(obj, log_len, &log_len, log);

		gl.DeleteShader(obj);

		MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

		delete[] log;
		return;
	}

	shader->shader_obj = obj;
}
