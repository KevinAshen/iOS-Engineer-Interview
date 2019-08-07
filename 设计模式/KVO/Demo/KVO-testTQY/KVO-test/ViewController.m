//
//  ViewController.m
//  KVO-test
//
//  Created by _祀梦 on 2019/8/1.
//  Copyright © 2019 涂强尧. All rights reserved.
//

#import "ViewController.h"
#import <objc/runtime.h>
#import "Person.h"

@interface ViewController ()
@property (nonatomic, strong) Person *person1;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.person1 = [[Person alloc] init];
    self.person1.age = 28;
    self.person1.name = @"tu";
    [self addObserver];
}

- (void)addObserver
{
    NSKeyValueObservingOptions option = NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew;
    
    NSLog(@"person1添加KVO监听对象之前-类对象 -%@", object_getClass(self.person1));
    NSLog(@"person1添加KVO监听之前-方法实现 -%p", [self.person1 methodForSelector:@selector(setAge:)]);
    NSLog(@"person1添加KVO监听之前-元类对象 -%@", object_getClass(object_getClass(self.person1)));
    
    [self.person1 addObserver:self forKeyPath:@"age" options:option context:@"age chage"];
    [self.person1 addObserver:self forKeyPath:@"name" options:option context:@"name change"];
    
    NSLog(@"person1添加KVO监听对象之后-类对象 -%@", object_getClass(self.person1));
    NSLog(@"person1添加KVO监听之后-方法实现 -%p", [self.person1 methodForSelector:@selector(setAge:)]);
    NSLog(@"person1添加KVO监听之后-元类对象 -%@", object_getClass(object_getClass(self.person1)));
}

/**
 观察者监听的回调方法
 @param keyPath 监听的keyPath
 @param object 监听的对象
 @param change 更改的字段内容
 @param context 注册时传入的地址值
 */
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
//    NSLog(@"监听到%@的%@属性值改变了 - %@ - %@", object, keyPath, change, context);
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    self.person1.age = 29;
    self.person1.name = @"qiangyao";
}

- (void)dealloc
{
    [self removeObserver];
}

- (void)removeObserver
{
    [self.person1 removeObserver:self forKeyPath:@"age"];
    [self.person1 removeObserver:self forKeyPath:@"name"];
}

@end
