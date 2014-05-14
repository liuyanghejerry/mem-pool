var cluster = require('cluster');
var addon = require('./build/Release/addon');

if (cluster.isMaster) {
	(function() {
		var obj = new addon.SharedMemory("test_memory_12", 0x3, 0x3);
		obj.truncate(1024);
		var slice = obj.slice(4, 200, 0x3);
		var buf = slice.buffer();
		console.log('master', slice.size(), buf.length);

		require('http').createServer(function (req, res) {
		  res.writeHead(200, {'Content-Type': 'text/plain'});
		  res.end('Hello World\n');
		  obj.recycle();
		}).listen(9000, '0.0.0.0');

		cluster.fork();

	})();
	
} else {
	(function() {
		var obj = new addon.SharedMemory("test_memory_12", 0x1, 0x1);
		var buf = obj.slice(0, 8, 0x1).buffer();
		console.log('cluster:', buf, buf.length); 
	})();
}
