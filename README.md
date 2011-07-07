example
=======

	#include "ofxNodejs.h"

	void testApp::setup()
	{
		// setup ofxNodejs
		ofxNodejs::init("node/main.js");
	
		// eval script from string
		$("console.log('hello world!')");
	
		// eval script from file
		$$("node/myscript.js");
	
		$('var a = 0');
	
		// start web server!
		// variable 'a' will set zero when access to http://localhost:8001
		$("var http = require('http');"
		  "http.createServer(function (request, response) {"
		  "    response.writeHead(200, {'Content-Type': 'text/plain'});"
		  "    response.end('Hello oF!\\n');"
		  "    a = 0;"
		  "}).listen(8001);");
	}

	void testApp::update()
	{
		// variable scope is global
		$("console.log(a); a += 1");
	}

how to build node.js
====================

ofxNodejs needs custom node.js: Here is how to compile node.js as library.

	# get sourcecode
	git clone git@github.com:satoruhiga/node.git
	cd node
	git fetch git@github.com:satoruhiga/node.git embed
	git branch embed

	# configure & build
	ARCHFLAGS="-arch i386" CFLAGS="-arch i386" LDFLAGS="-arch i386" ./configure --product-type=cstaticlib --dest-cpu=ia32
	make staticlib
