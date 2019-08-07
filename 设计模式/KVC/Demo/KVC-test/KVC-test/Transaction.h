//
//  Transaction.h
//  KVC-test
//
//  Created by _祀梦 on 2019/7/30.
//  Copyright © 2019 涂强尧. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface Transaction : NSObject
@property (nonatomic, strong) NSString *payee;
@property (nonatomic, strong) NSNumber *amount;
@property (nonatomic, strong) NSDate *date;
@end

@interface BankAccount : NSObject
@property (nonatomic, strong) NSArray *transcationArray;
@end

NS_ASSUME_NONNULL_END
