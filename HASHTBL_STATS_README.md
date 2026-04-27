# 哈希表桶分布分析工具使用说明

## 概述

本工具用于分析哈希表中元素的分布情况，帮助评估哈希函数的质量和哈希表的性能。

## 编译

```bash
gcc demo_stats.c hashtbl/hashtbl.c hashtbl/hashtbl_stats.c -o demo_stats -lm
```

## 统计指标说明

### 1. 负载因子 (Load Factor)

**含义**：每个桶平均存储的元素数量。

**公式**：`负载因子 = 总元素数 / 总桶数`

**解读**：
- `< 1`：桶比元素多，比较稀疏
- `= 1`：平均每个桶1个元素
- `> 1`：必然存在冲突

---

### 2. 冲突率 (Collision Rate)

**含义**：发生冲突的元素占总元素的比例。

**公式**：`冲突率 = (总元素数 - 已用桶数) / 总元素数`

**解读**：
- `0%`：完美哈希，无冲突
- `37.5%`：约37.5%的元素发生了冲突
- 越高说明哈希分布越不均匀

---

### 3. 标准差 (Standard Deviation)

**含义**：衡量各桶链长度的离散程度，反映分布均匀性。

**公式**：
```
方差 = Σ(每个桶链长度 - 平均链长)² / 总桶数
标准差 = √方差
```

**解读**：
- `= 0`：所有桶链长度相同，完美均匀
- 越小：分布越均匀
- 越大：存在热点桶（某些桶链特别长）

---

## API 接口

### 头文件

```c
#include "hashtbl/hashtbl_stats.h"
```

### 核心函数

#### 1. 获取统计信息

```c
bucket_stats_t stats;
hashtbl_get_bucket_stats(ht, &stats);

// 访问统计字段
printf("负载因子: %.4f\n", stats.load_factor);
printf("冲突率: %.2f%%\n", stats.collision_rate * 100);
printf("标准差: %.4f\n", stats.std_deviation);
```

#### 2. 打印完整报告

```c
hashtbl_print_stats_report(ht);
```

输出包含：
- 统计指标汇总
- 链长度直方图
- 区域热力图
- ASCII分布图

#### 3. 单独打印直方图

```c
hashtbl_print_chain_histogram(ht, NULL);
```

显示不同链长度的桶数量分布。

#### 4. 打印热力图

```c
hashtbl_print_heatmap(ht, 16);  // 分成16个区域
```

按区域显示负载情况，识别热点。

#### 5. 导出CSV

```c
hashtbl_export_distribution_csv(ht, "distribution.csv");
```

导出每个桶的索引和链长度，供外部分析。

---

## 使用示例

### 基础用法

```c
#include "hashtbl/hashtbl.h"
#include "hashtbl/hashtbl_stats.h"

// 创建并填充哈希表
hashtbl_t *ht = hashtbl_init(12, offsetof(my_struct, link), 0);
// ... 插入元素 ...

// 打印完整统计报告
hashtbl_print_stats_report(ht);

// 导出数据
hashtbl_export_distribution_csv(ht, "stats.csv");
```

### 自定义直方图

```c
histogram_config_t config = {
    .max_display_buckets = 15,  // 最多显示15个条
    .bar_width = 40,            // 每个条40个字符宽
    .fill_char = '*'            // 用*填充
};
hashtbl_print_chain_histogram(ht, &config);
```

---

## 输出示例

```
╔══════════════════════════════════════════════════════════════╗
║           Hash Table Statistics Report                       ║
╠══════════════════════════════════════════════════════════════╣
║  Total Buckets:            4096                              ║
║  Total Items:              5000                              ║
║  Used Buckets:             3125 (76.29%)                     ║
║  Empty Buckets:             971 (23.71%)                     ║
╠══════════════════════════════════════════════════════════════╣
║  Load Factor:               1.2207                           ║
║  Collision Rate:            37.50%                           ║
║  Max Chain Length:              8                            ║
║  Avg Chain Length:           1.2207                          ║
║  Std Deviation:              0.8923                          ║
╚══════════════════════════════════════════════════════════════╝

========== Chain Length Histogram ==========
Chain Len | Count      | Percentage | Distribution
----------+------------+------------+-------------------------------------------
        0 |        971 |     23.71% | ####################
        1 |       1953 |     47.68% | ##########################################
        2 |        976 |     23.83% | ####################
        3 |        183 |      4.47% | ###
        4 |         12 |      0.29% | #
```

---

## 分析建议

1. **负载因子 > 2**：考虑扩容哈希表
2. **冲突率 > 50%**：检查哈希函数质量
3. **标准差 > 平均链长**：存在严重的热点桶，需要优化哈希函数
4. **最大链长 >> 平均链长**：可能存在哈希攻击或数据分布极不均匀
