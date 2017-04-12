var fs = require('fs');
var path = require('path');
var config = require('./config.json');
var net = require('net');



var fileWatcher = (function() {

    var count = 0;
    var allfile = [];
    var allFileCount = 0;
    var sendFileNo = 0;
    var MaxFileSize = 14*1024*1024; //14m
    var SYNC_METHOD_UPLOAD = 'upload';
    var SYNC_METHOD_DELETE = 'delete';
    var maxProcess = 20;
    var childProcess = 0;


    //发送文件
    function sendFile(dir, filename, item) {
        if (filename && filename[0] == '.') return;
        var ext = path.extname(filename);
        if (item.exts && item.exts.length > 0 && item.exts.indexOf(ext) === -1) return;

        var relativePath = path.relative(item.offline, dir);
        relativePath = relativePath.replace(/\\/g, '/');
        var servfile = item.online + '/' + relativePath + '/' + filename;
        var localFile = dir + '/' + filename;
        var stat = fs.statSync(localFile);

        getConnectClientWithEvent(item, servfile, localFile, stat.size, SYNC_METHOD_UPLOAD)
    }


    function sendDelFile(dir, filename, item) {
        if (filename && filename[0] == '.') return;
        var ext = path.extname(filename);
        if (ext.length > 0 && item.exts && item.exts.length > 0 && item.exts.indexOf(ext) === -1) { return; }

        var relativePath = path.relative(item.offline, dir);
        relativePath = relativePath.replace(/\\/g, '/');
        var servfile = item.online + '/' + relativePath + '/' + filename;
        var localFile = dir + '/' + filename;


        getConnectClientWithEvent(item, servfile, localFile, 0, SYNC_METHOD_DELETE);

    }


    //检测目录变化
    function listenToChange(dir, item) {
        dir = path.resolve(dir);

        function onChg(event, filename) {
            if(filename.indexOf('___jb_tmp___') >= 0
              || filename.indexOf('___jb_old___') >= 0) {
                return ;
            }
            var file = dir + '/' + filename;
            fs.exists(file, function(exists) {
                if (!exists) {

                    sendDelFile(dir, filename, item);
                    return;
                }
                fs.lstat(dir + '/' + filename, function(err, stats) {
                    if (err) {
                        return;
                    }
                    if (stats.isDirectory()) {
                        var isSendFile = 0;
                        if (event == "rename") {
                            isSendFile = 1;
                        }
                        watchDir(dir + '/' + filename, item, isSendFile);
                    } else if (stats.isFile()) {
                        if (stats.size < item.maxFileSize) {
                            sendFile(dir, filename, item);
                        } else {
                            console.log("\n\n\n     You can not synchronize large files! \n\n\n");
                        }
                    }

                });
            });

        }

        fs.watch(dir, onChg);
    }

    //为目录加监控时间
    function watchDir(root, item, isSendFile) {

        for (var i = 0; i < item.ignore.length; i++) {
            if (root.indexOf(item.ignore[i]) >= 0) {
                return;
            }
        }


        listenToChange(root, item);
        fs.lstat(root, function(err, stats) {
            if (stats.isDirectory()) {
                fs.readdir(root, function(err, files) {
                    if (err) return;
                    files.forEach(function(file) {
                        var fileName = file;
                        if (file[0] == '.') { return; }
                        file = root + '/' + file;
                        fs.lstat(file, function(err, stats) {
                            if (err) return;
                            if (stats.isDirectory()) {
                                watchDir(file, item, isSendFile);
                            } else if (isSendFile && stats.isFile()) {
                                sendFile(root, fileName, item, true);
                            }
                        });
                    });
                });

            }

        });
    }

    //获取要同步全部的文件，
    function getSyncFILE(dir, item) {

        var stats = fs.lstatSync(dir);
        if (stats.isDirectory()) {

            var files = fs.readdirSync(dir);
            var fileNo = files.length;
            if (!files || files == undefined) {
                return;
            }

            for (var index = 0; index < fileNo; index++) {

                file = files[index];
                if (file[0] == '.') { continue; }

                file = dir + '/' + file;

                var isSkip = false;
                for (var i = 0; i < item.ignore.length; i++) {
                    if (file.indexOf(item.ignore[i]) >= 0) {
                        isSkip = true;
                        break;
                    }
                }
                if (isSkip) {
                    continue;
                }
                try {
                    stats = fs.lstatSync(file);
                    if (stats.isDirectory()) {
                        if (file[0] == '.') { continue; }

                        getSyncFILE(file, item);

                    } else if (stats.isFile()) {
                        file = path.parse(file);
                        if (!file['base']) { continue; }
                        allfile[allFileCount++] = { 'dir': dir, 'name': file['base'], 'item': item };
                    }
                } catch (err) {

                    console.log("error" + dir + "\t" + file);
                }

            }

        }

        files = undefined;
        fileNo = 0;

    }

    //同步全部文件
    function syncAllFile(rootItem) {
        var syncItem = allfile.pop();
        if (syncItem !== undefined && syncItem ) {
            sendFileForSyncAll(syncItem.dir, syncItem.name, syncItem.item, rootItem, 0);
            syncItem = undefined;
            console.info("sync file process: "+(++sendFileNo)+"/"+allFileCount);
        } else {
            childProcess --;
            if (childProcess <=  0) {
                printSyncAllHeader(false);
                //不同步目录下文件
                watchDir(rootItem.offline, rootItem, false);
            }
        }

        return;
    }

    var sendFileCount = {'succ':0, 'error':0};
    //用于同步全部文件时发送文件
    function sendFileForSyncAll(dir, filename, item, rootItem, retry) {

        if (filename && filename[0] == '.') { syncAllFile(rootItem); return; };
        var ext = path.extname(filename);
        if (item.exts && item.exts.length > 0 && item.exts.indexOf(ext) === -1) { syncAllFile(rootItem); return };

        var relativePath = path.relative(item.offline, dir);
        relativePath = relativePath.replace(/\\/g, '/');
        var servfile = item.online + '/' + relativePath + '/' + filename;
        var isErr = false;
        var localFile = dir + '/' + filename;
        var stat = fs.statSync(localFile);

        var client = client = net.connect({ host: item.host, port: item.port },

          function() { //'connect' listener
              client.write('put ' + servfile + '\ncontent-length:' + stat.size + '\n\n');// + content);
              var fileStream = fs.createReadStream(localFile);
              fileStream.pipe(client);

          }
        );

        client.on('error', function(error) {
            isErr = true;
            if (retry < 3) {
                sendFileForSyncAll(dir, filename, item, rootItem, retry + 1);
            } else {
                syncAllFile(rootItem);
                console.log('error:' + servfile);
            }
        });

        client.on('data', function(data) {
            client.end();

        });
        client.on('end', function() {
            if(isErr == false) {
                sendFileCount['succ'] ++;
                syncAllFile(rootItem);
            } else {
                sendFileCount['error'] ++;
            }

            client.end();

        });
    }

    function syncAllStartProcess(rootItem) {
        childProcess = 20;
        for(i = 0; i < 20; i++) {
            syncAllFile(rootItem);
        }
    }

    function getConnectClientWithEvent(item, servfile, localFile, fileSize, method) {
        var start = new Date().getTime();
        console.log("\nsync start: file[ " + servfile + ' ]');
        var isErr = false

        var client = net.connect({ host: item.host, port: item.port });

        client.on('data', function(data) {
            //console.log(data.toString());
            if (isErr == false) {
                var arrRespone = [] // = data.toString()
                arrRespone.push(data.toString())
                var strRespone = data.toString()
                header = arrRespone[0].split("\r\n\r\n")
                header.shift();
                if (header.length > 0 && header[0] != "success") {
                    isErr = true
                    console.log("\n\n=============\nfatal error\n")
                    console.log(header.join(""))
                    console.log("\n=============\n")
                }
            }
        });
        client.on('end', function() {
            if (isErr) {
                return
            }
            client.end();
            console.log('sync ' + method + ' file end: fileName[ ' + servfile + ' ]');
            var end = new Date().getTime();
            var ts = end - start;

            var strStartTime = formatDate(new Date(start));
            if(SYNC_METHOD_UPLOAD == method) {
                console.log('sync file end: file length[ ' + fileSize + 'b ]');
            }

            console.log('sync ' + method + ' file end: start time[ ' + strStartTime + ' ]  sync use time[ ' + ts + 'ms ]');
            console.log("\n");



        });
        client.on('error', function(err) {
            isErr = true
            console.log("\n\n=============\nfatal error\n")
            console.log("* sync file error:[check network or server is start]");
            console.log("\n=============\n")
        });

        client.write('put ' + servfile + '\ncontent-length:' + fileSize + '\n\n');// + content);
        if(method == SYNC_METHOD_UPLOAD) {
            var fileStream = fs.createReadStream(localFile);
            fileStream.pipe(client);
        }

    }

    function getConnectClient(item, servfile, localFile, fileSize) {

        return client = net.connect({ host: item.host, port: item.port },

          function() { //'connect' listener
              client.write('put ' + servfile + '\ncontent-length:' + fileSize + '\n\n');// + content);
              var fileStream = fs.createReadStream(localFile);
              fileStream.pipe(client);

          }
        );
    }


    //格式化时间戳
    function formatDate(now) {
        var year = now.getFullYear();
        var month = now.getMonth() + 1;
        var date = now.getDate();
        var hour = now.getHours();
        var minute = now.getMinutes();
        var second = now.getSeconds();
        return year + "-" + month + "-" + date + " " + hour + ":" + minute + ":" + second;
    }

    function printSyncAllHeader(isHeader) {
        if (isHeader == undefined || isHeader) {
            console.log("==============================================");
            console.log("*            start sync all start");
        } else {
            console.log("*            sync all file end");
            console.log("*");
            console.log("*            start watch file change");
            console.log("==============================================");
        }
    }


    function getItemConfig(argv) {
        var pwd = process.cwd();
        pwd = pwd.replace(/\\/g, '/');

        console.dir(pwd);

        var minFileSize = 1;
        item = config.path[argv.d];
        item.maxFileSize = parseInt(item.maxFileSize)
        item.offline = pwd;
        if (!item.host) {
            item.host = config.server.host;
        }
        if (!item.port) {
            item.port = config.server.port;
        }
        if(!item.maxFileSize) {
            item.maxFileSize = config.server.maxFileSize
        }
        if(1 > item.maxFileSize ) {
            item.maxFileSize = 14
        }
        item.maxFileSize = item.maxFileSize * 1024*1024;
        item.devSyncAll = argv.devSyncAll;
        item.online = item.online.replace(/\\/g, '/');
        if (item.ignore !== undefined) {
            for (var index = 0; index < item.ignore.length; index++) {
                item.ignore[index] = item.ignore[index].trim().replace(/^\/+/g, "").replace(/\/+$/g, "");
            }
        } else {
            item.ignore = [];
        }
        if (item.sh == undefined) {
            item.sh = '';
        }

        return item;
    }



    return function() {
        var options = process.argv
        var len = options.length;
        var i;
        var argv = { 'd': '', 'v': '', 'devSyncAll': false, "v": false }

        for (i = 0; i < len; i++) {
            if (options[i] == '-d') {
                argv.d = options[++i];
            } else if (options[i] == 'all') {
                argv.devSyncAll = true;
            } else if (options[i] == "-v") {
                argv.view = true;
            }
        }


        if (argv.d == null) {
            console.log("error: 请输入config配置中path配置项目");
            return;
        }


        if (argv.d != '') {

            if (config.path.hasOwnProperty(argv.d)) {
                item = getItemConfig(argv)

                if (argv.view == true) {
                    if (item.sh == '') {
                        console.log("error: no configure execute file ");
                    } else {
                        console.log("\n    \nhttp://" + item.host + ":" + item.port + item.sh + "\n");
                    }
                    return;
                }

                process.setMaxListeners(0);

                if (argv.devSyncAll) {
                    printSyncAllHeader(true);
                    getSyncFILE(item.offline, item);
                    syncAllStartProcess(item);
                } else {
                    console.log("==============================================");
                    console.log("*            start watch file change");
                    console.log("==============================================");
                    //只监控，不同步目录下文件
                    watchDir(item.offline, item, false);
                }

            } else {
                console.log("error:" + argv.d + "配置不存在");
            }

        } else {
            console.log("error:错误的工作方式");
        }
    }



})();



fileWatcher();