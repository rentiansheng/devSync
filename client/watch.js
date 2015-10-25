var fs = require('fs');
var path = require('path');
var config = require('./config.json');
var net = require('net');



var fileWatcher = (function() {
  function listenToChange(dir, offline, online, exts) {
    dir = path.resolve(dir);
    function onChg (event, filename) {
      if(filename[0] == '.') return;
      var ext = path.extname(filename);
      if(exts && exts.indexOf(ext) === -1) return;
      fs.readFile(dir+'/'+filename, function (err, content) {
        if(err) { return false;}
        var servfile  = online +'/'+path.relative(offline, dir)+'/'+filename;

        console.log(servfile);
        console.log(content.length);
        var client = net.connect(
            {host: config.server.host, port: config.server.port},
            function() { //'connect' listener
              client.write('put '+servfile+'\n'+content.length+'\n\n'+content);
            }
        );

        client.on('data', function(data) {
          client.end();
        });
        client.on('end', function() {
          console.log('disconnected from server');
        });
      });
    }


    fs.watch(dir, onChg);
  }



  function watchDir(root, offline, online, exts) {
    listenToChange(root, offline, online, exts);
    fs.lstat(root, function (err, stats) {
      if (stats.isDirectory()) {
        fs.readdir(root, function (err, files) {
          if (err) return;
          files.forEach(function (file) {
            if(file[0] == '.'){ return ;}

            file = root + '/' + file;
            fs.lstat(file, function (err, stats) {
              if (err) return;
              if (stats.isDirectory()) {
                watchDir(file, offline, online, exts, exts);
              }
            });
          });
        });

      }

    });
  }



  return function() {
    config.local.forEach(function(item){
      if(item.auto) {
        watchDir(item.offline, item.offline, item.online, item.exts);
      }
    });
  }

})();



fileWatcher();









