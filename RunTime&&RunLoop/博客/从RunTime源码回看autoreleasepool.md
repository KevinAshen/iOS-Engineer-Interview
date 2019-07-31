[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- autoreleasepool中就涉及了autorelease，应该是内存管理中比较难理解的一个方法
- 本文将分析其中的源码，理解pool中的存储数据结构，release时机等问题

# 准备工作

- 请准备好750.1版本的objc4源码一份【目前最新的版本】，打开它，找到文章中提到的方法，类型，对象
- 一切请以手中源码为准，不要轻信任何人，任何文章，包括本篇博客
- 文章中的源码都请过了我的删改，建议还是先看看源码
- 源码建议从Apple官方开源网站获取[obj4](https://opensource.apple.com/tarballs/objc4/objc4-750.1.tar.gz)
- 官网上下载下来需要自己配置才能编译运行，如果不想配置，可以在[RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)中clone

# 最熟悉的陌生人：@autoreleasepool

- 我们每次编写iOS程序都会有main.m文件，但我们从来没有操作过其中的代码
- 里面的@autoreleasepool我们已经知道是代替了MRC中的autorelease操作，那么到底是怎么实现的呢？
## 补充知识：RunLoop概念
- 虽说不深入RunLoop，大概概念还是提一下
- 在编写C语言程序的时候，我们执行完主函数就会return 0，程序就跑完了
- 但是作为一个iOS应用，明显是不应该有"跑完"这回事的，我们就可以将RunLoop视作一个不停跑的死循环
- 放到主函数的问题，其实iOS里的主函数从来不会真正的return哦
## clang编译
- 新建iOS项目，通过clang -rewrite-objc main.m指令得到mian.cpp

- 搜索UIApplicationMain
```objective-c
int main(int argc, char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 
        return UIApplicationMain(argc, argv, __null, NSStringFromClass(((Class (*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("AppDelegate"), sel_registerName("class"))));
    }
}

//__AtAutoreleasePool结构体就是__autoreleasepool的本质
//查看__AtAutoreleasePool
struct __AtAutoreleasePool {
  __AtAutoreleasePool() {atautoreleasepoolobj = objc_autoreleasePoolPush();}
  ~__AtAutoreleasePool() {objc_autoreleasePoolPop(atautoreleasepoolobj);}
  void * atautoreleasepoolobj;
};
```
- 就如《高级编程》上的一样，这里主要操作就是一个push，一个pop
- 下面首先来看下相关的数据结构

# AutoreleasePoolPage

## find it！
- 打开runtime源码工程，搜索objc_autoreleasePoolPush，再点进AutoreleasePoolPage
- push以及pop都是AutoreleasePoolPage这个类的方法
## 数据结构
```objective-c
    static size_t const SIZE = PAGE_MAX_SIZE; //定义内存大小 4096
    magic_t const magic; // 16字节
    id *next; // 8字节 
    pthread_t const thread; // 8字节 当前pool所处的线程
    // 前一个节点
    AutoreleasePoolPage * const parent; // 8字节
    // 后一个节点
    AutoreleasePoolPage *child; // 8字节
    uint32_t const depth; // 4字节 page的深度，首次为0，以后每次初始化一个page都加1。
    uint32_t hiwat; // 4字节 这个字段是high water的缩写，这个字段用来计算pool中最多存放的对象个数。在每次执行pop()的时候，会更新一下这个字段。
    //剩余的内存就是储存注册到pool里的对象地址
```
- 一个AutoreleasePoolPage占据4096个字节，扣除56个字节存储上面的信息外，其余的都用来存储加入到page中的对象地址

![page数据结构](http://ww3.sinaimg.cn/large/006tNc79ly1g542m1xp73j308z0cimxw.jpg)

- 根据parent以及child，看出该栈的实现方式是双向链表

![page双向链表](http://ww2.sinaimg.cn/large/006tNc79ly1g542n6mu93j30t205rdgd.jpg)

- 关于pool与page的关系我花了一个大致的草图

![931FBE99B832A981E54CD4908DC16A6A](http://ww4.sinaimg.cn/large/006tNc79ly1g542s8cdv5j31fk0t00yo.jpg)


- 了解完数据结构我们开始着重分析push与pop的实现

# push
## void *push() 
```objective-c
// push方法
    static inline void *push() 
    {
        id *dest;
        
        if (DebugPoolAllocation) {
            // 区别调试模式
            dest = autoreleaseNewPage(POOL_BOUNDARY);
            // POOL_BOUNDARY就是nil
        } else {
            dest = autoreleaseFast(POOL_BOUNDARY);
            //POOL_BOUNDARY就是nil
        }
        return dest;
    }
```
- 其中以Debug 开头的说明是调试模式的内容，会统统忽略
- 先来研究下这个POOL_BOUNDARY是起什么作用的

## 哨兵对象POOL_BOUNDARY详解

- POOL_BOUNDARY直译过来就是POOL的边界
- 首先这个POOL_BOUNDARY就是nil，它的作用是隔开page中的对象
- 假如我们执行了以下代码
```objective-c
push(A, B, C)
  push2(D, E, F)
    push3(G, H, I)
    
    pop1 //pop了G, H, I
  pop2 //pop了D, E, F
pop3 //pop了A, B, C
```
- 也就是说我们pop的时候被pop的最近一次push进来的对象
- 但是并不是每次push与pop之间存进的对象都刚好占满一个page，可能会不满，可能会超过
- 因此这个POOL_BOUNDARY帮助我们分隔每个@autoreleasepool块之间的对象【这也是叫POOL_BOUNDARY的原因】
- 我们看一张图，将其中的POOl_SENTINEL改成POOL_BOUNDARY

![pop](http://ww2.sinaimg.cn/large/006tNc79ly1g543del502j30yg0eet9l.jpg)
- 会看上面push的代码，其中其实就是调用了autoreleaseFast（POOL_BOUNDARY）
- 此外请注意，push操作是不带参数的，等于就是初始化的时候调用一次，真正压入对象的方法是
```objective-c
    static inline id autorelease(id obj)
    {
        assert(obj);
        assert(!obj->isTaggedPointer());
        id *dest __unused = autoreleaseFast(obj);
        assert(!dest  ||  dest == EMPTY_POOL_PLACEHOLDER  ||  *dest == obj);
        return obj;
    }
```
- 也就是说push调用的autoreleaseFast参数只有POOL_BOUNDARY，请记住
## autoreleaseFast
```objective-c
static inline id *autoreleaseFast(id obj)
    {
        // hotPage就是当前正在使用的AutoreleasePoolPage
        //即获取当前在使用的page
        AutoreleasePoolPage *page = hotPage();
        if (page && !page->full()) {
            // 有hotPage且hotPage不满，将对象添加到hotPage中
            return page->add(obj);
        } else if (page) {
            // 有hotPage但是hotPage已满
            // 使用autoreleaseFullPage初始化一个新页，并将对象添加到新的AutoreleasePoolPage中
            return autoreleaseFullPage(obj, page);
        } else {
            // 无hotPage
            // 使用autoreleaseNoPage创建一个hotPage,并将对象添加到新创建的page中
            return autoreleaseNoPage(obj);
        }
    }
```
- 模拟第一次push，无hotPage，看autoreleaseNoPage
### autoreleaseNoPage
```objective-c
    id *autoreleaseNoPage(id obj) {
        bool pushExtraBoundary = false;
        AutoreleasePoolPage *page = new AutoreleasePoolPage(nil);
        // 将初始化的AutoreleasePoolPage设置成hotPage
        setHotPage(page);
        // Push the requested object or pool.
        // 将对象添加到AutoreleasePoolPage中
        return page->add(obj);
    }

```
- 简化完就是创建一个page，设置为HotPage
- 将obj添加到其中
### autoreleaseFullPage
```objective-c
id *autoreleaseFullPage(id obj, AutoreleasePoolPage *page)
    {
        do {
            // 如果page->child不为空，那么使用page->child
            if (page->child) page = page->child;
            // 否则的话，初始化一个新的AutoreleasePoolPage
            else page = new AutoreleasePoolPage(page);
        } while (page->full());
        // 将找到的合适的page设置成hotPage
        setHotPage(page);
        // 将对象添加到hotPage中
        return page->add(obj);
    }
```
- 如果page有child，就使用它来存放
- 如果没有，和上面一样，新建一个来存放
# pop
```objective-c
static inline void pop(void *token) 
    {
        AutoreleasePoolPage *page;
        id *stop;
        page = pageForPointer(token);
        stop = (id *)token;
        page->releaseUntil(stop);
        // memory: delete empty children
        if (DebugPoolAllocation  &&  page->empty()) {

        } 
        else if (page->child) {
            if (page->lessThanHalfFull()) {
                page->child->kill();
            }
            else if (page->child->child) {
                page->child->child->kill();
            }
        }
    }
```

## token详解
- 先来看下Apple在Autorelease pool implementation中写的注释
```objective-c
/***********************************************************************
 自动释放池实现
 
 一个线程的自动释放池是一个指针堆栈。
每个指针要么指向要被释放的对象，要么是POOL_BOUNDARY说明一个pool的边界

token是指向该pool的POOL_BOUNDARY的指针。什么时候池被pop，所有比哨兵hotter的物体都被释放。

pool被分成一个双向指针构成的pages。pages在必要的时候被添加和删除
线程本地存储指针指向hot page，在这里新被autoreleased的objects被存储


   Autorelease pool implementation

   A thread's autorelease pool is a stack of pointers. 
   Each pointer is either an object to release, or POOL_BOUNDARY which is 
     an autorelease pool boundary.
   A pool token is a pointer to the POOL_BOUNDARY for that pool. When 
     the pool is popped, every object hotter than the sentinel is released.
   The stack is divided into a doubly-linked list of pages. Pages are added 
     and deleted as necessary. 
   Thread-local storage points to the hot page, where newly autoreleased 
     objects are stored. 
**********************************************************************/
```
- 这里就讲清楚了toekn本质就是指向POOL_BOUNDARY的指针，存储着每次push时插入的POOL_BOUNDARY的地址
- 请注意，只有第一次push的时候会在page中插入一个POOL_BOUNDARY【或者是中间page刚好满了，要使用新的page了】，并不是page的开头都一定是POOL_BOUNDARY
- 具体是怎么做到的，请注意下push方法返回的dest，一直查看下他的来源，你会有收获的

## releaseUntil

- 这个方法顾名思义，就是将对象一直release，一直到stop【token】
```objective-c
void releaseUntil(id *stop) 
    {
        // 释放AutoreleasePoolPage中的对象，直到next指向stop
        while (this->next != stop) {
            // Restart from hotPage() every time, in case -release 
            // autoreleased more objects
            // hotPage可以理解为当前正在使用的page
            AutoreleasePoolPage *page = hotPage();
            // fixme I think this `while` can be `if`, but I can't prove it
            //卑微工程师
            while (page->empty()) {
                page = page->parent;
                setHotPage(page);
            }
            page->unprotect();
            // obj = page->next; page->next--;
            id obj = *--page->next;
            memset((void*)page->next, SCRIBBLE, sizeof(*page->next));
            page->protect();
            // POOL_BOUNDARY为nil，是哨兵对象
            if (obj != POOL_BOUNDARY) {
                // 释放obj对象
                objc_release(obj);
              //一直是放到结束
            }
        }
        // 重新设置hotPage
        setHotPage(this);
    }
```
### 卑微工程师
- 这里有不知道哪年的Apple开发人员留下的一句注释// fixme I think this `while` can be `if`, but I can't prove it
- 我非常赞同他的观点，但也忍不住有些悲哀
- 所谓程序员的悲哀，大概就是想改个if都做不到吧。
## kill()
- releaseUntil把page里的对象进行了释放，但是page本身也会占据很多空间，要通过kill()来处理
```objective-c
  // 删除双向链表中的每一个page
    void kill() 
    {
        // Not recursive: we don't want to blow out the stack 
        // if a thread accumulates a stupendous amount of garbage
        AutoreleasePoolPage *page = this;
        // 找到链表最末尾的page
        while (page->child) page = page->child;

        AutoreleasePoolPage *deathptr;
        // 循环删除每一个page
        do {
            deathptr = page;
            page = page->parent;
            if (page) {
                page->unprotect();
                page->child = nil;
                page->protect();
            }
            delete deathptr;
        } while (deathptr != this);
    }
```
- 具体实现不是很重要，我们关注下kill的调用情况
## kill()的调用
```objective-c
if (page->lessThanHalfFull()) {
    page->child->kill();
    }
    else if (page->child->child) {
           page->child->child->kill();
}
```

- 如果当前page小于一半满，则把当前页的所有孩子都杀掉，否则，留下一个孩子，从孙子开始杀。
- 为什么呢？Apple假设，当前page一半都没满，说明已经够了，把接下来的全kill，如果超过一半，就认为下一页还有存在的必要，所以从孙子开始杀
- 这样子我认为是为了节约创建page操作

# 完整流程

![22F8A1FE520219CFB02FA1DBD1D29698](http://ww1.sinaimg.cn/large/006tNc79ly1g5450zlusvj31fk0t0462.jpg)
