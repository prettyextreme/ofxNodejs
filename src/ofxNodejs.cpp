#define EV_MULTIPLICITY 0
#define EV_EMBED_ENABLE 0
#define EV_FORK_ENABLE 0

#include "ofxNodejs.h"

#include <ev.h>
#include <eio.h>

namespace node
{
extern int Start (int argc, char *argv[]);
extern v8::Persistent<v8::Context> context;
extern void Stop();
}

namespace ofxNodejs
{

static ev_prepare prepare_watcher;
	
static v8::Handle<v8::Value> ofxInit(const v8::Arguments& args)
{
	v8::HandleScope scope;
	
	v8::Local<v8::Object> require = args[0]->ToObject();
	v8::Local<v8::Object> module = args[1]->ToObject();
	
	v8::Local<v8::Object> global = node::context->Global();
	global->Set(v8::String::NewSymbol("require"), require);
	global->Set(v8::String::NewSymbol("module"), module);
	
	global->Delete(v8::String::NewSymbol("ofxInit"));
	
	return v8::Undefined();
}
	
static v8::Handle<v8::Value> __ofToDataPath(const v8::Arguments& args)
{
	v8::HandleScope scope;
	
	string path = *v8::String::Utf8Value(args[0]);
	path = ofToDataPath(path, true);
	return v8::String::New(path.c_str());
}
	
static void node_init(EV_P_ ev_prepare *watcher, int revents)
{
	v8::HandleScope scope;
	assert(watcher == &prepare_watcher);
	assert(revents == EV_PREPARE);
	
	v8::Local<v8::Object> global = node::context->Global();
	global->Set(v8::String::NewSymbol("ofxInit"), 
				v8::FunctionTemplate::New(ofxInit)->GetFunction());
	
	global->Set(v8::String::NewSymbol("ofToDataPath"), 
				v8::FunctionTemplate::New(__ofToDataPath)->GetFunction());
	
	ev_prepare_stop(&prepare_watcher);
}
	
class NodeEventListener
{
public:
	
	void onUpdate(ofEventArgs&)
	{
		ev_loop(EV_DEFAULT_UC_ EVLOOP_NONBLOCK);
	}
	
	void onExit(ofEventArgs&)
	{
		node::Stop();
	}
	
};
	
static NodeEventListener listener;
static vector<string> paths;
	
void addNodePath(string path)
{
	paths.push_back(path);
}

void init(string startup_script)
{
	//
	// setup node.js
	//
	
	startup_script = ofToDataPath(startup_script, true);
	
	const char *argv[] = {"node", ""};
	int argc = 2;
	
	argv[1] = startup_script.c_str();
	
	string NODE_PATH;
	
	for (int i = 0; i < paths.size(); i++)
	{
		NODE_PATH = ofToDataPath(paths[i], true) + ":" + NODE_PATH;
	}
	
	const char *NODE_PATH_cstr = getenv("NODE_PATH");
	
	if (NODE_PATH_cstr)
	{
		NODE_PATH += string(NODE_PATH_cstr);
	}
	
	setenv("NODE_PATH", NODE_PATH.c_str(), 1);
	setenv("NODE_DISABLE_COLORS", "1", 1);
	
	ev_prepare_init(&prepare_watcher, node_init);
	ev_set_priority(&prepare_watcher, EV_MAXPRI);
	
	ev_prepare_start(EV_DEFAULT_UC_ &prepare_watcher);
	
	printf("[node] starting node.js\n");
	node::Start(argc, (char**)argv);
	

	//
	// setup of events
	//
	
	ofAddListener(ofEvents.update, &listener, &NodeEventListener::onUpdate);
	ofAddListener(ofEvents.exit, &listener, &NodeEventListener::onExit);
}

Value $(string source, string source_name)
{
	v8::HandleScope scope;
	v8::Context::Scope context_scope(node::context);
	
	v8::Local<v8::Script> script = v8::Script::Compile(v8::String::New(source.c_str()),
													   v8::String::NewSymbol(source_name.c_str()));
	v8::Local<v8::Value> result;
	if (!script.IsEmpty()) result = script->Run();

	return Value(scope.Close(result));
}

Value $$(string path)
{
	path = ofToDataPath(path, true);
	
	ofFile file(path);
	ofBuffer buffer = file.readToBuffer();
	
	return $(buffer.getText(), file.getFileName());
}
	
#pragma mark Value

string Value::toString()
{
	if (_value->IsObject())
	{
		return *v8::String::Utf8Value(_value->ToObject()->ObjectProtoToString());
	}
	else
	{
		return *v8::String::Utf8Value(_value);
	}
}
	
void Value::print()
{
	cout << toString() << endl;
}

void setFunction(string funcname, Function function)
{
	v8::HandleScope scope;
	v8::Context::Scope context_scope(node::context);

	v8::Local<v8::Object> global = node::context->Global();
	global->Set(v8::String::NewSymbol(funcname.c_str()), 
				v8::FunctionTemplate::New(function)->GetFunction());
}

}