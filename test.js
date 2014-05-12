var cluster = require('cluster');
var addon = require('./build/Release/addon');

if (cluster.isMaster) {
	(function() {
		var obj = new addon.SharedMemory("test_memory_12", false);
		console.log( obj.truncate(10240000) ); 
		var buf = obj.whole();
		console.time('fill, remote');
		buf.fill("hi");
		console.timeEnd('fill, remote');
		console.log('master:', buf); 
		var local_buf = new Buffer(10240000);
		console.time('fill, local');
		local_buf.fill('hi');
		console.timeEnd('fill, local');

		require('http').createServer(function (req, res) {
		  res.writeHead(200, {'Content-Type': 'text/plain'});
		  res.end('Hello World\n');
		  obj.recycle();
		}).listen(9000, '0.0.0.0');

		cluster.fork();

	})();
	
} else {
	(function() {
		var obj = new addon.SharedMemory("test_memory_12", true);
		var buf = obj.whole();
		console.log('cluster:', buf); 
	})();
}

