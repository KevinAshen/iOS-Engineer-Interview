[TOC]
# 关于我的仓库

- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本篇是解读RunTime源码中的第一篇文章，着重讲述RunTime中的基本结构体，了解类，对象中都有些什么东西

# 准备工作

- 请准备好750.1版本的objc4源码一份【目前最新的版本】，打开它，找到文章中提到的方法，类型，对象
- 一切请以手中源码为准，不要轻信任何人，任何文章，包括本篇博客
- 源码建议从Apple官方开源网站获取[obj4](https://opensource.apple.com/tarballs/objc4/objc4-750.1.tar.gz)
- 官网上下载下来需要自己配置才能编译运行，如果不想配置，可以在[RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)中clone

# 类与对象
## 对象
### objc_object
- 对象是一个objc_object类型的结构体
#### 补充知识：OC类的本质
- OC语言在编译期都会被编译为C语言的Runtime代码，二进制执行过程中执行的都是C语言代码。而OC的类本质上都是结构体，在编译时都会以结构体的形式被编译到二进制中。

### 找到它？

- 我们先在源码中找到objc_object在哪，于是你打开全局搜索，找到了这么一段

```objective-c
#if !OBJC_TYPES_DEFINED
/// An opaque type that represents an Objective-C class.
typedef struct objc_class *Class;

/// Represents an instance of a class.
struct objc_object {
    Class _Nonnull isa  OBJC_ISA_AVAILABILITY;
};

/// A pointer to an instance of a class.
typedef struct objc_object *id;
#endif
```

- 于是你感觉里面就一个Class _Nonnull isa  OBJC_ISA_AVAILABILITY;
- 然而，请注意上面的#if !OBJC_TYPES_DEFINED，点进去会发现该宏是1，也就是说。。。根本不走这个！！！
- 真正的定义是在objc-private文件里【请着重关注这个文件，是个宝藏文件】
```objective-c
struct objc_object {
    // isa结构体
private:
    isa_t isa;     //这里讲到了isa的类型是isa_t

public:
  //省略方法
};
```
- 先不看方法，发现这里面只有一个isa_t类型的isa；
### isa_t
- 点到isa_t里去【这次是唯一的，不会走错路了】，其定义为
```objective-c
union isa_t {
    isa_t() { }
    isa_t(uintptr_t value) : bits(value) { }

    Class cls;
    uintptr_t bits;
#if defined(ISA_BITFIELD)
    struct {
        ISA_BITFIELD;  // defined in isa.h
    };
#endif
};
```
- 关于isa，会在下面仔细讲解
- 这里只要知道isa里存储了对象所属类的信息【Class cls】
- 我们把目光投向Class cls这里
## 类
### objc_class

- 点到Class的定义里，此时你应该学乖了，知道要找objc-private里的
- 发现Class的定义为typedef struct objc_class *Class;
- 再查看下objc_class的定义
```objective-c
struct objc_class : objc_object {
    Class superclass;
    cache_t cache;            
    class_data_bits_t bits;  
    //省略方法
};
```
- 这个时候你应该会发现一个特别特别特别神奇的事，objc_class竟然继承于objc_object，换句话说，类的本质也是个对象
- objc_class中的的内容【概念图】

![objc-isa-class-pointer](http://ww2.sinaimg.cn/large/006tNc79ly1g51xry2tllj30iv09laal.jpg)

- objc_class里一共三个东西，第一个Class superclass显然是我们能理解的，是该类的父类
- 好，此时我们思考一个问题，类也是一个对象，类也应该有一个isa，可对象的isa存储的是所属类的信息，类的isa存储的会是什么呢？
#### 说明：isa指针
- 学习过程中，会发现很多人将isa称之为isa指针
- 这个因为isa虽然是一个结构体，但其中会存储所属类的地址【类也是有地址的，不在堆也不在栈，类似于单例】
- 因此很多文章会说isa指针指向xxx，但请理解，isa不是一个指针，后面会讲它是如何获取地址的
- 为方便讲述，下面也开始使用isa指向xxx这种说法
#### 引入概念：元类与根类

- 元类【meta class】：类的所属类，类的isa会指向其元类
- 根类【root class】：根类，一般情况下就是指NSObject
### isa闭环
- 首先来看一张闭环图

![isa闭环指针图](http://ww3.sinaimg.cn/large/006tNc79ly1g51w8048tuj30u00vegnt.jpg)

- 图里把继承关系和isa指向关系讲的很清楚，这里请注意两点
    - Root class根类，它是继承关系的顶点，它不继承与任何类
    - Root meta class根元类，它是isa指向的顶点，其isa直接指向自己

#### 补充知识：类方法与实例方法的存储

- 假设我们每生成一个对象就要开辟空间存储他的所有方法，显然是对空间的浪费
- 因此使用对象的实例方法都是存放在所属类当中
- 但是类方法怎么办呢，因此有了元类，将类方法存储在元类当中，让两者的存储统一起来，使得无论是类还是对象都能**通过相同的机制查找方法的实现**。
- 类被定义为`objc_class`结构体，`objc_class`结构体继承自`objc_object`，所以类也是对象。在应用程序中，类对象只会被创建一份。在`objc_class`结构体中定义了对象的`method list`、`protocol`、`ivar list`等，表示对象的行为。既然类是对象，那类对象也是其他类的实例。所以`Runtime`中设计出了`meta class`，通过`meta class`来创建类对象，所以类对象的`isa`指向对应的`meta class`。而`meta class`也是一个对象，所有元类的`isa`都指向其根元类，根原类的`isa`指针指向自己。通过这种设计，`isa`的整体结构形成了一个闭环。

# isa详解

- 下面来详细讲解isa即isa_t结构体

## 完整定义

```objective-c
union isa_t {
    isa_t() { }
    isa_t(uintptr_t value) : bits(value) { }

    Class cls;
    uintptr_t bits;
#if defined(ISA_BITFIELD)
    struct {
        ISA_BITFIELD;  // defined in isa.h
    };
#endif
};
```

## uintptr_t bits【unsigned long bits】

-  bits中存储了所属类地址等信息

### union isa_t

- 请注意，虽然前面将isa_t称之为结构体，但注意定义里将其定位的是union【共用体】
- 也就是说，里面的isa_t、cls、 bits 还有结构体**共用同一块地址空间**。而 isa 总共会占据 64 位的内存空间（决定于其中的结构体）
- 请牢记这一点，不然后面会有困惑

### 结构分析

- 在isa_t内部定义了一个结构体，里面只有一个宏ISA_BITFIELD
- 查看里面的内容
```objective-c
struct {
        ISA_BITFIELD;  // defined in isa.h
    };
    

//ISA_BITFIELD内容以及注解
# elif __x86_64__
#   define ISA_MASK        0x00007ffffffffff8ULL
#   define ISA_MAGIC_MASK  0x001f800000000001ULL
#   define ISA_MAGIC_VALUE 0x001d800000000001ULL
#   define ISA_BITFIELD                                   
      uintptr_t nonpointer        : 1;  //说明是不是指针【这是一个对象还是一个类，下文会继续介绍】
      uintptr_t has_assoc         : 1;  // 是否曾经或正在被关联引用，如果没有，可以快速释放内存             
      uintptr_t has_cxx_dtor      : 1;   // 这个字段存储类是否有c++析构器
      uintptr_t shiftcls          : 44;    //存储cls类地址 /*MACH_VM_MAX_ADDRESS 0x7fffffe00000*/ 
      uintptr_t magic             : 6;           //校验          
      uintptr_t weakly_referenced : 1;          // 对象是否曾经或正在被弱引用，如果没有，可以快速释放内存           
      uintptr_t deallocating      : 1;         // 对象是否正在释放内存            
      uintptr_t has_sidetable_rc  : 1;             // 是否需要散列表存储引用计数。当extra_rc存储不下时，该值为1        
      uintptr_t extra_rc          : 8		// 引用计数数量，实际的引用计数减一
#   define RC_ONE   (1ULL<<56)
#   define RC_HALF  (1ULL<<7)
```

#### 补充知识：arm 64与x86_64
- 这里都是处理器型号
- iOS5之后，CPU数据吞吐量为64bit(64个二进制位，表示8个字节)，相较于32位处理器效率提升了一倍，此时对应寄存器也变成了64位，可以处理更大的数据显示更多的状态。
- i386是针对intel通用微处理器32位处理器，x86_64是针对x86架构的64位处理器
- 模拟器32位处理器测试需要i386架构，模拟器64位处理器测试需要x86_64架构，真机32位处理器需要armv7,或者armv7s架构，真机64位处理器需要arm64架构。
- 这里就以x86_64为例

#### 各个部分存储空间
- 就是ISA_BITFIELD这里的内容存储了该类的信息

![123俄](http://ww1.sinaimg.cn/large/006tNc79ly1g51yrub07bj30lw0fn0u5.jpg)

- 下面我们看一下具体的存储地址

![5566](http://ww1.sinaimg.cn/large/006tNc79ly1g51ytniiwsj31900u0419.jpg)

## isa_t的初始化

### initIsa

```objective-c
inline void 
objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor) 
{
    assert(!isTaggedPointer()); 
    if (!nonpointer) {
        isa.cls = cls;
    } else {
        assert(!DisableNonpointerIsa);
        assert(!cls->instancesRequireRawIsa());
        isa_t newisa(0);

#if SUPPORT_INDEXED_ISA
        assert(cls->classArrayIndex() > 0);
        newisa.bits = ISA_INDEX_MAGIC_VALUE;
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.indexcls = (uintptr_t)cls->classArrayIndex();
#else
        newisa.bits = ISA_MAGIC_VALUE;  //0x001d800000000001ULL
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.shiftcls = (uintptr_t)cls >> 3;
#endif
        isa = newisa;
    }
}
```

- 看到这么大的一段代码先不要慌，先从三个参数开始

#### 补充知识：assert()函数
- 其作用是如果它的条件返回错误，则终止程序执行。
- assert的作用是先计算表达式 expression ，如果其值为假（即为0），那么它先向stderr打印一条出错信息，然后通过调用 abort 来终止程序运行。

#### 三个参数【Class cls】【bool nonpointer】【bool hasCxxDtor】

- cls就是其存储的类
- nonpointer这个我们看initInstanceIsa

```objective-c
inline void 
objc_object::initInstanceIsa(Class cls, bool hasCxxDtor)
{
    assert(!cls->instancesRequireRawIsa());
    assert(hasCxxDtor == cls->hasCxxDtor());

    initIsa(cls, true, hasCxxDtor);
}
```


- 对于实例对象，传入的nonpointer是true【翻译一下就是确实不是一个指针，有其他内容】
- 我们在返回上面的initIsa

```objective-c
inline void 
objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor) 
{
    assert(!isTaggedPointer()); 
    if (!nonpointer) {
        isa.cls = cls;
    } else {
				//省略代码
    }
}
```

- 也就是说如果传入的是false【不是不是指针，是指针】也就是说是在直接访问类，那么就直接返回cls内容就行
- 回忆下上面说的isa_t是一个共用体，给cls赋值，也就意味着在结构体里面填完了内容

#### 补充知识：ULL 
- ULL:unsiged long long   64bit
- 在16进制数据后面加上数据类型限制，因为默认的数据类型是int，加数据类型是为了防止数据越界。32和64位下long long int总是64位的， long int总是32位的。
#### 真正初始化
- 真正初始化的代码就只有几行
```objective-c
isa_t newisa(0);
newisa.bits = ISA_MAGIC_VALUE;
// isa.magic is part of ISA_MAGIC_VALUE
// isa.nonpointer is part of ISA_MAGIC_VALUE
newisa.has_cxx_dtor = hasCxxDtor;
newisa.shiftcls = (uintptr_t)cls >> 3;
isa = newisa;
```

- 这个newisa就是初始化出来的结果
- 我们要好好理解的是newisa.bits = ISA_MAGIC_VALUE;
- 首先ISA_MAGIC_VALUE的值为0x001d800000000001ULL，将它换成二进制数11101100000000000000000000000000000000000000000000001【记住前面的高低位】，输入到bits中【再强调一遍isa_t是一个共用体，输入到bits里就等于到了结构体里】
- 结果如下图【将indexed改成nonpointer】

![000](http://ww2.sinaimg.cn/large/006tNc79ly1g5204mk2ttj31cc0rkwgq.jpg)

- 因此初始化的时候只设置了nonpointer，magic两个部分
- `magic` 的值为 `0x3b` 用于调试器判断当前对象是真的对象还是没有初始化的空间
- 在设置 `indexed` 和 `magic` 值之后，会设置 `isa` 的 `has_cxx_dtor`，这一位表示当前对象有 C++ 或者 ObjC 的析构器(destructor)，如果没有析构器就会快速释放内存。

![小心心](http://ww2.sinaimg.cn/large/006tNc79ly1g520al4ugaj31cc0rkq56.jpg)

- newisa.shiftcls = (uintptr_t)cls >> 3这句是为了**将 Class 指针中无用的后三位清除减小内存的消耗，因为类的指针要按照字节（8 bits）对齐内存，其指针后三位都是没有意义的 0**。这里首先一个OC的指针长度是47，而这里的shiftcls占44个字节，本来
## isa诸多用处
### 获取cls地址：ISA() 方法
- 由于现在isa不在只存放地址了，还多了很多附加内容，因此需要一个专门的方法获取shiftcls中的内容
```objective-c
#define ISA_MASK 0x00007ffffffffff8ULL
inline Class 
objc_object::ISA() 
{
    return (Class)(isa.bits & ISA_MASK);
}
```
- 进行&操作获取地址
### class方法
- 进入源码以后，可以查看很多内容的源码
```objective-c
+ (Class)class {
    return self;
}

- (Class)class {
    return object_getClass(self);
}

Class object_getClass(id obj)
{
    if (obj) return obj->getIsa();
    else return Nil;
}
```
- class既是类方法又是实例方法，类方法直接返回自身，实例方法返回的就是isa中的内容
### isMemberOfClass&&isKindOfClass
```objective-c
+ (BOOL)isMemberOfClass:(Class)cls {
    return object_getClass((id)self) == cls;
}

- (BOOL)isMemberOfClass:(Class)cls {
    return [self class] == cls;
}

+ (BOOL)isKindOfClass:(Class)cls {
    for (Class tcls = object_getClass((id)self); tcls; tcls = tcls->superclass) {
        if (tcls == cls) return YES;
    }
    return NO;
}

- (BOOL)isKindOfClass:(Class)cls {
    for (Class tcls = [self class]; tcls; tcls = tcls->superclass) {
        if (tcls == cls) return YES;
    }
    return NO;
}
```
- 这也没啥好解释的了，结合class的内容应该很好理解了
# objc_class扩充：bits
- 回顾下objc_class的内容
```objective-c
struct objc_class : objc_object {
    Class superclass;
    cache_t cache;            
    class_data_bits_t bits;  
    //省略方法
};
```
-  其中bits是绝对的主角，里面存储了类的方法列表等等的信息
- 看下class_data_bits_t里的内容，发现就一个uintptr_t bits
- 看到这很容易联想到和前面isa里的bits，事实上，两者确实很想，这也是我把它放在后面来讲的原因之一
## class_rw_t与class_ro_t
- bits中最重要的部分就是class_rw_t，class_ro_t两兄弟
- 在哪里找到他们呢？返回objc_class的定义，发现其中有一个data函数
```objective-c
 class_rw_t *data() {
        return bits.data();
    }
    
//data（）
 class_rw_t* data() {
        return (class_rw_t *)(bits & FAST_DATA_MASK);
    }

// class_rw_t定义
struct class_rw_t {
    // Be warned that Symbolication knows the layout of this structure.
    uint32_t flags;
    uint32_t version;

    const class_ro_t *ro;
    
    // 方法信息
    method_array_t methods;
    // 属性信息
    property_array_t properties;
    // 协议信息
    protocol_array_t protocols;

    Class firstSubclass;
    Class nextSiblingClass;

    char *demangledName;
  //省略方法
};
```
![让我](http://ww1.sinaimg.cn/large/006tNc79ly1g52oi9ps0tj30lu060mxz.jpg)

- 就如同注释里说明的，其中有方法，属性，协议等等信息【后面在信息通讯等内容里我们会在仔细研究这一块】
- 但这里有一个我们不太懂的const class_ro_t是个啥？
```objective-c
struct class_ro_t {
    uint32_t flags;
    uint32_t instanceStart;
    uint32_t instanceSize;

    const uint8_t * ivarLayout;
    
    const char * name;
    // 方法列表
    method_list_t * baseMethodList;
    // 协议列表
    protocol_list_t * baseProtocols;
    // 变量列表
    const ivar_list_t * ivars;

    const uint8_t * weakIvarLayout;
    // 属性列表
    property_list_t *baseProperties;

    method_list_t *baseMethods() const {
        return baseMethodList;
    }
};
```
![肉](http://ww4.sinaimg.cn/large/006tNc79ly1g52oifno6kj30ji04bq3u.jpg)

- 可以看到两者内容基本一样，就是class_ro_t里的协议，方法等前面多了个base
- 这里先注意中class_rw_t定义的class_ro_t是带const修饰符的，是不可变的

-  下面我们通过class的初始化来研究下这两个都是这么运作的

### realizeClass方法

```objective-c
//精取代码
static Class realizeClass(Class cls)
{
    const class_ro_t *ro;
    class_rw_t *rw;
    Class supercls;
    Class metacls;
    bool isMeta;
    ro = (const class_ro_t *)cls->data(); //编译期间，cls->data指向的是class_ro_t结构体。cls->data()存放的是即ro中存储的是编译时就已经决定的原数据
    rw = (class_rw_t *)calloc(sizeof(class_rw_t), 1);	//初始化rw
    rw->ro = ro;	//rw中的ro赋值
    rw->flags = RW_REALIZED|RW_REALIZING;
    cls->setData(rw);
    rw->version = isMeta ? 7 : 0; 
    cls->chooseClassArrayIndex();
    methodizeClass(cls);	//运行期间，将ro中的内容赋值给rw
    return cls;
}
```
#### 补充知识：addRootClass和addSubclass
```objective-c
static void addSubclass(Class supercls, Class subcls)
{
    runtimeLock.assertLocked();
    if (supercls  &&  subcls) {
        assert(supercls->isRealized());
        assert(subcls->isRealized());
        subcls->data()->nextSiblingClass = supercls->data()->firstSubclass;
        supercls->data()->firstSubclass = subcls;

        if (supercls->hasCxxCtor()) {
            subcls->setHasCxxCtor();
        }

        if (supercls->hasCxxDtor()) {
            subcls->setHasCxxDtor();
        }

        if (supercls->hasCustomRR()) {
            subcls->setHasCustomRR(true);
        }

        if (supercls->hasCustomAWZ()) {
            subcls->setHasCustomAWZ(true);
        }

        // Special case: instancesRequireRawIsa does not propagate 
        // from root class to root metaclass
        if (supercls->instancesRequireRawIsa()  &&  supercls->superclass) {
            subcls->setInstancesRequireRawIsa(true);
        }
    }
}

static void addRootClass(Class cls)
{
    runtimeLock.assertLocked();

    assert(cls->isRealized());
    cls->data()->nextSiblingClass = _firstRealizedClass;
    _firstRealizedClass = cls;
}
```
- 这两个方法的职责是将某个类的子类串成一个列表，大致是superClass.firstSubclass -> subClass1.nextSiblingClass -> subClass2.nextSiblingClass -> ...
- 而存储这些的同样是data，也就是说、以通过class_rw_t，获取到当前类的所有子类

### methodizeClass方法

```objective-c
//精取代码
// 设置类的方法列表、协议列表、属性列表，包括category的方法
static void methodizeClass(Class cls)
{
    runtimeLock.assertLocked();

    bool isMeta = cls->isMetaClass();
    auto rw = cls->data();
    auto ro = rw->ro;
    method_list_t *list = ro->baseMethods();
    if (list) {
        prepareMethodLists(cls, &list, 1, YES, isBundleClass(cls));
        //添加方法
        rw->methods.attachLists(&list, 1);
    }
    property_list_t *proplist = ro->baseProperties;
    if (proplist) {
    //添加属性
        rw->properties.attachLists(&proplist, 1);
    }

    protocol_list_t *protolist = ro->baseProtocols;
    if (protolist) {
    //添加协议
        rw->protocols.attachLists(&protolist, 1);
    }
    //添加协议【略】
}
```
- 也就是说是在methodizeClass中，将rw的内容填充完

### 流程总结

- 编写代码运行后,开始类的方法,成员变量 属性 协议等信息都存放在 const class_ro_t中,运行过程中,会将信息整合,动态创建 class_rw_t,然后会将 class_ro_t中的内容(类的原始信息:方法 属性 成员变量 协议信息) 和 分类的方法 属性 协议 成员变量的信息 存储到 class_rw_t中.并通过数组进行排序,分类方法放在数组的前端,原类信息放在数组的后端.运行初始 objc-class中的 bits是指向 class_ro_t的,bits中的data取值是从class_ro_t中获得,而后创建 class_rw_t,class_rw_t中的 class_ro_t从初始的 class_ro_t中取值,class_rw_t初始化完成后,修改 objc_class中的bits指针指向class_rw_t
- 所以ro中存储的是编译时就已经决定的原数据，rw才是运行时动态修改的数据。

![rw与ro](http://ww3.sinaimg.cn/large/006tNc79ly1g52oh9pxxej312t0sv41c.jpg)

## (class_rw_t *)(bits & FAST_DATA_MASK)

- 这句和isa里的非常像，想必应该看了很有感觉了
- FAST_DATA_MASK0x00007ffffffffff8
- 这句取出bits中47-3的位置，就是class_rw_t

# 2019.7.26更新

## 对于class_rw_t 以及class_ro_t进一步理解

- 这里其实从名字上就可以理解这两个结构体，rw是指readwrite【可读写】，ro是指readonly【只读】，因此ro是在编译阶段决定的，无法修改，rw是在运行时可以进行添加的
- 这里可以先补充下方法的添加流程，我们知道类的扩充可以用extension，而extension中的东西也是添加在ro中，在编译阶段决定的；而category是在运行期决定的，添加在rw中
- 这也解释上面途中，为什么ro中的methods只要一个一维数组就行【因为里面只要保存本类的方法，协议，属性同理】，rw中就得是个二维数组，里面存放着本类以及category中添加的内容，具体是怎么操作的到后面分析category中再说

## 关于cache_t

- cache_t中存储着方法的缓存，通过一个散列表来存储，方便查找【这里不要怕这个散列，后面很多内容都是散列的】

![cache](http://ww2.sinaimg.cn/large/006tNc79ly1g5d1ug621sj30jf05qdgx.jpg)

- 这里要注意，我们把sel【函数名】来进行hash，查找
- 同时存放时又把sel，与imp都存放了

## 方法调用流程

```objective-c
CHPerson *person = [[CHPerson alloc] init];
[person test]
```

1. 首先通过isa找到person对应的CHPerson类，先查看其中的cache_t里的缓存方法
2. 如果找不到该test方法，就去查看bits中的rw的methods查找方法【如果找到了，调用，且添加到cache_t中】
3. 如果在CHPerson里找不到，会查看CHPerson的父类，同样是先cache_t，后bits，如果查找到了，是存放在自己的cache里，不是父类的【注意！】
4. 最后一直查看父类会到达NSObject根类，如果依然找不到就抛出异常

# 2019.8.3更新【ivar】
- 想不到这篇文章越写越长，也不想重新开博客了，就越写越多
- 这部分主要涉及了property和ivar以及Non Fragile ivars
## property和ivar
- 在刚学习OC的时候我们就了解过了属性与实例变量的区别
- property = ivar + get + set
- 下面我们探讨两者真正的关系
### ivar
- 在class_ro_t结构体中，有const ivar_list_t * ivars;
- 这里存储着所有ivar【实例变量】
```objective-c
//ivar_t
struct ivar_t {
    int32_t *offset;
    const char *name;
    const char *type;
    uint32_t alignment_raw;
    uint32_t size;
};
```

- 当我们编译的时候会决定ro结构体，也就是会将变量确定下来

### property

- 在ro，rw中都会有属性列表，查看property_t结构体

```objective-c
struct property_t {
    const char *name;
    const char *attributes;
};
```

- 这里我们可以看到存储一个属性的时候，我们只存储了其name和属性关键字，其余信息都不知道

### 两者关系

- 在编译时，对于我们的属性，其实会做两次添加，假设我们写了一个nameLabel属性，第一次是在属性列表里添加，内容为nameLabel，之后会在ivars中同步添加，加入名为_nameLabel的变量
- 因此在真正访问的时候，其实真正访问到的还是变量
- 与此同时，会把属性自动生成的set，get方法写入方法列表中
- 这就是为什么我们刚学OC的时候有这么一句话：property = ivar + get + set

## Non Fragile ivars

- Non Fragile ivars是Apple在OC2.0时推出的新特性
- 什么意思呢？我们现在有一个类A，在A里面的ivars中有两个ivar【a， b】，现在我们写了一个类B继承于A，在B中我们写了两个ivar【c，d】
- 我们假设A是顶点，不继承于任何类了
- B里面会只有两个ivar吗，只要有些许OC基础知识的人都会知道，不会，ab两个ivar也会跟着在列表里
- 下面是图解：

![img](http://ww4.sinaimg.cn/large/006tNc79ly1g5q7ci844dj30cd03aweq.jpg)

### 遇到问题了

- 看起来非常合理，但其实存在一个问题
- 我们知道内存的分配是在编译器决定的，假如现在我们在A里面又加了两个ivar【e，f】，在没有重新分配内存的情况下就会出现B中我们自己添加的cd两个ivar正好叠在了父类的ef两个新的ivar上，当我们根据内存地址去访问的时候就会出现错误

![img](http://ww3.sinaimg.cn/large/006tNc79ly1g5q7qrd7k6j30ca038q3b.jpg)

- 当然，这时候你肯定会说fnndp，修改完A后进行编译运行肯定会重新分配内存，那会出现这么愚蠢的情况？
- 可问题是，OC作为一门动态语言，对于静态库和苹果的官方库只是会进行加载
- 在这个问题没有解决的时代，只要静态库更新下，所有使用该静态库的app都得下架重新编译
- 于是我们的新特性**Non Fragile ivars**就闪亮登场了

### 动态偏移

-  `runtime`在加载`MyObject`类的时候(注:runtime加载类是在main函数跑起来之前),会计算基类的大小.`runtime`在运行期判断子类的`instanceStart`大小和父类`instanceSize`大小(**关于这两个成员请看文章开头展示的结构体内容**),如果子类的`instanceStart`小于父类的`instanceSize`,说明父类新增了成员变量,子类的成员变量需要进行偏移.
- 在上图的例子中,当`MyObject`的`instanceStart`小于`NSObject`的`instanceSize`,`MyObject`在编译器确定的结构体将会动态调整成员变量偏移,因此程序无需重新编译,就能在新版本的系统上运行.

![img](http://ww1.sinaimg.cn/large/006tNc79ly1g5q7wveyg8j30cc04paap.jpg)