[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 动态添加方法

# 准备工作

- 请准备好750.1版本的objc4源码一份【目前最新的版本】，打开它，找到文章中提到的方法，类型，对象
- 一切请以手中源码为准，不要轻信任何人，任何文章，包括本篇博客
- 文章中的源码都请过了我的删改，建议还是先看看源码
- 源码建议从Apple官方开源网站获取[obj4](https://opensource.apple.com/tarballs/objc4/objc4-750.1.tar.gz)
- 官网上下载下来需要自己配置才能编译运行，如果不想配置，可以在[RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)中clone

# class_addMethod

- 该方法可以替换掉已有方法，也可以新增船新方法

```objective-c
//class_addMethod
BOOL 
class_addMethod(Class cls, SEL name, IMP imp, const char *types)
{
    if (!cls) return NO;

    mutex_locker_t lock(runtimeLock);
    return ! addMethod(cls, name, imp, types ?: "", NO);
}

//addMethod
static IMP 
addMethod(Class cls, SEL name, IMP imp, const char *types, bool replace)
{
    IMP result = nil;
    method_t *m;
    if ((m = getMethodNoSuper_nolock(cls, name))) {
        //getMethodNoSuper_nolock从类的方法列表里查找，找到就判断传进来的replace
        // already exists
        if (!replace) {
            //replace为NO就直接返回找到的IMP
            result = m->imp;
        } else {
            //replace为YES就在方法列表里，将m的IMP设置成新的IMP
            //返回值还是原来方法的IMP
            result = _method_setImplementation(cls, m, imp);
        }
    } else {
        // fixme optimize
        //如果找不到，说明真正需要添加一个方法，就要新建method_list_t，给他的第一个方法添加成该方法
        //这种方式返回的IMP为nil
        //此时就是真正的动态添加方法
        method_list_t *newlist;
        newlist = (method_list_t *)calloc(sizeof(*newlist), 1);
        newlist->entsizeAndFlags = 
            (uint32_t)sizeof(method_t) | fixed_up_method_list;
        newlist->count = 1;
        newlist->first.name = name;
        newlist->first.types = strdupIfMutable(types);
        newlist->first.imp = imp;
        prepareMethodLists(cls, &newlist, 1, NO, NO);
        cls->data()->methods.attachLists(&newlist, 1);
        flushCaches(cls);
        result = nil;
    }
    return result;
}

```

## _method_setImplementation【替换原有方法】

```objective-c
static IMP 
_method_setImplementation(Class cls, method_t *m, IMP imp)
{
    IMP old = m->imp;
    m->imp = imp;
    flushCaches(cls);
    updateCustomRR_AWZ(cls, m);
    return old;
}

```

- 这里可以看到执行完该方法后返回的还是原有方法的IMP
- 但在过程中，确实将m对应的方法的IMP改成了新的IMP

## 注意

- 这里要注意的是如同我们在前面讲rw结构体里讲到的，在运行期的的方法列表是个二维数组
- 每一组方法是会构成一个list，里面有各种方法
- 因此在使用addMethod的时候，同样也是会新建一个list，在里面存放该方法

# 动态添加Ivar

- 对于使用runtime动态API创建的类，可以通过class_addIvar往里面添加实例对象
- 添加时必须在objc_allocateClassPair【创建类】以及objc_registerClassPair【注册类】之间进行

## objc_allocateClassPair

```objective-c
Class objc_allocateClassPair(Class superclass, const char *name, 
                             size_t extraBytes)
{
    Class cls, meta;

    mutex_locker_t lock(runtimeLock);

    // Fail if the class name is in use.
    // 判断类名是否已经被使用
    // Fail if the superclass isn't kosher.
    // 判断父类是否合法
    if (getClass(name)  ||  !verifySuperclass(superclass, true/*rootOK*/)) {
        return nil;
    }

    // Allocate new classes.
    cls  = alloc_class_for_subclass(superclass, extraBytes);
    meta = alloc_class_for_subclass(superclass, extraBytes);

    // fixme mangle the name if it looks swift-y?
    objc_initializeClassPair_internal(superclass, name, cls, meta);
    // 真正初始化

    return cls;
}
```



