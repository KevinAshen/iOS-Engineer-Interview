```objective-c
//1
NSObject *obj0 = [[NSObject alloc] init];
NSObject * obj1 __weak = obj0;
haveOld:0 haveNew:1 crashIfDeallocating:1(lldb) 
  
//2
objc_loadWeakRetained(id *location)
{
    id obj;
    id result;
    Class cls;

    SideTable *table;
    
 retry:
    // fixme std::atomic this load
    obj = *location;
    if (!obj) return nil;
    if (obj->isTaggedPointer()) return obj;
    
    table = &SideTables()[obj];
    
    table->lock();
    if (*location != obj) {
        table->unlock();
        goto retry;
    }
    
    result = obj;

    cls = obj->ISA();
    if (! cls->hasCustomRR()) {
        // Fast case. We know +initialize is complete because
        // default-RR can never be set before then.
        assert(cls->isInitialized());
        if (! obj->rootTryRetain()) {
            result = nil;
        }
    }
    else {
        // Slow case. We must check for +initialize and call it outside
        // the lock if necessary in order to avoid deadlocks.
        if (cls->isInitialized() || _thisThreadIsInitializingClass(cls)) {
            BOOL (*tryRetain)(id, SEL) = (BOOL(*)(id, SEL))
                class_getMethodImplementation(cls, SEL_retainWeakReference);
            if ((IMP)tryRetain == _objc_msgForward) {
                result = nil;
            }
            else if (! (*tryRetain)(obj, SEL_retainWeakReference)) {
                result = nil;
            }
        }
        else {
            table->unlock();
            _class_initialize(cls);
            goto retry;
        }
    }
        
    table->unlock();
    return result;
}
```

