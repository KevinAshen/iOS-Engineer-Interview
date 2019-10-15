[TOC]

# 下载模块【基于NSURLSession封装而成的下载管理类】

## 相关宏定义【KADownloadManager】

- KADownloadDirectory
```objective-c
#define KADownloadDirectory self.downloadedFilesDirectory ?: [[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject]] stringByAppendingPathComponent:NSStringFromClass([self class])]

//先判断self.downloadedFilesDirectory属性有没有被初始化【懒加载】
//NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserdomainMask，YES)获取到该应用程序的私有目录
//stringByAppendingPathComponent:NSStringFromClass([self class])添加该APP名【也就是KADownloadManager，管理类名与APP名相同】
//同时要注意这里要使用stringByAppendingPathComponent方法，如果使用普通的会导致出现缺少“/”符号
```
- KAFileName(URL)

```objective-c
#define KAFileName(URL) [URL lastPathComponent] 
//用URL的最后的路径名作为文件名
//e.g. 
//http://yxfile.idealsee.com/9f6f64aca98f90b91d260555d3b41b97_mp4.mp4
//文件名就是9f6f64aca98f90b91d260555d3b41b97_mp4.mp4
//文件名不仅用来作为下载的文件名，同时也是管理下载总量的plist文件中的key值
```

- KAFilePath(URL)

```objective-c
#define KAFilePath(URL) [KADownloadDirectory stringByAppendingPathComponent:KAFileName(URL)] 
//获取到文件的具体路径
```

- KAFilesTotalLengthPlistPath

```objective-c
#define KAFilesTotalLengthPlistPath [KADownloadDirectory stringByAppendingPathComponent:@"KAFilesTotalLength.plist"]  
//获取存放所有下载文件长度的plist文件
```

## KADownloadModel

- 先看相对简单的Model模块
- 这一块的目标就是把每一个下载任务的相关信息都放在一个model里存储
- 并且在里面定义了三个块属性【这一部分要细看，是怎么封装起来的】

### 下载状态枚举

```objective-c
/**
 下载状态枚举

 - KADownloadStateWaiting: 等待中
 - KADownloadStateRunning: 正在运行
 - KADownloadStateSuspended: 暂停
 - KADownloadStateCanceled: 取消
 - KADownloadStateCompleted: 完成
 - KADownloadStateFailed: 失败
 */
typedef NS_ENUM(NSInteger, KADownloadState) {
    KADownloadStateWaiting,
    KADownloadStateRunning,
    KADownloadStateSuspended,
    KADownloadStateCanceled,
    KADownloadStateCompleted,
    KADownloadStateFailed
};
```

- 状态是model的重要标示符，由于下载可以暂停，取消，删除等等，在做不同的操作时，不仅后台要进行管理，在UI界面也要进行更新

### 基本属性

- downloadURL：对应的下载URL
- downloadDataTask：对应的downloadDataTask，需要借助该属性进行resume，cancel，suspend等操作
- outputStream：执行将请求到的资源写入文件的操作
- totalLength：预期下载长度【即资源总下载长度】

### 块

```objective-c
/**
 状态改变时调用的block

 @param state 下载状态枚举值
 */
typedef void(^StateUpdate)(KADownloadState state);

/**
 过程更新是调用的block，与session的代理相关

 @param receivedSize 已经下载的文件大小
 @param expectedSize 运气要下载的文件大小
 @param progress 过程，用在显示下载进度
 */
typedef void(^ProgressUpdate)(NSInteger receivedSize, NSInteger expectedSize, CGFloat progress);

/**
 下载完成后调用的block

 @param isSuccess 下载是否成功
 @param filePath 下载的文件路径
 @param error 下载错误
 */
typedef void(^CompletionDone)(BOOL isSuccess, NSString *filePath, NSError *error);

@property (nonatomic, copy) StateUpdate stateUpdate;
@property (nonatomic, copy) ProgressUpdate progressUpdate;
@property (nonatomic, copy) CompletionDone completionDone;
```

- 这三个块我目前理解就是实现在Manager层和View层的通信问题
- 块的声明写在model里，块的调用放在KADownloadManager中的Session协议里
- 而块的实现放在ViewController里，进行UI上的更新
- 这样就实现了在下载任务过程中，随着协议方法不断调用，数据不断的写入，UI界面不断被刷新

## KADownloadManager

- 关于manager，由于其中的方法，属性比较多，我在介绍完一些基本的内容后，以建立一次下载为例子看一下具体会走过哪些方法
- 由于要实现断点续传，所以我们通过plist文件存储资源的预计下载长度，其已下载长度通过读取文件大小获得
- 为什么能这么狂呢，因为这个demo我们直接把三个URL写死了，如果我们要写的是一个在线项目，显然需要把URL也本地化存起来
- 获取预计下载长度是在调用didReceiveResponse也就是第一个协议方法里获取的，之后resume task不会再走该方法，所以需要将其本地化

### 三个属性

```objective-c
//包括下载中以及等待下载的model
@property (nonatomic, strong) NSMutableDictionary *downloadModelMutDic;
//包括现在在下载的model
@property (nonatomic, strong) NSMutableArray *downloadingModelMutArr;
//包括等待下载的model
@property (nonatomic, strong) NSMutableArray *waitingModelMutArr;
```

- downloadModelMutDic负责纵览要下载的model，包括正在下载和等待下载【下载完的不再里面】
- downloadingModelMutArr以及waitingModelMutArr模拟了队列，实现下载先进先出，由于有多线下载以及暂停继续等操作，我们要进行管理，知道下面一个进行下载的任务是哪一个

### 流程图

![KADownloadManager](https://tva1.sinaimg.cn/large/006y8mN6gy1g7ovevepr1j33a40u0qi1.jpg)

- [幕布链接](https://mubu.com/doc/aYrf0eSgFw)