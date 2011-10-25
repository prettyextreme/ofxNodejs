#include "ofxNodejsFunction.h"

namespace node
{
	extern v8::Persistent<v8::Context> context;
}

namespace ofxNodejs
{
	
map<string, Function*> Function::callback_map;

void Function::bind(string funcname_)
{
	funcname = funcname_;
	callback_map[funcname] = this;
	
	v8::Local<v8::Object> global = node::context->Global();
	v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(callback);
	v8::Local<v8::Function> func = ft->GetFunction();
	
	func->SetName(v8::String::NewSymbol(funcname.c_str()));
	global->Set(v8::String::NewSymbol(funcname.c_str()), func);
}

void Function::unbind()
{
	callback_map.erase(funcname);
}

v8::Handle<v8::Value> Function::exec(const v8::Arguments& args)
{
	return v8::Undefined();
}

v8::Handle<v8::Value> Function::callback(const v8::Arguments& args)
{
	v8::Handle<v8::Value> v = args.Callee()->GetName();
	string name = *v8::String::Utf8Value(v);
	
	if (callback_map.find(name) != callback_map.end())
	{
		return callback_map[name]->exec(args);
	}
	
	return v8::Undefined();
}

}