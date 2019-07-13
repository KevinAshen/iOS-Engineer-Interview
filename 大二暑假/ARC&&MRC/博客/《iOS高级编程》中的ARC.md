[TOC]

|        语句        |     obj0     | obj1 |   obj2   |
| :----------------- | :----------: | :------: | :--: |
| id __strong obj0 = [[NSObject alloc] init];//生成对象A |  A  |    |      |
|      id __strong obj1 = [[NSObject alloc] init];//生成对象B      | A |    B    |      |
|      id __strong obj2 = nil;      |   A    |    B    | nil |
|      obj0 = obj1;//obj0强引用对象B；而对象A不再被ojb0引用，被废弃      |   B    | B | nil |
|  obj2 = obj0;//obj2强引用对象B（现在obj0，ojb1，obj2都强引用对象B）  |    B     |    B    | B |
| obj1 = nil;//obj1不再强引用对象B | B | nil | B |
| obj0 = nil;//obj0不再强引用对象B | nil | nil | B |
| obj2 = nil;//obj2不再强引用对象B，不再有任何强引用引用对象B，对象B被废弃 | nil | nil | nil |
