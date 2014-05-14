var cluster = require('cluster');
var addon = require('./build/Release/addon');

if (cluster.isMaster) {
	(function() {
		var obj = new addon.SharedMemory("test_memory_12", 0x3, 0x3);
		console.log( obj.truncate(1024) );
		var slice = obj.slice(0, 200, 0x3);
		console.log(slice.size());
		var buf = slice.buffer();
		console.log(buf.length);

		require('http').createServer(function (req, res) {
		  res.writeHead(200, {'Content-Type': 'text/plain'});
		  res.end('Hello World\n');
		  obj.recycle();
		}).listen(9000, '0.0.0.0');

		cluster.fork();

	})();
	
} else {
	// (function() {
	// 	var obj = new addon.SharedMemory("test_memory_12", 0x1, 0x1);
	// 	// var buf = obj.whole();
	// 	// console.log('cluster:', buf); 
	// })();
}
