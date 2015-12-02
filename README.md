# devSync
the local machine andmachine synchronization code


#本地代码同步到开发机

有两部分组成 Client和Server；

### Client放在本地有nodejs开发

 **依赖nodejs。**

 Client 需要配置 config.json，config.js 是个数组，其中包含local，server，
 server 需要配置开发机器的 host和端口
 local 是个数据，每个数据项是一个开发机和本地机器同步的配置。每个数据项包含offline,online,exts,autos四个只。

 offline：本地需要同步到开发机器的代码路径

 online： 开发机器的线上路径

 exts: 可以同步的文件后缀

 auto: 是否开启自动同步




### Server放在开发机有C语言写

 **只能在支持epoll的系统使用**

 用C语言编写，需要自己编译，进入到server目录下make就行了

 主要要根据自己的情况修改 server/http/reagehttp.c文件中第13行，conf.port来确定端口。

 也可以通过运行函数的时候使用参数自定端口号  -p port   (2015-11-04)

 目前 没有写守护进程。想后台运行用nohup和&组合吧
 
 
 ### 注意
 
  **文件目录较多时候，容易出现问题，请设置可以打开的文件个数 ulimit -n 1024 **
