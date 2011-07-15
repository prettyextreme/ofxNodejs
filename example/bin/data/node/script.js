console.log("hello script.js");

var http = require('http');
var fs = require('fs')
var url = require('url');


// simple responce

http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('hello oF!\n');
}).listen(8000);


// serve static file

var staticResource = require('static-resource');
var handler = staticResource.createHandler(fs.realpathSync(ofToDataPath('node/public')));

var server = http.createServer(function(request, response) {
    var path = url.parse(request.url).pathname;
    
    if(!handler.handle(path, request, response)) {
        response.writeHead(404);
        response.write('404');
        response.end();
    }
});
server.listen(8001);


// communicate with socket.io

var io = require('socket.io').listen(3000);
io.set('log level', 1); 

io.sockets.on('connection', function (socket) {
    var client_id = socket.id;
    
    onLoggedIn(client_id);
    
    socket.on('disconnect', function () {
        onLoggedOut(client_id);
    });
    
    socket.on('mouse', function(data){
        onMouseMoved(client_id, data.x, data.y);
    })
});
