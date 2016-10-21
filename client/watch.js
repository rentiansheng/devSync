var fs = require('fs');
var path = require('path');
var config = require('./config.json');
var net = require('net');



var fileWatcher = (function() {

    var count = 0;
    var allfile = [];
    var filecount = 0;
    var sendFileNo = 0;
    var displayEnd = 1;
    var MaxFileSize = 4000000; //4m

    //发送文件
    function sendFile(dir, filename, item, display) {
        if (filename && filename[0] == '.') return;
        var ext = path.extname(filename);
        if (item.exts && item.exts.length > 0 && item.exts.indexOf(ext) === -1) return;
        fs.readFile(dir + '/' + filename, function(err, content) {
            if (err) { return false; }
            var relativePath = path.relative(item.offline, dir);
            relativePath = relativePath.replace(/\\/g, '/');
            var servfile = item.online + '/' + relativePath + '/' + filename;
            var start = new Date().getTime();
            try {
                if (display == 1) {
                    console.log("\nsync start: file[ " + servfile + ' ]');
                }
                var client = net.connect({ host: item.host, port: item.port },
                    function() { //'connect' listener
                        client.write('put ' + servfile + '\n' + content.length + '\n\n' + content);
                    }
                );

                client.on('data', function(data) {
                    console.log(data.toString());
                });
                client.on('end', function() {
                    client.end();
                    if (display == 1) {
                        console.log('sync file end: fileName[ ' + servfile + ' ]');
                        var end = new Date().getTime();
                        var ts = end - start;
                        console.log('sync file end: file length[ ' + content.length + 'b ]');

                        var strStartTime = formatDate(new Date(start));

                        console.log('sync file end: start time[ ' + strStartTime + ' ]  sync use time[ ' + ts + 'ms ]');
                        console.log("\n");

                    }

                });
                client.on('error', function(err) {
                    console.log(err);
                });
            } catch (err) {
                console.log(err);
            }

        });
    }


    //检测目录变化
    function listenToChange(dir, item) {
        dir = path.resolve(dir);

        function onChg(event, filename) {

            fs.lstat(dir + '/' + filename, function(err, stats) {
                if (err) { return; }
                if (stats.isDirectory()) {
                    watchDir(dir + '/' + filename, item);
                } else if (stats.isFile()) {
                    if (stats.size < MaxFileSize) {
                        sendFile(dir, filename, item, 1);
                    } else {
                        console.log("\n\n\n     You can not synchronize large files! \n\n\n");
                    }
                }

            });
        }

        fs.watch(dir, onChg);
    }

    //为目录加监控时间
    function watchDir(root, item) {

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
                        fileName = file;
                        if (file[0] == '.') { return; }
                        file = root + '/' + file;
                        fs.lstat(file, function(err, stats) {
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
                        allfile[filecount++] = { 'dir': dir, 'name': file['base'], 'item': item };
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
        if (syncItem !== undefined && syncItem && sendFileNo < filecount) {
            sendFileForSyncAll(syncItem.dir, syncItem.name, syncItem.item, rootItem, 0);
            syncItem = undefined;
            //console.log("send "+(++sendFileNo)+"/"+filecount);
        } else {
            if (displayEnd) {
                printSyncAllHeader(false);
                displayEnd = 0;
                watchDir(rootItem.offline, rootItem);
            }



        }

        return;
    }

    //同步全部文件，发送文件
    function sendFileForSyncAll(dir, filename, item, rootItem, retry) {

        if (filename && filename[0] == '.') { syncAllFile(rootItem); return; };
        var ext = path.extname(filename);
        if (item.exts && item.exts.length > 0 && item.exts.indexOf(ext) === -1) { syncAllFile(rootItem); return };
        fs.readFile(dir + '/' + filename, function(err, content) {
            if (err) { return false; }
            var relativePath = path.relative(item.offline, dir);
            relativePath = relativePath.replace(/\\/g, '/');
            var servfile = item.online + '/' + relativePath + '/' + filename;


            var client = net.connect({ host: item.host, port: item.port },
                function() { //'connect' listener
                    client.write('put ' + servfile + '\n' + content.length + '\n\n' + content);
                    syncAllFile(rootItem);
                }
            );

            client.on('error', function(error) {
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
                client.end();

            });


        });
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



    return function() {
        var options = process.argv
        var len = options.length;
        var i;
        var argv = { 'd': '', 'v': '', 'devSyncAll': false }

        for (i = 0; i < len; i++) {
            if (options[i] == '-d') {
                argv.d = options[++i];
            } else if (options[i] == 'all') {
                argv.devSyncAll = true;
            }
        }



        if (argv.d == null) {
            console.log("error: 请输入config配置中path配置项目");
            return;
        }


        if (argv.d != '') {
            var pwd = process.cwd();
            pwd = pwd.replace(/\\/g, '/');

            console.dir(pwd);
            if (config.path.hasOwnProperty(argv.d)) {
                item = config.path[argv.d];
                item.offline = pwd;
                if (!item.host) {
                    item.host = config.server.host;
                }
                if (!item.port) {
                    item.port = config.server.port;
                }
                item.devSyncAll = argv.devSyncAll;
                item.online = item.online.replace(/\\/g, '/');
                if (item.ignore !== undefined) {
                    for (var index = 0; index < item.ignore.length; index++) {
                        item.ignore[index] = item.ignore[index].trim().replace(/^\/+/g, "").replace(/\/+$/g, "");
                    }
                } else {
                    item.ignore = [];
                }

                if (argv.devSyncAll) {
                    printSyncAllHeader(true);
                    getSyncFILE(item.offline, item);

                    syncAllFile(item);
                } else {
                    console.log("==============================================");
                    console.log("*            start watch file change");
                    console.log("==============================================");
                    watchDir(item.offline, item);
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