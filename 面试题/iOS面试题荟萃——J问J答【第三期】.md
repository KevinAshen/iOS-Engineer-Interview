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
	- 是的，我们依然没有找到女主角，依然还是J先生自导自尬演
	- 今天是Category大礼包，关于load和initialize的部分会放在下一篇

## Category相关

### Category 的实现原理？

- 在编译期生成对应的category_t结构体，存储在分类中我们写的相关方法，协议等信息
- 在运行期将该结构体里的方法列表等拷贝到类对象的方法列表中

### Category为什么不能添加属性？

- Category中的内容是在运行期编入类对象的，此时类对象的成员变量内存布局已经定死，无法进行修改
- 我们也就无法在运行期给对象添加成员变量，也就更无法添加属性

### Category的好处？

- 可以减少单个文件的体积
- 可以把不同的功能组织到不同的Category里
- 可以由多个开发者共同完成一个类
- 可以按需加载想要的Category

### Category的使用场景？

- 给已存在的类添加方法
- 模拟多继承（另外可以模拟多继承的还有 protocol）
- 把 framework&&私有类 的私有方法公开
  - 在Category中申明同名方法但是不重写，由于方法调用机制会找到私有方法

### 比较Category与extension？

- extension在编译期决议，它就是类的一部分，伴随类的产生而产生，亦随之一起消亡，因此可以添加属性。extension一般用来隐藏类的私有信息，可以添加方法，属性，申明实现一些不想被外界调用的私有方法，因此必须有一个类的源码才能为一个类添加extension，这也是为什么无法为系统的类比如NSString添加extension
  - extension一个常见的用处就是可以在.h文件申明变量的时候用readonly，在.m文件写extension的时候重新申明成readwrite，这样可以做到外部调用的时候只读，内部调用的时候可以读写
- 而category在编译期只是创建了Category_t结构体，真正编入相关类是到运行期才会进行，我们不能添加属性，因为此时类的内部布局已经固定

### 添加同名方法会发生什么？

- 会发生方法覆盖，即调用到的方法会是分类中添加的那个方法而不是本类中的那个方法

- 并且如果是在多个分类中加入了同名方法的话，调用到的会是最后编译的那个分类里的

- 扩展：怎样调用到原来的同名方法？

  - 遍历类对象的方法列表，根据SEL比对，找到最后一个与之匹配的method，获取其对应的IMP

  - 根据IMP，SEL，执行该方法

  - 代码：

    ![img](https://tva1.sinaimg.cn/large/006y8mN6ly1g8z4nfga7jj30me0inmzw.jpg)