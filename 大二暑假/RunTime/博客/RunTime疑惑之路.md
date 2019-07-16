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
```

