//
//  PersonModel.h
//  KVC-test
//
//  Created by _祀梦 on 2019/7/30.
//  Copyright © 2019 涂强尧. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface PersonModel : NSObject
@property (nonatomic, copy) NSString *key1;
@property (nonatomic, copy) NSString *key2;
@property (nonatomic, copy) NSString *id;
@property (nonatomic, copy) NSString *key3;
@property (nonatomic, copy) NSString *other;

-(void)setValue:(id)value forUndefinedKey:(NSString *)key;
@end

NS_ASSUME_NONNULL_END
