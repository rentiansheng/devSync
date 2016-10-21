# devSync
---
the local machine and develop machine synchronization code


#本地代码同步到开发机
---

**target：write code，view result**

优点：

	1. 不依赖网络，断网开发不受影响，只是无法上传代码到服务器，连上服务器可以触发修改重新同步

	2. 所有文件存在本地，操作流畅。

	3. 多个项目分离，不受影响

	4. 不会删除服务端文件

	5. 部分目录可以忽略同步


####准备工作
---


整个工具有两部分组成 Client和Server；
	
	client 客户端放在个人电脑上，也就是装有IDE的机器上
	server 服务端放在开发机器，也就是公司提供的开发机器上，通俗饿说就是装nginx的机器上


 


#### 安装
---

$pwd 代表当前路径


1. client 
	
	安装nodejs
	
2. server

	需要是Linux2.4以上的版本，需要支持epoll
	安装gcc，makefile
	
3. 下载devSync代码到client和server的服务器

	 	git clone https://github.com/rentiansheng/devSync.git devSync
 		cd devSync
 		
4. 安装服务端
	
		cd $pwd/devSync/server/
 		make
 		
 		启动服务端
 		$pwd/devSync/server/obj/devSync
		
	

 	
     
   
5. 安装客户端
 	
 		
 		
 		cd $pwd/devSync/client/
 	
 		Linux 客户端安装
 		设置alias
 	
 		alias devSyncd = "node /Users/reage/devSync/client/watch.js -d"
 		alias devSync = "node /Users/reage/devSync/client/watch.js" 
 		
 		Windows 客户端安装
 		cd C:\Windows\System32\
 		新建devSyncd.bat,devSync.bat两个文件
 		
 		devSyncd.bat 输入
 		@echo off
		node C:\Users\reage\devSync\client\watch.js -d %1 %2 %4 %5 %6 %7 
		
		devSync.bat 输入
 		@echo off
		node C:\Users\reage\devSync\client\watch.js %1 %2 %4 %5 %6 %7 %8
		
 	




#### Client放在本地有nodejs开发
---
 **依赖nodejs。建议装比较搞的版本，避免监控文件过多，出现错误**
 
 **启动参数 all 表示同步所有文件**
 **启动参数 -d 监控当前目录下（运行client的目录），当clietn的文件修改同步到server**
 
 
#### client 配置
---

Client的配置文件放在$pwd/client/config.json，config.json 是个object，其中包含server，path等配置,


	 server 配置开发机器的默认的host和端口
	 path  开发机固目录相关配置
	 
 
#####path 配置详细
---
 
 path 是个数组，每个数据项是同步的配置。每个数据项包含下面的值。
 
  	offline：本地需要同步到开发机器的代码路径
  	exts: 可以同步的文件后缀
  	ignore: 不需要同步的目录
  	host: 开发机地址
	port: 端口号

 使用方法
 
 	cd 代码目录
 	node  $pwd/client/watch.js -d audit  //audit 表示使用path的那个配置
 	node  $pwd/client/watch.js all -d audit //audit 表示使用path的那个配置,all表示将本地所有文件同步到服务器
 	
 	等同于
 	Linux,Mac 使用
	 	devSync -d audit
	 	devSyncd audit 
	 	devSync -d audit all
	 	devSyncd audit  all 
	 windows
	 	在cmd中使用
	 	devSync.bat -d audit
	 	devSyncd.bat audit
	 	devSync.bat -d audit all
	 	devSyncd.bat audit  all 

 	
 	



### Server放在开发机有C语言写
---

 **只能在支持epoll的系统使用**
 
 **-p port 可选 默认使用8484端口，提供服务的端口，就是client端中config.json中配置的port的值**
 
 **-u user 可选，默认使用当前运行程序的账户，设置启动server端后运行程序的用户。影响写入文件和对目录的权限，一定要root或者sudo执行服务端**


 用C语言编写，需要自己编译，进入到server目录下make就行了

 主要要根据自己的情况修改 server/http/reagehttp.c文件中第11行，conf.port来确定端口。

 也可以通过运行函数的时候使用参数自定端口号  -p port   (2015-11-04):Eg: obj/devSync -p 8485

 新加-u参数，指定运行的帐户，使用-u参数一定要root或者sudo执行服务端(2016-7-15)。Eg: obj/devSync -u www


 缺少日志

 使用方法

 	cd obj
 	./devSync
 	./devSync -p 8088 //使用8088端口
 	./devSync -u www //使用www用户运行程序

 
### 注意
---
 
  **文件目录较多时候，容易出现问题，请设置可以打开的文件个数 ulimit -n 1024**

  **同步工具并不会删除服务端的文件**

  **服务端当目录不存在的时候会新建目录**

  **当client端中文件修改保存的时候，client端会将整个文件的内容发送server端，server将重新写入整个文件**

 


