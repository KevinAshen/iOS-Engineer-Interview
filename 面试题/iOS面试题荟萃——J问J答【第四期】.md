[toc]

# 主演

- J先生：
  - 简介：单身帅气，热情潇洒，热爱iOS开发，目前在努力集齐苹果全家桶
  - GitHub：[KevinAshen](https://github.com/KevinAshen)
  - CSDN：[太阳骑士索拉尔](https://blog.csdn.net/KevinAshen)
  - 简书：[太阳骑士索拉尔](https://www.jianshu.com/u/0de1c21e79ca)
- J先生：
  - 简介：单身帅气，热情潇洒，热爱iOS开发，目前在努力集齐苹果全家桶
  - GitHub：[KevinAshen](https://github.com/KevinAshen)
  - CSDN：[太阳骑士索拉尔](https://blog.csdn.net/KevinAshen)
  - 简书：[太阳骑士索拉尔](https://www.jianshu.com/u/0de1c21e79ca)
- 说明：
	- 学无止境

## Category相关

### load、initialize的区别，以及它们在category重写的时候的调用的次序

- 区别在于调用方式和调用时刻
- 调用方式：
  - load是根据函数地址直接调用
  - initialize是通过objc_msgSend调用
- 调用时刻：
  - load是runtime加载类、分类的时候也就是程序刚运行的时候调用（只会调用1次）
  - initialize是类第一次接收到消息的时候调用，每一个类只会initialize一次（父类的initialize方法可能会被调用多次）
- 调用顺序：
  - 先调用类的load方法，再调用分类的load方法，先编译哪个类，就先调用哪个类的load。在调用load之前会先调用父类的load方法。分类中load方法不会覆盖本类的load方法，先编译的分类优先调用load方法。
  - initialize先初始化父类，之后再初始化子类。如果子类没有实现+initialize，会调用父类的+initialize（所以父类的+initialize可能会被调用多次），如果分类实现了+initialize，就覆盖类本身的+initialize调用。

### 如何给Category添加属性?

- 使用关联对象的特性
- 其本质是，runtime维护了一个全局的哈希Map，存储着所有设定好的的关联对象，在这其中使用对象的地址作为key，关联对象的地址作为value进行存储，两者建立可以查找的联系
- 使用到的API：
  - objc_setAssociatedObject(id _Nonnull object, const void * _Nonnull key, id _Nullable value, objc_AssociationPolicy policy), OBJC_AVAILABLE(10.6, 3.1, 9.0, 1.0, 2.0);
    - OBJC_AVAILABLE可以设置存储策略，也就相当于属性关键字
  - objc_getAssociatedObject(id _Nonnull object, const void * _Nonnull key), OBJC_AVAILABLE(10.6, 3.1, 9.0, 1.0, 2.0);
  - objc_removeAssociatedObjects(id _Nonnull object), OBJC_AVAILABLE(10.6, 3.1, 9.0, 1.0, 2.0);