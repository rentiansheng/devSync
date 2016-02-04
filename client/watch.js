var fs = require('fs');
var path = require('path');
var config = require('./config.json');
var net = require('net');



var fileWatcher = (function() {
  function listenToChange(dir, offline, online, exts) {
    dir = path.resolve(dir);
    function onChg (event, filename) {

      if(filename && filename[0] == '.') return;
      var ext = path.extname(filename);
      if(exts && exts.indexOf(ext) === -1) return;
      fs.readFile(dir+'/'+filename, function (err, content) {
        if(err) { return false;}
        var relativePath = path.relative(offline, dir);
        relativePath = relativePath.replace(/\\/g, '/');
        var servfile  = online +'/'+relativePath+'/'+filename;
        var start =  new Date().getTime();

        try{
          console.log('data:'+content.length);

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
            client.end();
            var end = new Date().getTime();
            var ts = end - start;
            console.log('end: '+servfile);
            console.log('end: '+content.length);
            console.log('end: '+'disconnected from server');
            console.log('end: start: '+ start+' end: '+ end +' ts:'+ts);
          });
        }catch(err) {
          console.log(err);
        }

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
    var options = process.argv
    var len = options.length;
    var i ;
    var argv = {'d':'','v':''}

    for(i = 0; i< len; i++ ) {
      if(options[i] == '-d') {
        argv.d = options[++i];
      }
    }

    if(argv.d != '') {
      var pwd = process.cwd();
      console.dir(pwd);
      if(argv.d in config.path ) {
        item = config.path[argv.d];
        item.offline = pwd;
        watchDir(item.offline, item.offline, item.online, item.exts);
      } else {
        console.log("error:"+argv.d+"配置不存在");
      }

    }else {
      config.local.forEach(function (item) {
        if (item.auto) {
          watchDir(item.offline, item.offline, item.online, item.exts);
        }
      });
    }
  }

})();



fileWatcher();









