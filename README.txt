## ANet
（1）实现服务器通信进程，可接受和处理客户端，并将用户请求记录输出至控制台
（2）构建服务器任务队列，管理来自客户端的任务请求，链接通信线程和工作线程
（3）实现服务器工作线程池，基于任务的调度和任务处理
（4）实现客户端程序，用于测试服务器通信和处理性能，需要在客户端控制台输出服务器的任务响应时间，响应数据等信息
（5）建立客户机的管理机制，实现客户机的身份认证和辨识
（6）可采用Linux下的Epoll等模型实现对多个连接的管理
（7）客户机采用文件传输请求，测试服务器的并发性能

- bugs
[Solved]accept() error with Bad file descriptor
[Solved]buffer overflow
[Solved]data receive not completed
[] can't write into file
[Solved]too many CPU usage


