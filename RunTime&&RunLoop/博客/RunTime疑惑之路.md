```objective-c
//1
struct objc_object {
    Class _Nonnull isa  OBJC_ISA_AVAILABILITY;
};  //这里的isa没有定是isa_t类型 我佛了
/* OBJC_ISA_AVAILABILITY: `isa` will be deprecated or unavailable 
 * in the future */
#if !defined(OBJC_ISA_AVAILABILITY)
#   if __OBJC2__
#       define OBJC_ISA_AVAILABILITY  __attribute__((deprecated))
#   else
#       define OBJC_ISA_AVAILABILITY  /* still available */
#   endif
#endif
struct objc_object {
    // isa结构体
private:
    isa_t isa;
//在这里讲到了isa的类型是isa_t[objc_private]

//2
#if !__OBJC2__
    Class _Nullable super_class                              OBJC2_UNAVAILABLE;
    const char * _Nonnull name                               OBJC2_UNAVAILABLE;
    long version                                             OBJC2_UNAVAILABLE;
    long info                                                OBJC2_UNAVAILABLE;
    long instance_size                                       OBJC2_UNAVAILABLE;
    struct objc_ivar_list * _Nullable ivars                  OBJC2_UNAVAILABLE;
    struct objc_method_list * _Nullable * _Nullable methodLists                    OBJC2_UNAVAILABLE;
    struct objc_cache * _Nonnull cache                       OBJC2_UNAVAILABLE;
    struct objc_protocol_list * _Nullable protocols          OBJC2_UNAVAILABLE;
#endif
//2006年发布Objective-C 2.0，！__OBJC2__里的内容已经没有用了
  
//3
 #if !OBJC_TYPES_DEFINED //这个宏OBJC_TYPES_DEFINED定义为1，也就是说这里面的定义都是无效的
  
//4
//补充知识assert（）：其作用是如果它的条件返回错误，则终止程序执行。
    
//这个方法的命名是instance，也就是实例对象
//也就是说如果是对象的话直接传入true
inline void 
objc_object::initInstanceIsa(Class cls, bool hasCxxDtor)
{
    assert(!cls->instancesRequireRawIsa());
    assert(hasCxxDtor == cls->hasCxxDtor());

    initIsa(cls, true, hasCxxDtor);
}

inline void 
objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor) 
{ 
    assert(!isTaggedPointer()); 
    
    if (!nonpointer) {
        // 如果访问isa直接返回的是类指针
        //如果是类的的操作的话，直接返回类指针
        isa.cls = cls;
    } else {
        // 实例对象的isa初始化直接走else分支（是isa结构体）
        assert(!DisableNonpointerIsa);
        assert(!cls->instancesRequireRawIsa());

        // 初始化一个心得isa_t结构体
        isa_t newisa(0);

#if SUPPORT_INDEXED_ISA
        assert(cls->classArrayIndex() > 0);
        newisa.bits = ISA_INDEX_MAGIC_VALUE;
        // isa.magic is part of ISA_MAGIC_VALUE
        // isa.nonpointer is part of ISA_MAGIC_VALUE
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.indexcls = (uintptr_t)cls->classArrayIndex();
#else
        // 对新结构体newisa赋值
        // ISA_MAGIC_VALUE的值是0x001d800000000001ULL，转化成二进制是64位
        // 根据注释，使用ISA_MAGIC_VALUE赋值，实际上只是赋值了isa.magic和isa.nonpointer
        newisa.bits = ISA_MAGIC_VALUE;
        // isa.magic is part of ISA_MAGIC_VALUE
        // isa.nonpointer is part of ISA_MAGIC_VALUE
        newisa.has_cxx_dtor = hasCxxDtor;
        // 将当前对象的类指针赋值到shiftcls
        // 类的指针是按照字节（8bits）对齐的，其指针后三位都是没有意义的0，因此可以右移3位
        newisa.shiftcls = (uintptr_t)cls >> 3;
#endif

        // This write must be performed in a single store in some cases
        // (for example when realizing a class because other threads
        // may simultaneously try to use the class).
        // fixme use atomics here to guarantee single-store and to
        // guarantee memory order w.r.t. the class index table
        // ...but not too atomic because we don't want to hurt instantiation
        // 赋值。看注释这个地方不是线程安全的？？
        isa = newisa;
    }
}
  
//5
# elif __x86_64__
#   define ISA_MASK        0x00007ffffffffff8ULL
#   define ISA_MAGIC_MASK  0x001f800000000001ULL
#   define ISA_MAGIC_VALUE 0x001d800000000001ULL
    struct {
        uintptr_t nonpointer        : 1;
        uintptr_t has_assoc         : 1; // 对象是否含有或曾经含有关联引用
        uintptr_t has_cxx_dtor      : 1; // 表示是否有C++析构函数或OC的dealloc
        uintptr_t shiftcls          : 44; // MACH_VM_MAX_ADDRESS 0x7fffffe00000
        uintptr_t magic             : 6;
        uintptr_t weakly_referenced : 1;
        uintptr_t deallocating      : 1;
        uintptr_t has_sidetable_rc  : 1;
        uintptr_t extra_rc          : 8;
#       define RC_ONE   (1ULL<<56)
#       define RC_HALF  (1ULL<<7)
    };
  
//6
// 该方法包括初始化类的read-write数据，并返回真正的类结构
static Class realizeClass(Class cls)
{
    runtimeLock.assertLocked();

    const class_ro_t *ro;
    class_rw_t *rw;
    Class supercls;
    Class metacls;
    bool isMeta;

    if (!cls) return nil;
    // 如果类已经实现了，直接返回
    if (cls->isRealized()) return cls;
    assert(cls == remapClass(cls));
//    printf("cache bits = %d\n",sizeof(cls->cache));
//    printf("super bits = %d\n",sizeof(cls->superclass));
//    printf("isa bits = %d\n",sizeof(cls->ISA()));

    // fixme verify class is not in an un-dlopened part of the shared cache?

    // 编译期间，cls->data指向的是class_ro_t结构体
    // 因此这里强制转成class_ro_t没有问题
    ro = (const class_ro_t *)cls->data();
    //printf("nname = %s",ro->name);
    int flag = strcmp("Person",ro->name);
    if(flag == 0){
        printf("nname = %s\n",ro->name);
    }
    if (ro->flags & RO_FUTURE) {
        // rw结构体已经被初始化（正常不会执行到这里）
        // This was a future class. rw data is already allocated.
        rw = cls->data();
        ro = cls->data()->ro;
        cls->changeInfo(RW_REALIZED|RW_REALIZING, RW_FUTURE);
    } else {
        // 正常的类都是执行到这里
        // Normal class. Allocate writeable class data.
        // 初始化class_rw_t结构体
        rw = (class_rw_t *)calloc(sizeof(class_rw_t), 1);
        // 赋值class_rw_t的class_ro_t，也就是ro
        rw->ro = ro;
        rw->flags = RW_REALIZED|RW_REALIZING;
        // cls->data 指向class_rw_t结构体
        cls->setData(rw);
    }

    isMeta = ro->flags & RO_META;

    rw->version = isMeta ? 7 : 0;  // old runtime went up to 6


    // Choose an index for this class.
    // Sets cls->instancesRequireRawIsa if indexes no more indexes are available
    cls->chooseClassArrayIndex();

    if (PrintConnecting) {
        _objc_inform("CLASS: realizing class '%s'%s %p %p #%u", 
                     cls->nameForLogging(), isMeta ? " (meta)" : "", 
                     (void*)cls, ro, cls->classArrayIndex());
    }

    // Realize superclass and metaclass, if they aren't already.
    // This needs to be done after RW_REALIZED is set above, for root classes.
    // This needs to be done after class index is chosen, for root metaclasses.
    supercls = realizeClass(remapClass(cls->superclass));
    metacls = realizeClass(remapClass(cls->ISA()));

#if SUPPORT_NONPOINTER_ISA
    // Disable non-pointer isa for some classes and/or platforms.
    // Set instancesRequireRawIsa.
    bool instancesRequireRawIsa = cls->instancesRequireRawIsa();
    bool rawIsaIsInherited = false;
    static bool hackedDispatch = false;

    if (DisableNonpointerIsa) {
        // Non-pointer isa disabled by environment or app SDK version
        instancesRequireRawIsa = true;
    }
    else if (!hackedDispatch  &&  !(ro->flags & RO_META)  &&  
             0 == strcmp(ro->name, "OS_object")) 
    {
        // hack for libdispatch et al - isa also acts as vtable pointer
        hackedDispatch = true;
        instancesRequireRawIsa = true;
    }
    else if (supercls  &&  supercls->superclass  &&  
             supercls->instancesRequireRawIsa()) 
    {
        // This is also propagated by addSubclass() 
        // but nonpointer isa setup needs it earlier.
        // Special case: instancesRequireRawIsa does not propagate 
        // from root class to root metaclass
        instancesRequireRawIsa = true;
        rawIsaIsInherited = true;
    }
    
    if (instancesRequireRawIsa) {
        cls->setInstancesRequireRawIsa(rawIsaIsInherited);
    }
// SUPPORT_NONPOINTER_ISA
#endif

    // Update superclass and metaclass in case of remapping
    cls->superclass = supercls;
    cls->initClassIsa(metacls);

    // Reconcile instance variable offsets / layout.
    // This may reallocate class_ro_t, updating our ro variable.
    if (supercls  &&  !isMeta) reconcileInstanceVariables(cls, supercls, ro);

    // Set fastInstanceSize if it wasn't set already.
    cls->setInstanceSize(ro->instanceSize);

    // Copy some flags from ro to rw
    if (ro->flags & RO_HAS_CXX_STRUCTORS) {
        cls->setHasCxxDtor();
        if (! (ro->flags & RO_HAS_CXX_DTOR_ONLY)) {
            cls->setHasCxxCtor();
        }
    }

    // Connect this class to its superclass's subclass lists
    if (supercls) {
        addSubclass(supercls, cls);
    } else {
        addRootClass(cls);
    }

    // Attach categories
    // 将类实现的方法（包括分类）、属性和遵循的协议添加到class_rw_t结构体中的methods、properties、protocols列表中
    methodizeClass(cls);

    return cls;
}

```

