[TOC]

# 解析Block.private

- 在Block.private文件里面会解释真·block源码
- 包括block的拷贝究竟是怎么实现的，block的结构为什么会变来变去等等就能得到解答
- 一如block深似海，发现private才知刚下水

## Block的定义

```c++
// Values for Block_layout->flags to describe block objects
enum {
    BLOCK_DEALLOCATING =      (0x0001),  // runtime
    BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime
    BLOCK_NEEDS_FREE =        (1 << 24), // runtime
    BLOCK_HAS_COPY_DISPOSE =  (1 << 25), // compiler
    BLOCK_HAS_CTOR =          (1 << 26), // compiler: helpers have C++ code
    BLOCK_IS_GC =             (1 << 27), // runtime
    BLOCK_IS_GLOBAL =         (1 << 28), // compiler
    BLOCK_USE_STRET =         (1 << 29), // compiler: undefined if !BLOCK_HAS_SIGNATURE
    BLOCK_HAS_SIGNATURE  =    (1 << 30), // compiler
    BLOCK_HAS_EXTENDED_LAYOUT=(1 << 31)  // compiler
};

#define BLOCK_DESCRIPTOR_1 1
struct Block_descriptor_1 {
    uintptr_t reserved;
    uintptr_t size;
};

#define BLOCK_DESCRIPTOR_2 1
struct Block_descriptor_2 {
    // requires BLOCK_HAS_COPY_DISPOSE
    BlockCopyFunction copy;
    BlockDisposeFunction dispose;
};

#define BLOCK_DESCRIPTOR_3 1
struct Block_descriptor_3 {
    // requires BLOCK_HAS_SIGNATURE
    const char *signature;
    const char *layout;     // contents depend on BLOCK_HAS_EXTENDED_LAYOUT
};

struct Block_layout {
    void *isa;
    volatile int32_t flags; // contains ref count
    int32_t reserved;
    BlockInvokeFunction invoke;
    struct Block_descriptor_1 *descriptor;
    // imported variables
};
```

- 画了下其对应的结构：

- ![为了拥有更好的博客而画图.001](https://tva1.sinaimg.cn/large/006y8mN6ly1g741yegu9kj30sg0lcn7b.jpg)

  - 在上面的结构里虽然Block_layout里只有一个Block_descriptor_1，但实际上会根据设定的flag值增加Block_descriptor_2或者Block_descriptor_3，但是在上面的结构里虽然Block_layout里只有一个Block_descriptor_1是一定会有的
  
- Block_layout结构体成员含义如下：
  - isa: 所属类指针，说明block的类型，就是`_NSConcreteStackBlock`，`_NSConcreteMallocBlock`，`_NSConcreteGlobalBlock`这几个，说明OC本身也是一个对象。
  - flags: 标志变量
  
  ```c++
  enum {
    	BLOCK_DEALLOCATING =      (0x0001),  // runtime
      BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime	用来标识栈Block
      BLOCK_NEEDS_FREE =        (1 << 24), // runtime	用来标识堆
      BLOCK_HAS_COPY_DISPOSE =  (1 << 25), // compiler compiler 含有copy_dispose助手	如果标记为BLOCK_HAS_COPY_DISPOSE代表含有copy_dispose助手，说明该lay_out拥有Block_descriptor_2
      BLOCK_HAS_CTOR =          (1 << 26), // compiler: helpers have C++ code
      BLOCK_IS_GC =             (1 << 27), // runtime
      BLOCK_IS_GLOBAL =         (1 << 28), // compiler 是否为全局Block
      BLOCK_USE_STRET =         (1 << 29), // compiler: undefined if !BLOCK_HAS_SIGNATURE
      BLOCK_HAS_SIGNATURE  =    (1 << 30), // compiler	判断是否有签名
      BLOCK_HAS_EXTENDED_LAYOUT=(1 << 31)  // compiler	
  };
  
  // 当然，这几个flag是可以通过与操作组合在一起的
  // 关于签名这一块，参看：NSInvocation动态调用Block
  ```
  
  - Reserved: 保留变量
  - invoke: block执行时调用的函数指针，block内部的执行代码都在这个函数
  - descriptor: block的详细描述，一定包含Block_descriptor_1，在面对不同的情况时，编译器会修改lay_out结构，按需增加
  - variables: block范围外的变量，如果block没有调用任何外部变量，该变量就不存在

## `__block`变量的定义

```c++
// Values for Block_byref->flags to describe __block variables
enum {
    // Byref refcount must use the same bits as Block_layout's refcount.
    // BLOCK_DEALLOCATING =      (0x0001),  // runtime
    // BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime

    BLOCK_BYREF_LAYOUT_MASK =       (0xf << 28), // compiler
    BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler
    BLOCK_BYREF_LAYOUT_NON_OBJECT = (  2 << 28), // compiler
    BLOCK_BYREF_LAYOUT_STRONG =     (  3 << 28), // compiler
    BLOCK_BYREF_LAYOUT_WEAK =       (  4 << 28), // compiler
    BLOCK_BYREF_LAYOUT_UNRETAINED = (  5 << 28), // compiler

    BLOCK_BYREF_IS_GC =             (  1 << 27), // runtime

    BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler
    BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime
};

struct Block_byref {
    void *isa;
    struct Block_byref *forwarding;
    volatile int32_t flags; // contains ref count
    uint32_t size;
};

struct Block_byref_2 {
    // requires BLOCK_BYREF_HAS_COPY_DISPOSE
    BlockByrefKeepFunction byref_keep;
    BlockByrefDestroyFunction byref_destroy;
};

struct Block_byref_3 {
    // requires BLOCK_BYREF_LAYOUT_EXTENDED
    const char *layout;
};

```

- isa
  
  - 指向其所属类，初始为0，运行时动态判断
  
- forwarding
  
  - 指向自身/堆上的复制
  
- flags
  
  - ```c++
    // Values for Block_byref->flags to describe __block variables
    enum {
        // Byref refcount must use the same bits as Block_layout's refcount.
        // BLOCK_DEALLOCATING =      (0x0001),  // runtime
        // BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime
    
        BLOCK_BYREF_LAYOUT_MASK =       (0xf << 28), // compiler
        BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler
        BLOCK_BYREF_LAYOUT_NON_OBJECT = (  2 << 28), // compiler
        BLOCK_BYREF_LAYOUT_STRONG =     (  3 << 28), // compiler
        BLOCK_BYREF_LAYOUT_WEAK =       (  4 << 28), // compiler
        BLOCK_BYREF_LAYOUT_UNRETAINED = (  5 << 28), // compiler
    
        BLOCK_BYREF_IS_GC =             (  1 << 27), // runtime
    
        BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler
        BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime
    };
    ```
  
  - 这个结构和上面基本一样，上面没有见名也能知意
  
  - 值得注意的是这三个：
  
  - ```c++
    BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler	含有layout
    BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler	含有copy_dispose函数，在__block捕获的变量为对象时就会生成copy_dispose函数用来管理对象内存
    BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime	判断是否要释放
    ```
  
- size
  
  - 所占内存大小
  
- Block_byref_2、Block_byref_3

  - Block_byref_2和Block_byref_3用来保存附加信息

# Block_copy以及Block_release的底层实现

- 这部分源码在[runtime.cpp](https://opensource.apple.com/source/libclosure/libclosure-73/runtime.cpp)

## Block_copy

- 在`Block.h`中，Block_copy是如此定义的：

```c++
// Create a heap based copy of a Block or simply add a reference to an existing one.
// This must be paired with Block_release to recover memory, even when running
// under Objective-C Garbage Collection.

// 创建块的基于堆的副本， 或者只需添加对现有块的引用
// 这必须与Block_release配对才能恢复内存， 即使在运行时
// 在Objective-C垃圾收集机制下
BLOCK_EXPORT void *_Block_copy(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
```

- 它的实现部分在runtime.cpp里面：

```c++
// Copy, or bump refcount, of a block.  If really copying, call the copy helper if present.

// 块的复制或凹凸【???】引用计数。如果真的要复制，请调用复制助手（如果存在）
// 应该是说不要显式调用， 不过复制助手也是不能显式调用的
void *_Block_copy(const void *arg) {
    struct Block_layout *aBlock;

    if (!arg) return NULL;
    
    // The following would be better done as a switch statement
  
    // 下面最好使用switch语句
  	// 这是FIXME？
    aBlock = (struct Block_layout *)arg;
    if (aBlock->flags & BLOCK_NEEDS_FREE) {
        // latches on high
        latching_incr_int(&aBlock->flags);
        return aBlock;
    }
    else if (aBlock->flags & BLOCK_IS_GLOBAL) {
        return aBlock;
    }
    else {
        // Its a stack block.  Make a copy.
      	
      	// 这是一个栈上的block，进行拷贝
        struct Block_layout *result =
            (struct Block_layout *)malloc(aBlock->descriptor->size);
        if (!result) return NULL;
        memmove(result, aBlock, aBlock->descriptor->size); // bitcopy first
      
      	// 首先进行位拷贝
#if __has_feature(ptrauth_calls)
        // Resign the invoke pointer as it uses address authentication.
      
      	// 在调用指针使用地址身份验证时将其重新制定
        result->invoke = aBlock->invoke;
#endif
        // reset refcount
      
      	// 重置引用计数
        result->flags &= ~(BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING);    // XXX not needed
      
      	// 不需要XXX
        result->flags |= BLOCK_NEEDS_FREE | 2;  // logical refcount 1
      	
      	// 逻辑引用计数1
        _Block_call_copy_helper(result, aBlock);
        // Set isa last so memory analysis tools see a fully-initialized object.
      
      	// 设置isa指向，以便于分析内存工具能够看到完全初始化的对象
        result->isa = _NSConcreteMallocBlock;
        return result;
    }
}
```

- 在详细分析copy实现过程前，我们先对其用到的子函数进行一些分析：

### `latching_incr_int()`

```c++
static int32_t latching_incr_int(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return BLOCK_REFCOUNT_MASK;
        }
        if (OSAtomicCompareAndSwapInt(old_value, old_value+2, where)) {
            return old_value+2;
        }
    }
}
```

- latch:占有；incr:增量寄存器
- 这一块就是对block的应用计数+1的操作，我们可以看到显然block的引用计数是通过flags去管理的
- 进入一个死循环，如果flags含有`BLOCK_REFCOUNT_MASK`证明其引用计数达到最大，直接返回，需要三万多个指针指向，正常情况下不会出现;其余情况下，使用`OSAtomicCompareAndSwapInt`函数【在`where`取值与`old_value`相同时，将`old_value + 2`赋给`where`】
- 这里值得注意的是，引用计数每次是 + 2的，block的引用计数是以flags的后16位代表的，以2为单位，每次递增2，1被`BLOCK_DEALLOCATING`占用

### 详解`Block_copy`

- 先对于传入的`Block_layout`结构体进行判断，如果为NULL就直接返回，接下来进入if判断【就是注释里说的可以使用switch】
  1. 对于堆上的block【`BLOCK_NEEDS_FREE`，话说这个宏真是简单不做作，堆上的数据就是需要释放的】，进入`latching_incr_int`函数对其进行引用计数增加操作【flags】
  2. 对于全局变量，不做任何操作，直接返回【也就是说，copy函数会一直调用喽】
  3. 剩下的情况就是在栈上的block
- 对于栈上的block进行拷贝：

  1. 先分配一块和原block一样大小的内存空间【失败直接返回】
  2. 使用memmove()函数进行位复制，将原block的每个字节都拷贝过去【memmove用于拷贝字节，如果目标区域和源区域有重叠的话，memmove能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中，但复制后源内容会被更改。但是当目标区域与源区域没有重叠则和memcpy函数功能相同】
  3. 将新Block的引用计数置零。`BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING`就是0xffff，~(0xffff)就是0x0000【所以咱就不能直接写一个0000宏么】，result->flags与0x0000与等就将result->flags的后16位置零。然后将新Block标识为堆Block并将其引用计数置为2【此处存疑，在栈上的block之前的引用计数时用来干嘛的呢？】
  4. 接着调用copy助手，如果有copy函数的话直接调用，没有的话直接返回
  5. 设置isa指针，指向`_NSConcreteMallocBlock`

- 下面我们看一个例子：

```objective-c
NSObject *obj = [[NSObject alloc] init];
void(^aBlock)() = ^{
  
  obj;
};
NSLog(@"%ld", CFGetRetainCount((__bridge void *)obj));
```


- 这样子的打印结果会是3
- 由于obj时截获的外部自动变量，所以我们会用结构体指针指向它来进行保存，所以引用计数会加
- 在讲块赋值给aBlock的时候，由于块从栈上复制到了堆上，在堆上的block也会指向obj，导致其引用计数再加一

## Block_release

- 在`Block.h`中，Block_release是如此定义的：

```c++
// Lose the reference, and if heap based and last reference, recover the memory

// 失去引用，如果是这是基于堆的并且这是最后一个引用，恢复内存
BLOCK_EXPORT void _Block_release(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
```

- 它的实现部分在runtime.cpp里面：

```c++
// API entry point to release a copied Block

// 释放复制块的API入口点
void _Block_release(const void *arg) {
    struct Block_layout *aBlock = (struct Block_layout *)arg;
    if (!aBlock) return;
    if (aBlock->flags & BLOCK_IS_GLOBAL) return;
    if (! (aBlock->flags & BLOCK_NEEDS_FREE)) return;

    if (latching_decr_int_should_deallocate(&aBlock->flags)) {
        _Block_call_dispose_helper(aBlock);
        _Block_destructInstance(aBlock);
        free(aBlock);
    }
}
```

### `latching_decr_int_should_deallocate`

```c++
// return should_deallocate?
static bool latching_decr_int_should_deallocate(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return false; // latched high
        }
        if ((old_value & BLOCK_REFCOUNT_MASK) == 0) {
            return false;   // underflow, latch low
        }
        int32_t new_value = old_value - 2;
        bool result = false;
        if ((old_value & (BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING)) == 2) {
            new_value = old_value - 1;
            result = true;
        }
        if (OSAtomicCompareAndSwapInt(old_value, new_value, where)) {
            return result;
        }
    }
}
```

- 对于引用计数过高(0xfffe)，或者过低(0) ，返回false
- 如果引用计数为2，减1，说明正在释放，返回true
- 如果大于2，则将其引用计数-2返回false

### 详解`Block_release`

- 对于全局Block，非堆Block不进行处理
- 判断引用计数需不需要释放
- 对于判断出来为true的，也就是引用计数2的，调用助手函数中的dispose函数
- free

# `_Block_object_assign`以及`_Block_object_dispose`

- 在block中的助手函数里，两个助手函数copy和dispose，他们实际上时调用的`_Block_object_assign`以及`_Block_object_dispose`

```c++
__attribute__((__blocks__(byref))) __Block_byref_obj_0 obj = {(void*)0,(__Block_byref_obj_0 *)&obj, 33554432, sizeof(__Block_byref_obj_0), __Block_byref_id_object_copy_131, __Block_byref_id_object_dispose_131, ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"))};
```

- 千山万水就差一点，冲冲冲！
- 我们先来了解下其中会用到的枚举含义：

```c++
// Runtime support functions used by compiler when generating copy/dispose helpers

// Values for _Block_object_assign() and _Block_object_dispose() parameters
enum {
    // see function implementation for a more complete description of these fields and combinations
    BLOCK_FIELD_IS_OBJECT   =  3,  // id, NSObject, __attribute__((NSObject)), block, ...	OC对象类型
    BLOCK_FIELD_IS_BLOCK    =  7,  // a block variable	另一个block
    BLOCK_FIELD_IS_BYREF    =  8,  // the on stack structure holding the __block variable	为一个被__block修饰后生成的结构体
    BLOCK_FIELD_IS_WEAK     = 16,  // declared __weak, only used in byref copy helpers	被__weak修饰过的弱引用，只在Block_byref管理内部对象内存时使用，也就是__block __weak id 
    BLOCK_BYREF_CALLER      = 128, // called from __block (byref) copy/dispose support routines.	在处理Block_byref内部对象内存的时候会加的一个额外标记，配合上面的枚举一起使用；
};

enum {
    BLOCK_ALL_COPY_DISPOSE_FLAGS = 
        BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_BLOCK | BLOCK_FIELD_IS_BYREF |
        BLOCK_FIELD_IS_WEAK | BLOCK_BYREF_CALLER
};

```

## `_Block_object_assign`

```c++
/
// When Blocks or Block_byrefs hold objects then their copy routine helpers use this entry point
// to do the assignment.
//
  
// blocks或block_byrefs保存对象时，它们的复制例程助手使用此入口点进行赋值。
void _Block_object_assign(void *destArg, const void *object, const int flags) {
    const void **dest = (const void **)destArg;
  	///  destArg为执行Block_copy()后的block中的对象、block、或者BYREF指针的指针，object为copy之前的变量指针。
    switch (os_assumes(flags & BLOCK_ALL_COPY_DISPOSE_FLAGS)) {
      case BLOCK_FIELD_IS_OBJECT:
        // 
        /*******
        id object = ...;
        [^{ object; } copy];
        ********/
				///	简单的指针赋值
        _Block_retain_object(object);
        *dest = object;
        break;

      case BLOCK_FIELD_IS_BLOCK:
        /*******
        void (^object)(void) = ...;
        [^{ object; } copy];
        ********/
				
        /// 当block捕获的变量为另外一个block时执行此步，copy一个新的block并赋值
        *dest = _Block_copy(object);
        break;
    
      case BLOCK_FIELD_IS_BYREF | BLOCK_FIELD_IS_WEAK:
      /// 当block捕获的变量为__block修饰的变量时会执行此步，执行byref_copy操作
      case BLOCK_FIELD_IS_BYREF:
      /// 当block捕获的变量为__block修饰的变量时会执行此步，执行byref_copy操作
        /*******
         // copy the onstack __block container to the heap
         // Note this __weak is old GC-weak/MRC-unretained.
         // ARC-style __weak is handled by the copy helper directly.
         __block ... x;
         __weak __block ... x;
         [^{ x; } copy];
         ********/

        *dest = _Block_byref_copy(object);
        break;
        
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK:
        /*******
         // copy the actual field held in the __block container
         // Note this is MRC unretained __block only. 
         // ARC retained __block is handled by the copy helper directly.
         __block id object;
         __block void (^object)(void);
         [^{ object; } copy];
         ********/

        *dest = object;
        break;

      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_WEAK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK  | BLOCK_FIELD_IS_WEAK:
        /*******
         // copy the actual field held in the __block container
         // Note this __weak is old GC-weak/MRC-unretained.
         // ARC-style __weak is handled by the copy helper directly.
         __weak __block id object;
         __weak __block void (^object)(void);
         [^{ object; } copy];
         ********/

        *dest = object;
        break;

      default:
        break;
    }
}
```

## `_Block_byref_copy`

```C++
// A closure has been copied and its fixup routine is asking us to fix up the reference to the shared byref data
/// 已经复制了一个闭包，它的修复例程要求我们修复对共享byref数据的引用
// Closures that aren't copied must still work, so everyone always accesses variables after dereferencing the forwarding ptr.
/// 未被复制的闭包必须仍然有效，因此每个人在取消对转发ptr的引用后总是访问变量。
// We ask if the byref pointer that we know about has already been copied to the heap, and if so, increment and return it.
/// 我们询问我们知道的byref指针是否已经复制到堆中，如果是，则递增并返回它。
// Otherwise we need to copy it and update the stack forwarding pointer
/// 否则我们需要复制它并更新堆栈转发指针
/// 机翻看的想吐🤮
static struct Block_byref *_Block_byref_copy(const void *arg) {
    struct Block_byref *src = (struct Block_byref *)arg;

    if ((src->forwarding->flags & BLOCK_REFCOUNT_MASK) == 0) {
        // src points to stack
      	/// 对于栈上的byref，先分配内存，isa设置为NULL
        struct Block_byref *copy = (struct Block_byref *)malloc(src->size);
        copy->isa = NULL;
        // byref value 4 is logical refcount of 2: one for caller, one for stack
      	/// 新的byref的引用计数要设置为4，标记为堆，一份为调用方，一份为栈所有，所以为4【还是2为单位】
        copy->flags = src->flags | BLOCK_BYREF_NEEDS_FREE | 4;
        copy->forwarding = copy; // patch heap copy to point to itself
        src->forwarding = copy;  // patch stack to point to heap copy
      	/// 将当前byref以及malloc的byref的forwarding都指向堆byref，操作堆栈都是同一份东西
        copy->size = src->size;

        if (src->flags & BLOCK_BYREF_HAS_COPY_DISPOSE) {
            // Trust copy helper to copy everything of interest
            // If more than one field shows up in a byref block this is wrong XXX
            struct Block_byref_2 *src2 = (struct Block_byref_2 *)(src+1);
            struct Block_byref_2 *copy2 = (struct Block_byref_2 *)(copy+1);
            copy2->byref_keep = src2->byref_keep;
          	// 执行byref的byref_keep函数(即assign函数，只是会加上BLOCK_BYREF_CALLER标志)，管理捕获的对象内存
            copy2->byref_destroy = src2->byref_destroy;

            if (src->flags & BLOCK_BYREF_LAYOUT_EXTENDED) {
                struct Block_byref_3 *src3 = (struct Block_byref_3 *)(src2+1);
                struct Block_byref_3 *copy3 = (struct Block_byref_3*)(copy2+1);
                copy3->layout = src3->layout;
            }

            (*src2->byref_keep)(copy, src);
        }
        else {
            // Bitwise copy.
            // This copy includes Block_byref_3, if any.
          
          	/// 如果捕获的是普通变量，就没有Block_byref_2，copy+1和src+1指向的就是Block_byref_3，执行字节拷贝。
            memmove(copy+1, src+1, src->size - sizeof(*src));
        }
    }
    // already copied to heap
  	/// 如果该byref是存在于堆，则只需要增加其引用计数。
    else if ((src->forwarding->flags & BLOCK_BYREF_NEEDS_FREE) == BLOCK_BYREF_NEEDS_FREE) {
        latching_incr_int(&src->forwarding->flags);
    }
    
    return src->forwarding;
}

```



## `_Block_object_dispose`

```c++
// When Blocks or Block_byrefs hold objects their destroy helper routines call this entry point
// to help dispose of the contents
void _Block_object_dispose(const void *object, const int flags) {
    switch (os_assumes(flags & BLOCK_ALL_COPY_DISPOSE_FLAGS)) {
      case BLOCK_FIELD_IS_BYREF | BLOCK_FIELD_IS_WEAK:
      case BLOCK_FIELD_IS_BYREF:
        // get rid of the __block data structure held in a Block
        _Block_byref_release(object);
        break;
      case BLOCK_FIELD_IS_BLOCK:
        _Block_release(object);
        break;
      case BLOCK_FIELD_IS_OBJECT:
        _Block_release_object(object);
        break;
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_WEAK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK  | BLOCK_FIELD_IS_WEAK:
        break;
      default:
        break;
    }
}
// 这部分有了上面的基础应该很好懂了
```

## `_Block_byref_release`

```C++
static void _Block_byref_release(const void *arg) {
    struct Block_byref *byref = (struct Block_byref *)arg;

    // dereference the forwarding pointer since the compiler isn't doing this anymore (ever?)
    byref = byref->forwarding;
    
    if (byref->flags & BLOCK_BYREF_NEEDS_FREE) {
        int32_t refcount = byref->flags & BLOCK_REFCOUNT_MASK;
        os_assert(refcount);
        if (latching_decr_int_should_deallocate(&byref->flags)) {
            if (byref->flags & BLOCK_BYREF_HAS_COPY_DISPOSE) {
                struct Block_byref_2 *byref2 = (struct Block_byref_2 *)(byref+1);
                (*byref2->byref_destroy)(byref);
            }
            free(byref);
        }
    }
}
```

- 首先赋值给forwaring
- 如果有助手的话执行destrory
- free

