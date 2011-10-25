#pragma once

#include "ofMain.h"
#include "v8.h"

namespace ofxNodejs
{

//
// ObjectProxy
//

class Object;
typedef vector<Object> Array;

class Object
{
public:

	Object() { v = v8::Null(); }
	Object(v8::Handle<v8::Value> value) : v(value) {}

	Object(bool o) { v = v8::Boolean::New(o); }
	Object(int32_t o) { v = v8::Int32::New(o); }
	Object(uint32_t o) { v = v8::Uint32::New(o); }
	Object(float o) { v = v8::Number::New(o); }
	Object(double o) { v = v8::Number::New(o); }
	Object(string o) { v = v8::String::New(o.c_str()); }
	Object(Array o);

	template<class T>
	inline T as() {}

	operator v8::Handle<v8::Value>&() { return v; }

private:

	v8::Handle<v8::Value> v;
};

template<>
inline bool Object::as() { return v->BooleanValue(); }

template<>
inline int32_t Object::as() { return v->Int32Value(); }

template<>
inline uint32_t Object::as() { return v->Uint32Value(); }

template<>
inline float Object::as() { return v->NumberValue(); }

template<>
inline double Object::as() { return v->NumberValue(); }

template<>
inline string Object::as()
{
	v8::HandleScope scope;

	if (v->IsArray())
	{
		string str;

		v8::Local<v8::Array> a = v8::Local<v8::Array>(v8::Array::Cast(*v));
		for (int i = 0; i < a->Length(); i++)
		{
			Object o = a->Get(i);
			str += o.as<string>() + ", ";
		}

		if (!str.empty()) str.resize(str.length() - 2);

		return "[" + str + "]";
	}
	else if (v->IsString())
	{
		return string("'") + *v8::String::Utf8Value(v) + "'";
	}
	else if (v->IsObject())
	{
		return *v8::String::Utf8Value(v->ToObject()->ObjectProtoToString());
	}
	else
	{
		return *v8::String::Utf8Value(v);
	}
}

template<>
inline Array Object::as()
{
	v8::HandleScope scope;

	Array result;
	v8::Local<v8::Array> a = v8::Local<v8::Array>(v8::Array::Cast(*v));

	for (int i = 0; i < a->Length(); i++)
		result.push_back(a->Get(i));

	return result;
}

}