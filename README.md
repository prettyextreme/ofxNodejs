# ofxNodejs

## Setup notes

Add compiler flags to Project.xcconfig

	OTHER_LDFLAGS = $(OF_CORE_LIBS) -lcrypto -lssl

## How to build libnode (option)

ofxNodejs needs custom node.js library: Here is how to compile libnode.

	# checkout patched source code
	$ git clone git@github.com:satoruhiga/node.git
	$ cd node
	$ git checkout origin/ofxNodejs-v0.6.14-release -b ofxNodejs-v0.6.14-release
	
	# build
	$ ./configure --dest-cpu=ia32 --product-type=cstaticlib --openssl-includes=/usr/include --openssl-libpath=/usr/lib
	$ make staticlib
	
	# maybe link error will occurred
	# you need to create 32bit-version of uv.a
	
	$ cd out/Release/deps/uv
	$ env CFLAGS="-arch i386" CPPFLAGS='-arch i386' make

	# build again	
	$ cd ../../../../
	$ make staticlib
	
	# find staticlib
	$ find . -name "*.a"
	
