var fs = require('fs');
var path = require('path');
var config = require('./config.json');
var net = require('net');





var fileWatcher = (function() {

  function sendFile(dir, filename, item, display ) {

    if(filename && filename[0] == '.') return;
    var ext = path.extname(filename);
    if(item.exts && item.exts.indexOf(ext) === -1) return;
    fs.readFile(dir+'/'+filename, function (err, content) {
      if(err) { return false;}
      var relativePath = path.relative(item.offline, dir);
      relativePath = relativePath.replace(/\\/g, '/');
      var servfile  = item.online +'/'+relativePath+'/'+filename;
      var start =  new Date().getTime();

      try{
        if(display == 1) {
          console.log('data:'+content.length);
        }
        var client = net.connect(
            {host: item.host, port: item.port},
            function() { //'connect' listener
              client.write('put '+servfile+'\n'+content.length+'\n\n'+content);
            }
        );

        client.on('data', function(data) {
          client.end();
        });
        client.on('end', function() {
          client.end();
          if(display == 1 ) {
            console.log('end: '+servfile);
            var end = new Date().getTime();
            var ts = end - start;
            console.log('end: '+content.length);
            console.log('end: start: '+ start+' end: '+ end +' ts:'+ts);
          }

        });
      }catch(err) {
        console.log(err);
      }

    });
  }




  function listenToChange(dir, item) {
    dir = path.resolve(dir);
    function onChg (event, filename) {
      fs.lstat(dir+'/'+filename, function(err, stats) {
        if(err) {return ;}
        if(stats.isDirectory()) {
          item.online = item.online +'/'+filename;
          listenToChange(dir+'/'+filename, item);
        } else if(stats.isFile()) {
          sendFile(dir, filename, item,1);
        }

      });
    }

    fs.watch(dir, onChg);
  }

  function syncAllFILE(root,item) {

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
                syncAllFILE(file, item);
              }else if(stats.isFile()) {
                file  = path.parse(file);
                if(!file['base']) {return;}
                sendFile(root,file['base'],item, 0);
              }
            });
          });
        });

      }

    });
  }


  function watchDir(root, item) {
    listenToChange(root, item);
    fs.lstat(root, function (err, stats) {
      if (stats.isDirectory()) {
        fs.readdir(root, function (err, files) {
          if (err) return;
          files.forEach(function (file) {
            fileName = file;
            if(file[0] == '.'){ return ;}
            file = root + '/' + file;
            fs.lstat(file, function (err, stats) {
              if (err) return;
              if (stats.isDirectory()) {
                watchDir(file, item);
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
    var argv = {'d':'','v':'','devSyncAll':false}

    for(i = 0; i< len; i++ ) {
      if(options[i] == '-d') {
        argv.d = options[++i];
      }else if(options[i] == 'all') {
        argv.devSyncAll = true;
      }
    }



    if(argv.d == null) {
      console.log("error: 请输入config配置中path配置项目");
      return ;
    }


    if(argv.d != '') {
      var pwd = process.cwd();
      console.dir(pwd);
      if(config.path.hasOwnProperty(argv.d) ) {
        item = config.path[argv.d];
        item.offline = pwd;
        if(!item.host) {
          item.host = config.server.host;
        }
        if(!item.port) {
          item.port = config.server.port;
        }
        item.devSyncAll = argv.devSyncAll;

        if(argv.devSyncAll) {
          syncAllFILE(item.offline, item);
          console.log("\n-------------------------------------------------\n");
          console.log("\n  devSync all end\n");
          console.log("\n-------------------------------------------------\n");

        }
        watchDir(item.offline, item);
      } else {
        console.log("error:"+argv.d+"配置不存在");
      }

    }else {
      config.local.forEach(function (item) {
        if (item.auto) {
          if(!item.host) {
            item.host = config.server.host;
          }
          if(!item.port) {
            item.host = config.server.port;
          }
          item.devSyncAll = argv.devSyncAll;
          watchDir(item.offline, item);
        }
      });
    }
  }

})();



fileWatcher();
