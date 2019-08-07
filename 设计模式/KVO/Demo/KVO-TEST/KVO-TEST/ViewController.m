//
//  ViewController.m
//  KVO-TEST
//
//  Created by Kevin.J on 2019/8/2.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "ViewController.h"
#import "Person.h"
#import <objc/runtime.h>

NSInteger changeInt = 1;



@interface ViewController ()

@property (nonatomic, strong) UIButton *addButton;
@property (nonatomic, strong) UIButton *deleteButton;
@property (nonatomic, strong) UIButton *changeButton;
@property (nonatomic, strong) UIButton *removeButton;

@property (nonatomic, copy) NSString *testStr;

@property (nonatomic, strong) Person *kvoPerson;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.kvoPerson = [[Person alloc] init];
    
    self.testStr = @"QSTSD";
    
    self.addButton = [[UIButton alloc] initWithFrame:CGRectMake(100, 100, 100, 100)];
    [self.view addSubview:_addButton];
    
    [_addButton setBackgroundColor:[UIColor blueColor]];
    [_addButton setTitle:@"添加" forState:UIControlStateNormal];
    [_addButton addTarget:self action:@selector(clickButton:) forControlEvents:UIControlEventTouchUpInside];
    _addButton.tag = 101;
    
    self.deleteButton = [[UIButton alloc] initWithFrame:CGRectMake(200, 200, 100, 100)];
    [self.view addSubview:_deleteButton];
    [_deleteButton setBackgroundColor:[UIColor greenColor]];
    [_deleteButton setTitle:@"删除" forState:UIControlStateNormal];
    [_deleteButton addTarget:self action:@selector(clickButton:) forControlEvents:UIControlEventTouchUpInside];
    _deleteButton.tag = 102;
    
    self.changeButton = [[UIButton alloc] initWithFrame:CGRectMake(300, 300, 100, 100)];
    [self.view addSubview:_changeButton];
    [_changeButton setBackgroundColor:[UIColor grayColor]];
    [_changeButton setTitle:@"改变" forState:UIControlStateNormal];
    [_changeButton addTarget:self action:@selector(clickButton:) forControlEvents:UIControlEventTouchUpInside];
    _changeButton.tag = 103;
    
    self.removeButton = [[UIButton alloc] initWithFrame:CGRectMake(100, 500, 100, 100)];
    [self.view addSubview:_removeButton];
    [_removeButton setBackgroundColor:[UIColor redColor]];
    [_removeButton setTitle:@"移除" forState:UIControlStateNormal];
    [_removeButton addTarget:self action:@selector(clickButton:) forControlEvents:UIControlEventTouchUpInside];
    _removeButton.tag = 104;
    
    self.kvoPerson = [[Person alloc] init];
    
    
}

- (void)clickButton:(UIButton *)sender {
    NSLog(@"CLICK");
    if (sender.tag == 101) {
        NSLog(@"CLICK1");
        [self addObserver:self.kvoPerson forKeyPath:@"testStr" options:NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew context:nil];
    } else if (sender.tag == 102) {
        NSLog(@"CLICK2");
        [self removeObserver:self.kvoPerson forKeyPath:@"testStr"];
    } else if (sender.tag == 103) {
        NSLog(@"CLICK3");
        self.testStr = @"QSTSD";
    } else {
        _kvoPerson = nil;
    }
}


//- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
//    if ([keyPath isEqual:@"testStr"]) {
//        NSLog(@"oldValue:%@", [change objectForKey:@"old"]);
//        NSLog(@"newValue:%@", [change objectForKey:@"new"]);
//    }
//}


@end
