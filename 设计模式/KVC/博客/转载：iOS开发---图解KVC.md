目录
=================

   * [iOS开发---详解KVC](#ios开发---详解kvc)
      * [什么是KVC？](#什么是kvc)
      * [KVC方法全览](#kvc方法全览)
         * [KVC基础操作](#kvc基础操作)
            * [KVC取值](#kvc取值)
               * [取值方法](#取值方法)
               * [基于getter取值底层实现](#基于getter取值底层实现)
            * [KVC设值](#kvc设值)
               * [赋值方法](#赋值方法)
               * [基于setter赋值底层实现](#基于setter赋值底层实现)
         * [KVC批量操作](#kvc批量操作)
         * [KVC集合属性操作](#kvc集合属性操作)
            * [集合操作符](#集合操作符)
            * [数组操作符](#数组操作符)
            * [嵌套操作符](#嵌套操作符)
         * [KVC与容器类](#kvc与容器类)
            * [KVC与有序容器(NSMutableArray)](#kvc与有序容器nsmutablearray)
               * [取值方法](#取值方法-1)
               * [NSMutableArray取值底层实现](#nsmutablearray取值底层实现)
            * [KVC与无序容器(NSMutableSet)](#kvc与无序容器nsmutableset)
               * [取值方法](#取值方法-2)
               * [NSMutableSet取值底层实现](#nsmutableset取值底层实现)
         * [KVC异常处理](#kvc异常处理)
         * [KVC处理非对象](#kvc处理非对象)
         * [KVC属性验证](#kvc属性验证)
      * [KVC适用场景](#kvc适用场景)
         * [动态的取值和设值](#动态的取值和设值)
         * [Model和字典转换](#model和字典转换)
         * [用KVC来访问和修改私有变量](#用kvc来访问和修改私有变量)
         * [修改一些控件的内部属性](#修改一些控件的内部属性)
         

# iOS开发---图解KVC[原作者:_祀梦](https://github.com/tqy582144930)

## 什么是KVC？

> KVC（Key-value coding）键值编码，单看这个名字可能不太好理解。其实是指iOS的开发中，可以允许开发者通过Key名直接访问对象的属性，或者给对象的属性赋值。这样就可以在运行时动态地访问和修改对象的属性。而不是在编译时确定，很多高级的iOS开发技巧都是基于KVC实现的。目前网上关于KVC的文章在非常多，有的只是简单地说了下用法，我会运用图解的方式写下这遍文章就是为了让大家更好的理解。

## KVC方法全览

> `KVC`提供了一种间接访问其属性方法或成员变量的机制，可以通过字符串来访问对应的属性方法或成员变量。

![KVC方法全览](http://ww4.sinaimg.cn/large/006tNc79ly1g5nx6lrhusj309t0oktki.jpg)

### KVC基础操作

#### KVC取值

##### 取值方法

1. 通过key

```objective-c
- (nullable id)valueForKey:(NSString *)key;                          //直接通过Key来取值
```

2. 通过keyPath

```objective-c
- (nullable id)valueForKeyPath:(NSString *)keyPath;                  //通过KeyPath来取值
```

##### 基于getter取值底层实现

> 当调用`valueForKey`的代码时，其搜索方式如下：
>
> 你需要先看一下这张流程图，大致知道如何运转的，之后再看文字描述，仔细了解其机制

![基于getter取值](http://ww1.sinaimg.cn/large/006tNc79ly1g5ou9gjqooj31tj0oegsg.jpg)

1. 通过`getter`方法搜索实例，按照`get<Key>`, `<key>`, `is<Key>`, `_<key>的顺序查找`getter`方法。如果发现符合的方法，就调用对应的方法并拿着结果跳转到第五步。否则，就继续到下一步。

2. 如果没有找到简单的`getter`方法，则搜索其匹配模式的方法`countOf<Key>`、`objectIn<Key>AtIndex:`、`<key>AtIndexes:`。

   如果找到其中的第一个和其他两个中的一个，则就会返回一个可以响应`NSArray`所有方法的代理集合(它是`NSKeyValueArray`，是`NSArray`的子类)。或者说给这个代理集合发送属于`NSArray`的方法，就会以`countOf<Key>`,`objectIn<Key>AtIndex`或`<Key>AtIndexes`这几个方法组合的形式调用。否则，继续到第三步。

   代理对象随后将`NSArray`接收到的`countOf<Key>`、`objectIn<Key>AtIndex:`、`<key>AtIndexes:`的消息给符合`KVC`规则的调用方。

   当代理对象和`KVC`调用方通过上面方法一起工作时，就会允许其行为类似于`NSArray`一样。

3. 如果没有找到`NSArray`简单存取方法，或者`NSArray`存取方法组。那么会同时查找`countOf<Key>`、`enumeratorOf<Key>`、`memberOf<Key>:`命名的方法。

   如果找到三个方法，则创建一个集合代理对象，该对象响应所有`NSSet`方法并返回。否则，继续执行第四步。

   给这个代理对象发`NSSet`的消息，就会以`countOf<Key>`，`enumeratorOf<Key>`,`memberOf<Key>`组合的形式调用。

4. 如果没有发现简单`getter`方法，或集合存取方法组，以及接收类方法`accessInstanceVariablesDirectly`是返回`YES`的。搜索一个名为`_<key>`、`_is<Key>`、`<key>`、`is<Key>`的实例，根据他们的顺序。

   如果发现对应的实例，则立刻获得实例可用的值并跳转到第五步，如果重写了类方法`+ (BOOL)accessInstanceVariablesDirectly`返回NO的话，那么会直接调用`valueForUndefinedKey:`。

5. 如果取回的是一个对象指针，则直接返回这个结果。
    如果取回的是一个基础数据类型，但是这个基础数据类型是被`NSNumber`支持的，则存储为`NSNumber`并返回。
    如果取回的是一个不支持`NSNumber`的基础数据类型，则通过`NSValue`进行存储并返回。

6. 如果所有情况都失败，则调用`valueForUndefinedKey:`方法并抛出异常，这是默认行为。但是子类可以重写此方法。

#### KVC设值

##### 赋值方法

1. 通过key

- 直接将属性名当做`key`，并设置`value`，即可对属性进行赋值。

  ```objective-c
  - (void)setValue:(nullable id)value forKey:(NSString *)key;          //通过Key来设值
  ```

2. 通过keyPath

- 除了对当前对象的属性进行赋值外，还可以对其更“深层”的对象进行赋值。`KVC`进行多级访问时，直接类似于属性调用一样用点语法进行访问即可。例如`Person`属性中有`name`属性，我就可以通过`Person.name`进行赋值

  ```objective-c
  - (void)setValue:(nullable id)value forKeyPath:(NSString *)keyPath;  //通过KeyPath来设值
  ```

##### 基于setter赋值底层实现

> 这是`setValue:forKey:`的默认实现，给定输入参数`value`和`key`。试图在接收调用对象的内部，设置属性名为`key`的`value`，通过下面的步骤
>
> 你需要先看一下这张流程图，大致知道如何运转的，之后再看文字描述，仔细了解其机制

![基于setter搜素](http://ww2.sinaimg.cn/large/006tNc79ly1g5nzl3txetj31390mtwhw.jpg)

1. 查找`set<Key>:`或`_set<Key>`命名的`setter`，按照这个顺序，如果找到的话，代码通过`setter`方法完成设置。

2. 如果没有找到`setter`方法，KVC机制会检查`+ (BOOL)accessInstanceVariablesDirectly`的返回值，如果`accessInstanceVariablesDirectly`类属性返回`YES`，则查找一个命名规则为`_<key>`、`_is<Key>`、`<key>`、`is<Key>`的实例变量。根据这个顺序，如果发现则将`value`赋值给实例变量，如果返回值为NO，KVC会执行`setValue：forUndefinedKey：`方法。

3. 如果没有发现`setter`或实例变量，则调用`setValue:forUndefinedKey:`方法，并默认提出一个异常，但是一个`NSObject`的子类可以提出合适的行为。

### KVC批量操作

- 在对象调用`setValuesForKeysWithDictionary:`方法时，可以传入一个包含`key`、`value`的字典进去，`KVC`可以将所有数据按照属性名和字典的`key`进行匹配，并将`value`给`User`对象的属性赋值。

  ```objective-c
  //创建一个model模型，里面的字符串名称必须和key的名称对应，不然该方法会崩溃
  @interface PersonModel : NSObject
  @property (nonatomic, copy) NSString *key1;
  @property (nonatomic, copy) NSString *key2;
  @property (nonatomic, copy) NSString *id;
  @property (nonatomic, copy) NSString *key3;
  @property (nonatomic, copy) NSString *other;
  @end
    
  PersonModel *person = [[PersonModel alloc] init];
  //1.这是直接赋值，数据量小会很简单，但是数据量一多就很麻烦，就像我们进行网络请求时
  person.key1 = dictionary[@"key1"];
  person.key2 = dictionary[@"key2"];
  person.key3 = dictionary[@"key3"];
  
  //2.通过下面该方法可以批量赋值
  //2.1如果model里面的string不存在于dictionary中，输出结果为null；
  [person setValuesForKeysWithDictionary:dictionary];
  NSLog(@"\n%@\n%@\n%@\n%@\n", person.key1,person.key2,person.key3,person.other);
  
  //输出结果
  test1
  test2
  test3
  (null)
  
  //2.2如果dictionary中有的元素，moedl中没有运行会直接出错，那么我们应该怎么解决？
  //我们需要实现setValue:forUndefinedKey:这个方法能过滤掉不存在的键值
  -(void)setValue:(id)value forUndefinedKey:(NSString *)key{
    //这里我们不需要写任何内容
  }
  person.key1 = dictionary[@"key1"];
  person.key2 = dictionary[@"key2"];
  person.key3 = dictionary[@"key3"];
  [person setValuesForKeysWithDictionary:dictionary];
  NSLog(@"\n%@\n%@\n%@\n", person.key1,person.key2,person.key3);
  
  //输出结果
  test1
  test2
  test3
  
  //2.3如果dictionar中的key与model中的变量名字不同，怎么赋值？
  //还是从setValue:forUndefinedKey:这个方法入手
  -(void)setValue:(id)value forUndefinedKey:(NSString *)key{
    if ([key isEqualToString:@"key2"]) {
      self.id = value;
  }
  person.key1 = dictionary[@"key1"];
  person.id = dictionary[@"key2"];
  person.key3 = dictionary[@"key3"];
  [person setValuesForKeysWithDictionary:dictionary];
  NSLog(@"\n%@\n%@\n%@\n", person.key1,person.id,person.key3);
    
  //输出结果
  test1
  test2
  test3
  ```



### KVC集合属性操作

> `KVC`提供的`valueForKeyPath:`方法非常强大，可以通过该方法对集合对象进行“深入”操作，在其`keyPath`中嵌套集合运算符，例如求一个数组中对象某个属性的`count`。(集合对象主要指`NSArray`和`NSSet`，但不包括`NSDictionary`)

![集合运算符格式](http://ww4.sinaimg.cn/large/006tNc79ly1g5owry8m7aj30dz01d3yf.jpg)

上面表达式主要分为三部分，`left`部分是要操作的集合对象，如果调用`KVC`的对象本来就是集合对象，则`left`可以为空。中间部分是表达式，表达式一般以@符号开头。后面是进行运算的属性。

- 为了验证操作符，我们需要先建立一个Model类

```objective-c
@interface Transaction : NSObject
@property (nonatomic, strong) NSString *payee;
@property (nonatomic, strong) NSNumber *amount;
@property (nonatomic, strong) NSDate *date;
@end

@interface BankAccount : NSObject
@property (nonatomic, strong) NSArray *transcationArray;
@end
```

#### 集合操作符

> 处理集合包含的对象，并根据操作符的不同返回不同的类型，返回值以`NSNumber`为主。

```objective-c
//@avg用来计算集合中right keyPath指定的属性的平均值
NSNumber *transactionAverage = [bankAccount.transcationArray valueForKeyPath:@"@avg.amount"];
NSLog(@"@avg = %@", transactionAverage);

//@count用来计算集合的总数
NSNumber *numberOfTransactions = [bankAccount.transcationArray valueForKeyPath:@"@count"];
NSLog(@"@count = %@", numberOfTransactions);
//备注：@count操作符比较特殊，它不需要写right keyPath，即使写了也会被忽略。

//@sum用来计算集合中right keyPath指定的属性的总和。
NSNumber *amountSum = [bankAccount.transcationArray valueForKeyPath:@"@sum.amount"];
NSLog(@"@sum = %@", amountSum);

//@max用来查找集合中right keyPath指定的属性的最大值
NSNumber *amountMax = [bankAccount.transcationArray valueForKeyPath:@"@max.amount"];
NSLog(@"@max = %@", amountMax);

//@min用来查找集合中right keyPath指定的属性的最小值。
NSNumber *amountMin = [bankAccount.transcationArray valueForKeyPath:@"@min.amount"];
NSLog(@"@min = %@", amountMin);
```

#### 数组操作符

> 根据操作符的条件，将符合条件的对象包含在数组中返回。

```objective-c
//@unionOfObjects将集合对象中，所有payee对象放在一个数组中并返回
NSArray *payees = [bankAccount.transcationArray valueForKeyPath:@"@unionOfObjects.payee"];
NSLog(@"@unionOfObjects = %@", payees);

//@distinctUnionOfObjects将集合对象中，所有payee对象放在一个数组中，并将数组进行去重后返回。
NSArray *distinctPayees = [bankAccount.transcationArray valueForKeyPath:@"@distinctUnionOfObjects.payee"];
NSLog(@"@distinctUnionOfObjects = %@", distinctPayees);
//注意：以上两个方法中，如果操作的属性为nil，在添加到数组中时会导致Crash。
```

#### 嵌套操作符

> 处理集合对象中嵌套其他集合对象的情况，返回结果也是一个集合对象。

```objective-c
//@distinctUnionOfArrays是用来操作集合内部的集合对象，将所有right keyPath对应的对象放在一个数组中，并进行排重。
NSArray *collectedPayees = [allArray valueForKeyPath:@"@unionOfArrays.payee"];
NSLog(@"@unionOfArrays = %@", collectedPayees);

//@distinctUnionOfSets是用来操作集合内部的集合对象，将所有right keyPath对应的对象放在一个set中，并进行排重。
NSArray *collectedDistinctPayees = [allArray valueForKeyPath:@"@distinctUnionOfArrays.payee"];
NSLog(@"@distinctUnionOfArrays = %@", collectedDistinctPayees);
```

### KVC与容器类

> 对象的属性可以是一对一的，也可以是一对多的。一对多的属性要么是有序的(数组)，要么是无序的(集合)。

> ⚠️：根据`KVO`的实现原理，是在运行时生成新的子类并重写其`setter`方法，在其内容发生改变时发送消息。但这只是对属性直接进行赋值会触发，如果属性是容器对象，对容器对象进行`add`或`remove`操作，则不会调用`KVO`的方法。可以通过`KVC`对应的`API`来配合使用，使容器对象内部发生改变时也能触发`KVO`。
>
> 在进行容器对象操作时，先通过`key`或者`keyPath`获取集合对象，然后再对容器对象进行`add`或`remove`等操作时，就会触发`KVO`的消息通知了。

#### KVC与有序容器(NSMutableArray)

##### 取值方法

1. 通过key

```objective-c
- (NSMutableArray *)mutableArrayValueForKey:(NSString *)key;
//该方法返回一个可变有序数组
```

2. 通过keyPath

```objective-c
- (NSMutableArray *)mutableArrayValueForKeyPath:(NSString *)keyPath;
//该方法返回一个可变有序数组
```

##### NSMutableArray取值底层实现

> 当调用`mutableArrayValueForKey`的代码时，其搜索方式如下：
>
> 你需要先看一下这张流程图，大致知道如何运转的，之后再看文字描述，仔细了解其机制

![NSMutableArray取值底层实现](http://ww1.sinaimg.cn/large/006tNc79ly1g5owaipsrxj31jz0mr0yg.jpg)

1. 搜索`insertObject:in<Key>AtIndex:` , `removeObjectFrom<Key>AtIndex:` 或者 `insert<Key>AdIndexes` , `remove<Key>AtIndexes` 格式的方法
    如果至少找到一个`insert`方法和一个`remove`方法，那么同样返回一个可以响应`NSMutableArray`所有方法代理集合(类名是`NSKeyValueFastMutableArray`)，那么给这个代理集合发送`NSMutableArray`的方法，以`insertObject:in<Key>AtIndex:` , `removeObjectFrom<Key>AtIndex:` 或者 `insert<Key>AdIndexes` , `remove<Key>AtIndexes`组合的形式调用。

   当对象接收一个`mutableArrayValueForKey:`消息并实现可选替换方法，例如`replaceObjectIn<Key>AtIndex:withObject:`或`replace<Key>AtIndexes:with<Key>:`方法，代理对象会在适当的情况下使用它们，以获得最佳性能。

2. 如果上步的方法没有找到，则搜索`set<Key>:` 格式的方法，如果找到，那么发送给代理集合的`NSMutableArray`最终都会调用`set<Key>:`方法。 

   也就是说，`mutableArrayValueForKey:`取出的代理集合修改后，用`set<Key>:` 重新赋值回去去。这样做效率会低很多。所以推荐实现上面的方法。

3. 如果上一步的方法还还没有找到，再检查类方法`+ (BOOL)accessInstanceVariablesDirectly`,如果返回YES(默认行为)，会按`_<key>`,`<key>`,的顺序搜索成员变量名，如果找到，那么发送的`NSMutableArray`消息方法直接交给这个成员变量处理。

4. 如果还是找不到，则调用`valueForUndefinedKey:`

#### KVC与无序容器(NSMutableSet)

##### 取值方法

1. 通过key

```objective-c
- (NSMutableSet *)mutableSetValueForKey:(NSString *)key;
//方法返回一个可变的无序数组
```

2. 通过keyPath

```objective-c
- (NSMutableSet *)mutableSetValueForKeyPath:(NSString *)keyPath;
//方法返回一个可变的无序数组
```



##### NSMutableSet取值底层实现

> 当调用`NSMutableSet`的代码时，其搜索方式如下：
>
> 你需要先看一下这张流程图，大致知道如何运转的，之后再看文字描述，仔细了解其机制

![NSMutableSet取值底层实现](http://ww4.sinaimg.cn/large/006tNc79ly1g5owb7lu9ej31pi0i7jvm.jpg)

1. 搜索`addObject<Key>Object:` , `remove<Key>Object:` 或者 `add<Key>` , `remove<Key>` 格式的方法
    如果至少找到一个`insert`方法和一个`remove`方法，那么同样返回一个可以响应`NSMutableSet`所有方法代理集合(类名是`NSKeyValueFastMutableSet2`)，那么给这个代理集合发送`NSMutableSet`的方法，以`addObject<Key>Object:` , `remove<Key>Object:` 或者 `add<Key>` , `remove<Key>`组合的形式调用。还有两个可选实现的接口：`intersect<Key> , set<Key>:` 。

2. 如果`receiver`是`ManagedObject`，那么就不会继续搜索。

3. 如果上一步的方法没有找到，则搜索`set<Key>`: 格式的方法，如果找到，那么发送给代理集合的`NSMutableSet`最终都会调用`set<Key>:`方法。 也就是说，`mutableSetValueForKey`取出的代理集合修改后，用`set<Key>:` 重新赋值回去去。这样做效率会低很多。所以推荐实现上面的方法。

4. 如果上一步的方法还没有找到，再检查类方法`+ (BOOL)accessInstanceVariablesDirectly`,如果返回`YES`(默认行为)，会按`_<key>`,`<key>`的顺序搜索成员变量名，如果找到，那么发送的`NSMutableSet`消息方法直接交给这个成员变量处理。

5. 如果还是找不到，调用`valueForUndefinedKey:`
    可见，除了检查`receiver`是`ManagedObject`以外，其搜索顺序和`mutableArrayValueForKey`基本一至

### KVC异常处理

1. key或者keyPath发生错误

> 当根据`KVC`搜索规则，没有搜索到对应的`key`或者`keyPath`，则会调用对应的异常方法。异常方法的默认实现，在异常发生时会抛出一个`NSUndefinedKeyException`的异常，并且应用程序`Crash`

我们可以重写下面两个方法:

```objective-c
- (nullable id)valueForUndefinedKey:(NSString *)key;
- (void)setValue:(nullable id)value forUndefinedKey:(NSString *)key;
```

2. 传参为nil

> 通常情况下，KVC不允许你要在调用`setValue：属性值 forKey：`(或者keyPath)时**对非对象**传递一个`nil`的值。因为值类型是不能为`nil`的。如果你不小心传了，KVC会调用`setNilValueForKey:`方法。这个方法默认是抛出异常，所以一般而言最好还是重写这个方法。

我们可以重写这个方法：

```objective-c
-(void)setNilValueForKey:(NSString *)key{
    NSLog(@"不能将%@设成nil",key);
}
```

### KVC处理非对象

> KVC是支持基础数据类型和结构体的，可以在`setter`和`getter`的时候，通过`NSValue`和`NSNumber`来转换为`OC`对象。该方法`valueForKey：`总是返回一个id对象，如果原本的变量类型是值类型或者结构体，返回值会封装成`NSNumber`或者`NSValue`对象。这两个类会处理从数字，布尔值到指针和结构体任何类型。然后开发者需要手动转换成原来的类型。尽管`valueForKey：`会自动将值类型封装成对象，但是`setValue：forKey：`却不行。你必须手动将值类型转换成`NSNumber`或者`NSValue`类型，才能传递过去。

- 可以调用`initWithBool:`方法对基础数据类型进行包装

```objective-c
@property (nonatomic, assign, readonly) BOOL boolValue;
- (NSNumber *)initWithBool:(BOOL)value 
```

### KVC属性验证

> KVC提供了属性值,用来验证key对应的Value是否可用的方法

- 在调用`KVC`时可以先进行验证，验证通过下面两个方法进行，支持`key`和`keyPath`两种方式。验证方法默认实现返回`YES`，可以通过重写对应的方法修改验证逻辑。

  验证方法需要我们手动调用，并不会在进行`KVC`的过程中自动调用。

```objective-c
- (BOOL)validateValue:(inout id _Nullable * _Nonnull)ioValue forKey:(NSString *)inKey error:(out NSError **)outError;
- (BOOL)validateValue:(inout id _Nullable * _Nonnull)ioValue forKeyPath:(NSString *)inKeyPath error:(out NSError **)outError;
```

> 这个方法的默认实现是去探索类里面是否有一个这样的方法：`-(BOOL)validate<Key>:error:`如果有这个方法，就调用这个方法来返回，没有的话就直接返回`YES`

```objective-c
@implementation Address
-(BOOL)validateCountry:(id *)value error:(out NSError * _Nullable __autoreleasing *)outError{  //在implementation里面加这个方法，它会验证是否设了非法的value
    NSString* country = *value;
    country = country.capitalizedString;
    if ([country isEqualToString:@"Japan"]) {
        return NO;                                                                             //如果国家是日本，就返回NO，这里省略了错误提示，
    }
    return YES;
}
@end
NSError* error;
id value = @"japan";
NSString* key = @"country";
BOOL result = [add validateValue:&value forKey:key error:&error]; //如果没有重写-(BOOL)-validate<Key>:error:，默认返回Yes
if (result) {
    NSLog(@"键值匹配");
    [add setValue:value forKey:key];
}
else{
    NSLog(@"键值不匹配"); //不能设为日本，其他国家都行
}
NSString* country = [add valueForKey:@"country"];
NSLog(@"country:%@",country);
//打印结果 
KVCDemo[867:58871] 键值不匹配
KVCDemo[867:58871] country:China
```

## KVC适用场景

### 动态的取值和设值

> 利用KVC动态的取值和设值是最基本的用途了。相信每一个iOS开发者都能熟练掌握

### Model和字典转换

> 在上面KVC批量操作已阐述

### 用KVC来访问和修改私有变量

> 根据上面的实现原理我们知道，`KVC`本质上是操作方法列表以及在内存中查找实例变量。我们可以利用这个特性访问类的私有变量，例如下面在`.m`中定义的私有成员变量和属性，都可以通过`KVC`的方式访问。
>
> 这个操作对`readonly`的属性，`@protected`的成员变量，都可以正常访问。如果不想让外界访问类的成员变量，则可以将`accessInstanceVariablesDirectly`属性赋值为`NO`。

### 修改一些控件的内部属性

> 这也是iOS开发中必不可少的小技巧。众所周知很多UI控件都由很多内部UI控件组合而成的，但是Apple度没有提供这访问这些控件的API，这样我们就无法正常地访问和修改这些控件的样式。而KVC在大多数情况可下可以解决这个问题。



