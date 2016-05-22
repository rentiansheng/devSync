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


  var allfile = [];
  var filecount = 0;
  var sendFileNo = 0;
  var displayEnd = 1;

  function getSyncFILE(dir,item) {

    var stats = fs.lstatSync(dir);
    if(stats.isDirectory()) {

      var files = fs.readdirSync(dir);
      var fileNo = files.length;
      if(!files || files == undefined) {
        return;
      }

      for(var index = 0; index < fileNo; index++) {

        file = files[index];
        if(file[0] == '.'){ continue ;}

        file = dir + '/' + file;

        try {
          stats = fs.lstatSync(file);
          if(stats.isDirectory()) {
            if(file[0] == '.'){ continue ;}

            getSyncFILE(file, item);

          } else if(stats.isFile()) {
            file  = path.parse(file);
            if(!file['base']) {continue;}
            allfile[filecount++] = {'dir':dir,'name':file['base'], 'item':item};
          }
        }catch(err) {

          console.log("error"+dir+"\t"+file);
        }

      }

    }

    files = undefined;
    fileNo = 0;

  }

  function syncAllFile(rootItem) {
    var syncItem = allfile.pop();
    if( syncItem !== undefined && syncItem && sendFileNo < filecount ) {

      sendFileSync(syncItem.dir, syncItem.name, syncItem.item, rootItem, 0);
      syncItem = undefined;
      //console.log("send "+(++sendFileNo)+"/"+filecount);
    } else{
        if(displayEnd ) {
          console.log("==============================================");
          console.log("*            sync all file end");
          console.log("*            start watch file change");
          console.log("==============================================");
          displayEnd = 0;
          watchDir(rootItem.offline, rootItem);
        }



    }

    return ;
  }

  function sendFileSync(dir, filename, item, rootItem, retry) {
    if(filename && filename[0] == '.') {syncAllFile(rootItem); return ;};
    var ext = path.extname(filename);
    if(item.exts && item.exts.indexOf(ext) === -1) {syncAllFile(rootItem); return};
    fs.readFile(dir+'/'+filename, function (err, content) {
      if(err) { return false;}
      var relativePath = path.relative(item.offline, dir);
      relativePath = relativePath.replace(/\\/g, '/');
      var servfile  = item.online +'/'+relativePath+'/'+filename;


      var client = net.connect(
          {host: item.host, port: item.port},
          function() { //'connect' listener
            client.write('put '+servfile+'\n'+content.length+'\n\n'+content);
            syncAllFile(rootItem);
          }
      );

      client.on('error', function() {
        if(retry < 3) {
          sendFileSync (dir, filename, item, rootItem, retry+1);
        } else {
          syncAllFile(rootItem);
          console.log('error:'+servfile);
        }
      });

      client.on('data', function(data) {
        client.end();

      });
      client.on('end', function() {
        client.end();

      });


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
      pwd  =  pwd.replace(/\\/g, '/'); 

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
        item.online = item.online.replace(/\\/g, '/');
        if(argv.devSyncAll) {
          getSyncFILE(item.offline, item);

          syncAllFile(item);
        } else {
          console.log("==============================================");
          console.log("*            start watch file change");
          console.log("==============================================");
          watchDir(item.offline, item);
        }

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
          item.online = item.online.replace(/\\/g, '/');
          watchDir(item.offline, item);
        }
      });
    }
  }

})();



fileWatcher();
