# devSync
---
the local machine andmachine synchronization code


#本地代码同步到开发机
---
有两部分组成 Client和Server；

### Client放在本地有nodejs开发
---
 **依赖nodejs。建议装比较搞的版本，避免监控文件过多，出现错误**
 
#### client 配置

Client 需要配置 config.json，config.json 是个数组，其中包含server，local，path等配置

	 server 需要配置开发机器的 host和端口
	 local 配置本地固定目录方式同步
	 path 同步到服务器固定目录
	 



#### 本地固定目录同步

**适用长期开发使用**

  通过local配置本地目录和服务器目录来同步数据的。
  
  local 是个数组，每个数据项是一个开发机和本地机器同步的配置。每个数据项包含offline,online,exts,autos四个值。

 offline：本地需要同步到开发机器的代码路径

 online： 开发机器的线上路径

 exts: 可以同步的文件后缀

 auto: 是否开启自动同步
 
 
 
	 **使用方法 直接 node watch.js  **
 
 
####同步到服务器固定目录


 将运行程序的目录数据改动同步到path配置的开发机的路径中
 
 path 是个数组，每个数据项是同步的配置。每个数据项包含online,exts两个值。
 
  offline：本地需要同步到开发机器的代码路径
  exts: 可以同步的文件后缀

 使用方法
 
 	cd 代码目录
 	node  /client目录/watch.js -d audit //audit 表示使用path的那个配置
 	



### Server放在开发机有C语言写
---
 **只能在支持epoll的系统使用**

 用C语言编写，需要自己编译，进入到server目录下make就行了

 主要要根据自己的情况修改 server/http/reagehttp.c文件中第13行，conf.port来确定端口。

 也可以通过运行函数的时候使用参数自定端口号  -p port   (2015-11-04)

 //目前 没有写守护进程。想后台运行用nohup和&组合吧 (2015-12-08已经加上daemon)

 未区分错误和正常日志，日志统一放在/tmp/devSync.log中(2015-12-23)//缺少日志


 
### 注意
 
  **文件目录较多时候，容易出现问题，请设置可以打开的文件个数 ulimit -n 1024**

 
