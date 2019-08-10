//
//  ViewController.m
//  Responder-Test
//
//  Created by Kevin.J on 2019/8/5.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "ViewController.h"
#import "RTTouchView.h"

@interface ViewController ()



@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    RTTouchView *viewA = [[RTTouchView alloc] initWithFrame:self.view.bounds];
    [self.view addSubview:viewA];
    
    viewA.backgroundColor = [UIColor grayColor];
    
    UITextField *nameTextField = [[UITextField alloc] initWithFrame:CGRectMake(50, 600, 100, 100)];
    [viewA addSubview:nameTextField];
    
    nameTextField.backgroundColor = [UIColor yellowColor];
    
    RTTouchView *viewB = [[RTTouchView alloc] initWithFrame:CGRectMake(50, 50, 200, 200)];
    [viewA addSubview:viewB];
    
    viewB.backgroundColor = [UIColor redColor];
    
    UIButton *testButton = [[UIButton alloc] initWithFrame:CGRectMake(50, 50, 100, 100)];
    [viewB addSubview:testButton];
    
    [testButton setBackgroundColor:[UIColor whiteColor]];
    [testButton addTarget:self action:@selector(touch) forControlEvents:UIControlEventTouchUpInside];
    
    RTTouchView *viewF = [[RTTouchView alloc] initWithFrame:CGRectMake(10, 10, 30, 30)];
    [testButton addSubview:viewF];
    
    viewF.backgroundColor = [UIColor redColor];
    
    RTTouchView *viewC = [[RTTouchView alloc] initWithFrame:CGRectMake(50, 300, 200, 200)];
    [viewA addSubview:viewC];
    
    viewC.backgroundColor = [UIColor blueColor];
    
    RTTouchView *viewD = [[RTTouchView alloc] initWithFrame:CGRectMake(50, 50, 50, 50)];
    [viewC addSubview:viewD];
    
    viewD.backgroundColor = [UIColor greenColor];
   
    
    
    RTTouchView *viewE = [[RTTouchView alloc] initWithFrame:CGRectMake(50, 100, 50, 50)];
    [viewC addSubview:viewE];
    
    viewE.backgroundColor = [UIColor blackColor];
    
    NSLog(@"viewA:%p, viewB:%p, viewC:%p, viewD:%p, viewE:%p", viewA, viewB, viewC, viewD, viewE);
    
    
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    NSLog(@"viewContrller");
    [self.view endEditing:YES];
}

- (void)touch {
    NSLog(@"click");
}

@end
