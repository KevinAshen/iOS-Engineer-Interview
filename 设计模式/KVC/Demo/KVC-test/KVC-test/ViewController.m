//
//  ViewController.m
//  KVC-test
//
//  Created by _祀梦 on 2019/7/30.
//  Copyright © 2019 涂强尧. All rights reserved.
//

#import "ViewController.h"
#include "PersonModel.h"
#include "Transaction.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:@"test1",@"key1",
                                     @"test2",@"key2",
                                     @"test3",@"key3",
                                     @"test4",@"key4", nil];
    NSLog(@"%@", [dictionary valueForKey:@"key1"]);
    NSDictionary *dictionary1 = [dictionary dictionaryWithValuesForKeys:@[@"key2", @"key1"]];
    NSLog(@"%@", dictionary1);
    
    //创建一个model模型，里面的字符串名称必须和key的名称对应，不然该方法会崩溃
    PersonModel *person = [[PersonModel alloc] init];
    //1.这是直接赋值，数据量小会很简单，但是数据量一多就很麻烦，就像我们进行网络请求时
    person.key1 = dictionary[@"key1"];
    person.key2 = dictionary[@"key2"];
    person.key3 = dictionary[@"key3"];
    
    //2.通过下面该方法可以批量赋值
    //2.1如果model里面的string不存在于dictionary中，输出结果为null；
    [person setValuesForKeysWithDictionary:dictionary];
    NSLog(@"\n%@\n%@\n%@\n%@\n", person.key1,person.key2,person.key3,person.other);
    
    //2.2如果dictionary中有的元素，moedl中没有运行会直接出错，那么我们应该怎么解决？
    //我们需要实现setValue:forUndefinedKey:这个方法能过滤掉不存在的键值
    //============请跳转至PersonModel文件中============
    person.key1 = dictionary[@"key1"];
    person.key2 = dictionary[@"key2"];
    person.key3 = dictionary[@"key3"];
    [person setValuesForKeysWithDictionary:dictionary];
    NSLog(@"\n%@\n%@\n%@\n", person.key1,person.key2,person.key3);
    
    //2.3如果dictionar中的key与model中的变量名字不同，怎么赋值？
    //还是从setValue:forUndefinedKey:这个方法入手
    //============请跳转至PersonModel文件中============
    person.key1 = dictionary[@"key1"];
    person.id = dictionary[@"key2"];
    person.key3 = dictionary[@"key3"];
    [person setValuesForKeysWithDictionary:dictionary];
    NSLog(@"\n%@\n%@\n%@\n", person.key1,person.id,person.key3);
  
    
    //----------------------------集合运算符-----------------------------
    //---------------------------1.集合操作符-------------------------
    //处理集合包含的对象，并根据操作符的不同返回不同的类型，返回值以NSNumber为主。
    Transaction *transaction1 = [[Transaction alloc] init];
    transaction1.payee = @"xiaowang";
    transaction1.amount = [NSNumber numberWithInt:1];
    Transaction *transaction2 = [[Transaction alloc] init];
    transaction2.payee = @"xiaowang";
    transaction2.amount = [NSNumber numberWithInt:2];
    Transaction *transaction3 = [[Transaction alloc] init];
    transaction3.payee = @"xiaoliu";
    transaction3.amount = [NSNumber numberWithInt:3];
    Transaction *transaction4 = [[Transaction alloc] init];
    transaction4.payee = @"xiaozhang";
    transaction4.amount = [NSNumber numberWithInt:4];
    
    BankAccount *bankAccount = [[BankAccount alloc] init];
    bankAccount.transcationArray = [[NSArray alloc] initWithObjects:transaction1, transaction2, transaction3, transaction4, nil];
    
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
    
    
    //---------------------------2.数组操作符-------------------------
    //根据操作符的条件，将符合条件的对象包含在数组中返回
    
    //@unionOfObjects将集合对象中，所有payee对象放在一个数组中并返回
    NSArray *payees = [bankAccount.transcationArray valueForKeyPath:@"@unionOfObjects.payee"];
    NSLog(@"@unionOfObjects = %@", payees);
    
    //@distinctUnionOfObjects将集合对象中，所有payee对象放在一个数组中，并将数组进行去重后返回。
    NSArray *distinctPayees = [bankAccount.transcationArray valueForKeyPath:@"@distinctUnionOfObjects.payee"];
    NSLog(@"@distinctUnionOfObjects = %@", distinctPayees);
    //注意：以上两个方法中，如果操作的属性为nil，在添加到数组中时会导致Crash。
    

    //---------------------------3.嵌套操作符-------------------------
    //处理集合对象中嵌套其他集合对象的情况，返回结果也是一个集合对象。
    Transaction *transaction5 = [[Transaction alloc] init];
    transaction5.payee = @"xiaomei";
    transaction5.amount = [NSNumber numberWithInt:5];
    Transaction *transaction6 = [[Transaction alloc] init];
    transaction6.payee = @"xiaoxi";
    transaction6.amount = [NSNumber numberWithInt:6];
    NSArray *transcationArray2 = [[NSArray alloc] initWithObjects:transaction4,transaction5,transaction6, nil];
    NSArray *allArray = @[bankAccount.transcationArray, transcationArray2];
    
    //@distinctUnionOfArrays是用来操作集合内部的集合对象，将所有right keyPath对应的对象放在一个数组中，并进行排重。
    NSArray *collectedPayees = [allArray valueForKeyPath:@"@unionOfArrays.payee"];
    NSLog(@"@unionOfArrays = %@", collectedPayees);
    
    //@distinctUnionOfSets是用来操作集合内部的集合对象，将所有right keyPath对应的对象放在一个set中，并进行排重。
    NSArray *collectedDistinctPayees = [allArray valueForKeyPath:@"@distinctUnionOfArrays.payee"];
    NSLog(@"@distinctUnionOfArrays = %@", collectedDistinctPayees);
    
    
    //属性验证
    //验证通过下面两个方法进行，支持key和keyPath两种方式。验证方法默认实现返回YES，
    //可以通过重写对应的方法修改验证逻辑。
    NSError *error;
    NSString *payee = @"xiaotu";
    if (![dictionary validateValue:&payee forKey:@"d" error:&error]) {
        NSLog(@"error = %@", error);
    }
    
}



@end
