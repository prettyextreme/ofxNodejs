#include "ofxNodejsBindings.h"

namespace ofxNodejs
{

	

	
//	register_swig_type_info(vector<string>);
//	register_swig_type_info(vector<float>);
//	register_swig_type_info(vector<int>);
//	register_swig_type_info(vector<ofFloatColor>);
//	register_swig_type_info(vector<ofVec2f>);
//	register_swig_type_info(vector<ofVec3f>);
//	register_swig_type_info(vector<ofVec4f>);
//	register_swig_type_info(vector<ofPath>);
//	register_swig_type_info(vector<ofPolyline>);
	
	
#pragma -
#pragma push
	
	template <typename T>
	void push(T *val, bool gc = false)
	{
		swig_type_info *t = type2swig_type_info<T>();
		SWIG_NewPointerObj(val, t, gc);
	}
	
	template <typename T>
	void push(const T *val)
	{
		push(val, false);
	}
	
	template <typename T>
	void push(const T& val)
	{
		push(new T(val), true);
	}
	
	template <typename T>
	void push(T val)
	{
		push(new T(val), true);
	}
}
