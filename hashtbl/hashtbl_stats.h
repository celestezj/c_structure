#ifndef __HASHTBL_STATS_H__
#define __HASHTBL_STATS_H__

#include "hashtbl.h"
#include <stdint.h>

/* 桶分布统计信息 */
typedef struct bucket_stats_ {
    uint32_t bucket_count;      // 总桶数
    uint32_t total_items;       // 总item数
    uint32_t used_buckets;      // 非空桶数
    uint32_t empty_buckets;     // 空桶数
    uint32_t max_chain_len;     // 最大链长度
    double   avg_chain_len;     // 平均链长度
    double   variance;          // 方差
    double   std_deviation;     // 标准差
    double   load_factor;       // 负载因子
    double   collision_rate;    // 冲突率
} bucket_stats_t;

/* 直方图分桶配置 */
typedef struct histogram_config_ {
    uint32_t max_display_buckets;   // 最大显示的直方图条数
    uint32_t bar_width;             // 每个条的宽度（字符数）
    char     fill_char;             // 填充字符
} histogram_config_t;

/* 默认配置 */
#define DEFAULT_HISTOGRAM_CONFIG {20, 50, '#'}

/* 统计函数 */
int hashtbl_get_bucket_stats(hashtbl_t *hashtbl, bucket_stats_t *stats);

/* 打印直方图 - 显示链长度分布 */
void hashtbl_print_chain_histogram(hashtbl_t *hashtbl, histogram_config_t *config);

/* 打印ASCII分布图 - 显示桶索引与链长度的关系 */
void hashtbl_print_ascii_distribution(hashtbl_t *hashtbl, uint32_t display_buckets);

/* 打印热力图 - 按区域显示负载 */
void hashtbl_print_heatmap(hashtbl_t *hashtbl, uint32_t regions);

/* 打印完整统计报告 */
void hashtbl_print_stats_report(hashtbl_t *hashtbl);

/* 导出CSV数据 */
int hashtbl_export_distribution_csv(hashtbl_t *hashtbl, const char *filename);

#endif
