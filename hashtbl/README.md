# EasyHash 使用指南

EasyHash 是一个基于 C 语言的侵入式哈希表库，通过宏封装提供了类型安全且易于使用的 API。

## 目录

- [基础用法](#基础用法)
  - [1. 定义数据结构](#1-定义数据结构)
  - [2. 声明哈希表模板](#2-声明哈希表模板)
  - [3. 实现哈希和比较函数](#3-实现哈希和比较函数)
  - [4. 创建并初始化哈希表实例](#4-创建并初始化哈希表实例)
  - [5. 基本操作（插入、查找、删除）](#5-基本操作插入查找删除)
- [进阶用法](#进阶用法)
  - [6. 遍历和打印](#6-遍历和打印)
  - [7. 多维度索引（同一数据多哈希表）](#7-多维度索引同一数据多哈希表)
  - [8. 动态切换哈希/比较函数](#8-动态切换哈希比较函数)

---

## 基础用法

### 1. 定义数据结构

在你的结构体中使用 `HASHLINK(instance)` 宏嵌入哈希表链接字段：

```c
#include "hashtbl/easyhash.h"

typedef struct user_ {
    hashtbl_link_t HASHLINK(primary);  // 哈希表链接字段，"primary"是实例标识
    uint32_t id;
    char name[32];
    uint8_t age;
} user_t;
```

`HASHLINK(primary)` 会展开为 `u_hashlink_primary`，这是该哈希表实例专用的链接字段。

### 2. 声明哈希表模板

使用 `DECLARE_HASHTBL_TEMPLATE` 宏为指定类型声明哈希表模板：

```c
// 为 user_t 类型声明哈希表模板
DECLARE_HASHTBL_TEMPLATE(user_t);
```

这会生成 `hashtbl_user_t` 类型，包含 `insert`、`del`、`find` 等函数指针。

### 3. 实现哈希和比较函数

```c
// 哈希函数：根据 user->id 计算哈希值
int user_hash_by_id(user_t *user) {
    return hashtbl_hash_uint32(user->id, 1 << 12);  // 使用库提供的 uint32 哈希
}

// 比较函数：比较两个 user 的 id 是否相等
int user_compare_by_id(user_t *a, user_t *b) {
    return a->id == b->id ? 1 : 0;  // 返回1表示相等，0表示不等
}
```

### 4. 创建并初始化哈希表实例

```c
// 创建名为 "primary" 的哈希表实例，绑定 hash/cmp 函数
CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(user_t, primary, 
    user_hash_by_id, user_compare_by_id);

int main() {
    // 初始化哈希表：log_size=12（桶数量=4096），auto_grow=0（不自动扩容）
    INIT_HASHTBL_INSTANCE(user_t, primary, 12, 0);
    
    // ... 使用哈希表
    
    return 0;
}
```

### 5. 基本操作（插入、查找、删除）

```c
int main() {
    INIT_HASHTBL_INSTANCE(user_t, primary, 12, 0);
    
    // 创建用户数据
    user_t user1 = {.id = 1, .name = "Alice", .age = 25};
    user_t user2 = {.id = 2, .name = "Bob", .age = 30};
    
    // 插入
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, primary, &user1);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, primary, &user2);
    
    // 查找
    user_t target = {.id = 2};  // 只需要设置查找字段
    user_t *found = HASHTBL_INSTANCE_FIND_ITEM(user_t, primary, &target);
    if (found) {
        printf("Found: %s, age=%d\n", found->name, found->age);
    }
    
    // 删除
    HASHTBL_INSTANCE_DEL_ITEM(user_t, primary, &user1);
    
    return 0;
}
```

---

## 进阶用法

### 6. 遍历和打印

#### 6.1 自定义打印函数

```c
// 打印函数：返回节点内容的字符串表示
char* print_user(void *_user, void *_args) {
    static char buffer[64];  // 注意：使用静态缓冲区，如需并发请用 args 传递
    user_t *user = (user_t*)_user;
    snprintf(buffer, sizeof(buffer), "[%u:%s]", user->id, user->name);
    return buffer;
}
```

#### 6.2 打印所有元素

```c
int main() {
    INIT_HASHTBL_INSTANCE(user_t, primary, 12, 0);
    
    user_t user1 = {.id = 1, .name = "Alice", .age = 25};
    user_t user2 = {.id = 2, .name = "Bob", .age = 30};
    user_t user3 = {.id = 3, .name = "Charlie", .age = 35};
    
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, primary, &user1);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, primary, &user2);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, primary, &user3);
    
    // 打印哈希表内容
    hashtbl_printer_context_t context;
    HASHTBL_INSTANCE_PRINT_ALL_ITEM(user_t, primary, print_user, NULL, &context);
    // 输出示例：哈希表：(1:[1:Alice])->(2:[2:Bob])->(3:[3:Charlie])
    
    return 0;
}
```

#### 6.3 自定义遍历

```c
// 遍历回调函数
int process_user(user_t *user, void *arg) {
    int *count = (int*)arg;
    (*count)++;
    printf("Processing user: %s\n", user->name);
    return 0;  // 返回0继续遍历，非0停止
}

int main() {
    INIT_HASHTBL_INSTANCE(user_t, primary, 12, 0);
    // ... 插入数据 ...
    
    int count = 0;
    // 使用 traverse 函数指针遍历
    HASHTBL_INSTANCE(user_t, primary).traverse(
        &HASHTBL_INSTANCE(user_t, primary), 
        process_user, 
        &count
    );
    printf("Total users: %d\n", count);
    
    return 0;
}
```

---

### 7. 多维度索引（同一数据多哈希表）

同一数据结构可以同时存在于多个哈希表中，每个哈希表使用不同的链接字段和索引策略。

#### 7.1 定义多链接字段

```c
typedef struct user_ {
    hashtbl_link_t HASHLINK(by_id);      // 用于按ID索引的哈希表
    hashtbl_link_t HASHLINK(by_name);    // 用于按名称索引的哈希表
    hashtbl_link_t HASHLINK(by_age);     // 用于按年龄索引的哈希表
    uint32_t id;
    char name[32];
    uint8_t age;
} user_t;
```

#### 7.2 声明模板并创建多个实例

```c
// 只需声明一次模板
DECLARE_HASHTBL_TEMPLATE(user_t);

// 按ID索引的哈希函数和比较函数
int user_hash_by_id(user_t *user) {
    return hashtbl_hash_uint32(user->id, 1 << 12);
}
int user_cmp_by_id(user_t *a, user_t *b) {
    return a->id == b->id ? 1 : 0;
}

// 按名称索引的哈希函数和比较函数
int user_hash_by_name(user_t *user) {
    return hashtbl_hash_string(user->name, strlen(user->name)) & ((1 << 12) - 1);
}
int user_cmp_by_name(user_t *a, user_t *b) {
    return strcmp(a->name, b->name) == 0 ? 1 : 0;
}

// 按年龄索引的哈希函数和比较函数
int user_hash_by_age(user_t *user) {
    return hashtbl_hash_uint32(user->age, 1 << 12);
}
int user_cmp_by_age(user_t *a, user_t *b) {
    return a->age == b->age ? 1 : 0;
}

// 创建三个独立的哈希表实例
CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(user_t, by_id, user_hash_by_id, user_cmp_by_id);
CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(user_t, by_name, user_hash_by_name, user_cmp_by_name);
CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(user_t, by_age, user_hash_by_age, user_cmp_by_age);
```

#### 7.3 初始化并使用多个哈希表

```c
int main() {
    // 初始化三个哈希表实例
    INIT_HASHTBL_INSTANCE(user_t, by_id, 12, 0);
    INIT_HASHTBL_INSTANCE(user_t, by_name, 12, 0);
    INIT_HASHTBL_INSTANCE(user_t, by_age, 12, 0);
    
    // 创建用户数据
    user_t user1 = {.id = 1, .name = "Alice", .age = 25};
    user_t user2 = {.id = 2, .name = "Bob", .age = 30};
    user_t user3 = {.id = 3, .name = "Alice", .age = 30};  // 同名不同ID
    
    // 同一数据插入到三个哈希表中
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_id, &user1);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_name, &user1);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_age, &user1);
    
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_id, &user2);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_name, &user2);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_age, &user2);
    
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_id, &user3);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_name, &user3);
    HASHTBL_INSTANCE_INSERT_ITEM(user_t, by_age, &user3);
    
    // 按ID查找
    user_t target_by_id = {.id = 2};
    user_t *found_by_id = HASHTBL_INSTANCE_FIND_ITEM(user_t, by_id, &target_by_id);
    printf("Found by ID: %s\n", found_by_id ? found_by_id->name : "not found");
    
    // 按名称查找（返回第一个匹配"Alice"的用户）
    user_t target_by_name = {.name = "Alice"};
    user_t *found_by_name = HASHTBL_INSTANCE_FIND_ITEM(user_t, by_name, &target_by_name);
    printf("Found by name: %s (ID=%u)\n", 
           found_by_name ? found_by_name->name : "not found",
           found_by_name ? found_by_name->id : 0);
    
    // 按年龄查找（返回第一个年龄为30的用户）
    user_t target_by_age = {.age = 30};
    user_t *found_by_age = HASHTBL_INSTANCE_FIND_ITEM(user_t, by_age, &target_by_age);
    printf("Found by age: %s (age=%d)\n", 
           found_by_age ? found_by_age->name : "not found",
           found_by_age ? found_by_age->age : 0);
    
    // 从所有哈希表中删除
    HASHTBL_INSTANCE_DEL_ITEM(user_t, by_id, &user1);
    HASHTBL_INSTANCE_DEL_ITEM(user_t, by_name, &user1);
    HASHTBL_INSTANCE_DEL_ITEM(user_t, by_age, &user1);
    
    return 0;
}
```

---

### 8. 动态切换哈希/比较函数

可以在运行时临时切换哈希表的哈希和比较函数，适用于多策略场景。

#### 8.1 准备多套函数

```c
// 标准ID哈希
typedef enum { STD_ID, FAST_ID, LOOSE_ID } hash_strategy_t;

int std_hash(user_t *user) {
    return hashtbl_hash_uint32(user->id, 1 << 12);
}
int std_cmp(user_t *a, user_t *b) {
    return a->id == b->id ? 1 : 0;
}

// 快速哈希（仅取低8位）
int fast_hash(user_t *user) {
    return user->id & 0xFF;
}
int fast_cmp(user_t *a, user_t *b) {
    return (a->id & 0xFF) == (b->id & 0xFF) ? 1 : 0;
}

// 宽松比较（只比较ID范围）
int loose_hash(user_t *user) {
    return (user->id / 100) % (1 << 12);
}
int loose_cmp(user_t *a, user_t *b) {
    return (a->id / 100) == (b->id / 100) ? 1 : 0;
}
```

#### 8.2 运行时切换策略

```c
CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(user_t, primary, std_hash, std_cmp);

void switch_strategy(hash_strategy_t strategy) {
    switch (strategy) {
        case STD_ID:
            // 恢复默认的标准哈希/比较函数
            RESTORE_HASHTBL_INSTANCE_HASH_CMP_FUNC(user_t, primary);
            break;
        case FAST_ID:
            // 切换到快速哈希函数
            CHANGE_HASHTBL_INSTANCE_HASH_CMP_FUNC(user_t, primary, fast_hash, fast_cmp);
            break;
        case LOOSE_ID:
            // 切换到宽松比较函数
            CHANGE_HASHTBL_INSTANCE_HASH_CMP_FUNC(user_t, primary, loose_hash, loose_cmp);
            break;
    }
}

int main() {
    INIT_HASHTBL_INSTANCE(user_t, primary, 12, 0);
    
    // 插入测试数据
    user_t users[5] = {
        {.id = 100, .name = "User1"},
        {.id = 200, .name = "User2"},
        {.id = 150, .name = "User3"},
        {.id = 250, .name = "User4"},
        {.id = 300, .name = "User5"}
    };
    for (int i = 0; i < 5; i++) {
        HASHTBL_INSTANCE_INSERT_ITEM(user_t, primary, &users[i]);
    }
    
    // 使用标准策略查找（精确匹配ID=150）
    user_t target = {.id = 150};
    user_t *found = HASHTBL_INSTANCE_FIND_ITEM(user_t, primary, &target);
    printf("Standard find ID=150: %s\n", found ? found->name : "not found");
    
    // 切换到宽松策略（按ID范围查找，150和200都在100-199范围）
    switch_strategy(LOOSE_ID);
    user_t target_loose = {.id = 120};  // 查找ID范围100-199
    found = HASHTBL_INSTANCE_FIND_ITEM(user_t, primary, &target_loose);
    printf("Loose find ID=120 (range 100-199): %s\n", found ? found->name : "not found");
    
    // 恢复标准策略
    switch_strategy(STD_ID);
    
    return 0;
}
```

#### 8.3 注意事项

1. **切换时机**：在哈希表非空时切换哈希函数可能导致查找失败，因为已有数据是按照旧哈希函数分布的
2. **恢复函数**：`RESTORE_HASHTBL_INSTANCE_HASH_CMP_FUNC` 会恢复到创建实例时绑定的原始函数
3. **适用场景**：
   - 调试/测试时临时切换比较策略
   - 根据数据特征动态选择哈希算法
   - 实现渐进式搜索（先宽松后精确）

---

## API 参考

### 核心宏

| 宏 | 说明 |
|---|---|
| `HASHLINK(instance)` | 在结构体中定义哈希表链接字段 |
| `DECLARE_HASHTBL_TEMPLATE(type)` | 为指定类型声明哈希表模板 |
| `CREATE_HASHTBL_INSTANCE(type, instance)` | 创建哈希表实例（无哈希/比较函数） |
| `CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(type, instance, hash_func, cmp_func)` | 创建并绑定哈希/比较函数 |
| `INIT_HASHTBL_INSTANCE(type, instance, log_size, auto_grow)` | 初始化哈希表实例 |
| `HASHTBL_INSTANCE_INSERT_ITEM(type, instance, obj)` | 插入元素 |
| `HASHTBL_INSTANCE_FIND_ITEM(type, instance, obj)` | 查找元素 |
| `HASHTBL_INSTANCE_DEL_ITEM(type, instance, obj)` | 删除元素 |
| `HASHTBL_INSTANCE_PRINT_ALL_ITEM(type, instance, print_func, print_args, context)` | 打印所有元素 |
| `CHANGE_HASHTBL_INSTANCE_HASH_CMP_FUNC(type, instance, hash_func, cmp_func)` | 动态切换哈希/比较函数 |
| `RESTORE_HASHTBL_INSTANCE_HASH_CMP_FUNC(type, instance)` | 恢复原始哈希/比较函数 |

### 辅助函数

| 函数 | 说明 |
|---|---|
| `hashtbl_hash_uint32(uint32_t val, uint32_t max_val)` | uint32_t 哈希函数 |
| `hashtbl_hash_string(char *str, size_t len)` | 字符串哈希函数 |

---

## 完整示例

参见 [`demo_easyhash.c`](demo_easyhash.c) 获取完整的使用示例。

## 许可证

MIT License