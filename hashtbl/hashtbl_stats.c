#include "hashtbl_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* 计算桶分布统计信息 */
int hashtbl_get_bucket_stats(hashtbl_t *hashtbl, bucket_stats_t *stats)
{
    if (!hashtbl || !stats) {
        return -1;
    }

    uint32_t bucket_count = 1 << hashtbl->log_size;
    uint32_t *chain_lengths = (uint32_t *)calloc(bucket_count, sizeof(uint32_t));
    if (!chain_lengths) {
        return -1;
    }

    // 统计每个桶的链长度
    for (uint32_t i = 0; i < bucket_count; i++) {
        hashtbl_link_t *head = hashtbl->tbl[i];
        uint32_t count = 0;
        for (hashtbl_link_t *iter = head; iter; iter = iter->next) {
            count++;
        }
        chain_lengths[i] = count;
    }

    // 计算基本统计量
    uint32_t used_buckets = 0;
    uint32_t max_chain = 0;
    double sum = 0.0;
    double sum_sq = 0.0;

    for (uint32_t i = 0; i < bucket_count; i++) {
        uint32_t len = chain_lengths[i];
        if (len > 0) {
            used_buckets++;
        }
        if (len > max_chain) {
            max_chain = len;
        }
        sum += len;
        sum_sq += (double)len * len;
    }

    stats->bucket_count = bucket_count;
    stats->total_items = hashtbl->num_items;
    stats->used_buckets = used_buckets;
    stats->empty_buckets = bucket_count - used_buckets;
    stats->max_chain_len = max_chain;
    stats->avg_chain_len = sum / bucket_count;
    stats->variance = (sum_sq / bucket_count) - (stats->avg_chain_len * stats->avg_chain_len);
    stats->std_deviation = sqrt(stats->variance);
    stats->load_factor = (double)hashtbl->num_items / bucket_count;
    stats->collision_rate = (hashtbl->num_items > 0) ? 
        ((double)(hashtbl->num_items - used_buckets) / hashtbl->num_items) : 0.0;

    free(chain_lengths);
    return 0;
}

/* 打印直方图 - 链长度分布 */
void hashtbl_print_chain_histogram(hashtbl_t *hashtbl, histogram_config_t *config)
{
    if (!hashtbl) return;

    histogram_config_t default_config = DEFAULT_HISTOGRAM_CONFIG;
    if (!config) {
        config = &default_config;
    }

    uint32_t bucket_count = 1 << hashtbl->log_size;
    uint32_t max_chain = 0;

    // 先找出最大链长度
    for (uint32_t i = 0; i < bucket_count; i++) {
        hashtbl_link_t *head = hashtbl->tbl[i];
        uint32_t count = 0;
        for (hashtbl_link_t *iter = head; iter; iter = iter->next) {
            count++;
        }
        if (count > max_chain) {
            max_chain = count;
        }
    }

    // 分配直方图数组
    uint32_t *histogram = (uint32_t *)calloc(max_chain + 1, sizeof(uint32_t));
    if (!histogram) return;

    // 统计每个链长度的桶数量
    for (uint32_t i = 0; i < bucket_count; i++) {
        hashtbl_link_t *head = hashtbl->tbl[i];
        uint32_t count = 0;
        for (hashtbl_link_t *iter = head; iter; iter = iter->next) {
            count++;
        }
        histogram[count]++;
    }

    // 找出最大值用于缩放
    uint32_t max_count = 0;
    for (uint32_t i = 0; i <= max_chain; i++) {
        if (histogram[i] > max_count) {
            max_count = histogram[i];
        }
    }

    printf("\n========== Chain Length Histogram ==========\n");
    printf("Chain Len | Count      | Percentage | Distribution\n");
    printf("----------+------------+------------+-------------------------------------------\n");

    for (uint32_t i = 0; i <= max_chain && i < config->max_display_buckets; i++) {
        uint32_t count = histogram[i];
        double percent = (double)count * 100.0 / bucket_count;
        
        // 计算条形图长度
        int bar_len = (max_count > 0) ? 
            (int)((double)count * config->bar_width / max_count) : 0;
        
        printf("%9u | %10u | %9.2f%% | ", i, count, percent);
        for (int j = 0; j < bar_len; j++) {
            putchar(config->fill_char);
        }
        printf("\n");
    }

    if (max_chain >= config->max_display_buckets) {
        printf("... (truncated, max chain length: %u)\n", max_chain);
    }

    free(histogram);
}

/* 打印ASCII分布图 - 显示桶索引与链长度的关系 */
void hashtbl_print_ascii_distribution(hashtbl_t *hashtbl, uint32_t display_buckets)
{
    if (!hashtbl) return;

    uint32_t bucket_count = 1 << hashtbl->log_size;
    if (display_buckets == 0 || display_buckets > bucket_count) {
        display_buckets = bucket_count;
    }

    // 采样策略：如果桶数太多，均匀采样
    uint32_t step = (bucket_count + display_buckets - 1) / display_buckets;
    uint32_t actual_display = (bucket_count + step - 1) / step;

    uint32_t *samples = (uint32_t *)calloc(actual_display, sizeof(uint32_t));
    if (!samples) return;

    // 采样每个桶的链长度
    uint32_t idx = 0;
    for (uint32_t i = 0; i < bucket_count && idx < actual_display; i += step) {
        hashtbl_link_t *head = hashtbl->tbl[i];
        uint32_t count = 0;
        for (hashtbl_link_t *iter = head; iter; iter = iter->next) {
            count++;
        }
        samples[idx++] = count;
    }

    // 找出最大值
    uint32_t max_val = 0;
    for (uint32_t i = 0; i < idx; i++) {
        if (samples[i] > max_val) {
            max_val = samples[i];
        }
    }

    printf("\n========== ASCII Distribution Map ==========\n");
    printf("Showing %u buckets (sampled every %u buckets from %u total)\n", 
           idx, step, bucket_count);
    printf("Y-axis: Chain Length, X-axis: Bucket Index\n\n");

    // 打印Y轴和图表
    int height = (max_val > 20) ? 20 : max_val;  // 限制高度
    double scale = (max_val > 0) ? (double)height / max_val : 0;

    for (int row = height; row >= 0; row--) {
        // Y轴标签
        if (row == height) {
            printf("%3u | ", max_val);
        } else if (row == 0) {
            printf("  0 | ");
        } else {
            printf("    | ");
        }

        // 打印该行
        for (uint32_t col = 0; col < idx; col++) {
            double scaled_val = samples[col] * scale;
            if ((int)scaled_val >= row && row > 0) {
                printf("█");
            } else if (row == 0 && samples[col] == 0) {
                printf("·");  // 空桶用点表示
            } else if (row == 0) {
                printf("_");  // 基线
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    // X轴
    printf("    +");
    for (uint32_t i = 0; i < idx; i++) {
        printf("-");
    }
    printf("\n");

    // X轴标签
    printf("     0");
    for (uint32_t i = 1; i <= 4; i++) {
        uint32_t pos = (idx * i) / 4;
        if (pos < idx) {
            printf("\033[%uG%u", 6 + pos, pos * step);
        }
    }
    printf("\n");

    free(samples);
}

/* 打印热力图 - 按区域显示负载 */
void hashtbl_print_heatmap(hashtbl_t *hashtbl, uint32_t regions)
{
    if (!hashtbl || regions == 0) return;

    uint32_t bucket_count = 1 << hashtbl->log_size;
    if (regions > bucket_count) {
        regions = bucket_count;
    }

    uint32_t buckets_per_region = bucket_count / regions;
    double *region_loads = (double *)calloc(regions, sizeof(double));
    uint32_t *region_max = (uint32_t *)calloc(regions, sizeof(uint32_t));
    if (!region_loads || !region_max) {
        free(region_loads);
        free(region_max);
        return;
    }

    // 计算每个区域的负载
    for (uint32_t r = 0; r < regions; r++) {
        uint32_t start = r * buckets_per_region;
        uint32_t end = (r == regions - 1) ? bucket_count : (r + 1) * buckets_per_region;
        
        uint32_t total_items_in_region = 0;
        uint32_t max_in_region = 0;
        
        for (uint32_t i = start; i < end; i++) {
            hashtbl_link_t *head = hashtbl->tbl[i];
            uint32_t count = 0;
            for (hashtbl_link_t *iter = head; iter; iter = iter->next) {
                count++;
            }
            total_items_in_region += count;
            if (count > max_in_region) {
                max_in_region = count;
            }
        }
        
        region_loads[r] = (double)total_items_in_region / (end - start);
        region_max[r] = max_in_region;
    }

    // 找出最大负载用于归一化
    double max_load = 0;
    for (uint32_t r = 0; r < regions; r++) {
        if (region_loads[r] > max_load) {
            max_load = region_loads[r];
        }
    }

    printf("\n========== Load Heatmap ==========\n");
    printf("Region    | Buckets    | Avg Load | Max Chain | Heat\n");
    printf("----------+------------+----------+-----------+------------------------------\n");

    const char *heat_chars[] = {"░", "▒", "▓", "█"};
    int heat_levels = 4;

    for (uint32_t r = 0; r < regions; r++) {
        uint32_t start = r * buckets_per_region;
        uint32_t end = (r == regions - 1) ? bucket_count : (r + 1) * buckets_per_region;
        
        int heat_idx = (max_load > 0) ? 
            (int)((region_loads[r] / max_load) * (heat_levels - 1)) : 0;
        
        printf("[%6u-%-6u] | %10u | %8.2f | %9u | ", 
               start, end - 1, end - start, region_loads[r], region_max[r]);
        
        // 打印热力条
        int bar_len = 20;
        int filled = (max_load > 0) ? 
            (int)((region_loads[r] / max_load) * bar_len) : 0;
        
        for (int i = 0; i < bar_len; i++) {
            if (i < filled) {
                printf("%s", heat_chars[heat_idx]);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("\nHeat scale: ░=low  ▒=medium  ▓=high  █=very high\n");

    free(region_loads);
    free(region_max);
}

/* 打印完整统计报告 */
void hashtbl_print_stats_report(hashtbl_t *hashtbl)
{
    if (!hashtbl) return;

    bucket_stats_t stats;
    if (hashtbl_get_bucket_stats(hashtbl, &stats) != 0) {
        printf("Failed to get bucket statistics\n");
        return;
    }

    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           Hash Table Statistics Report                       ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  Total Buckets:      %10u                              ║\n", stats.bucket_count);
    printf("║  Total Items:        %10u                              ║\n", stats.total_items);
    printf("║  Used Buckets:       %10u (%6.2f%%)                    ║\n", 
           stats.used_buckets, (double)stats.used_buckets * 100.0 / stats.bucket_count);
    printf("║  Empty Buckets:      %10u (%6.2f%%)                    ║\n", 
           stats.empty_buckets, (double)stats.empty_buckets * 100.0 / stats.bucket_count);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  Load Factor:        %10.4f                              ║\n", stats.load_factor);
    printf("║  Collision Rate:     %10.4f%%                            ║\n", stats.collision_rate * 100.0);
    printf("║  Max Chain Length:   %10u                              ║\n", stats.max_chain_len);
    printf("║  Avg Chain Length:   %10.4f                              ║\n", stats.avg_chain_len);
    printf("║  Std Deviation:      %10.4f                              ║\n", stats.std_deviation);
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    // 打印直方图
    hashtbl_print_chain_histogram(hashtbl, NULL);

    // 打印热力图（最多显示16个区域）
    uint32_t regions = (stats.bucket_count > 16) ? 16 : stats.bucket_count;
    hashtbl_print_heatmap(hashtbl, regions);

    // 打印ASCII分布图（如果桶数不是太大）
    if (stats.bucket_count <= 128) {
        hashtbl_print_ascii_distribution(hashtbl, stats.bucket_count);
    } else {
        hashtbl_print_ascii_distribution(hashtbl, 64);
    }
}

/* 导出CSV数据 */
int hashtbl_export_distribution_csv(hashtbl_t *hashtbl, const char *filename)
{
    if (!hashtbl || !filename) return -1;

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to open file: %s\n", filename);
        return -1;
    }

    // 写入CSV头
    fprintf(fp, "bucket_index,chain_length\n");

    uint32_t bucket_count = 1 << hashtbl->log_size;
    for (uint32_t i = 0; i < bucket_count; i++) {
        hashtbl_link_t *head = hashtbl->tbl[i];
        uint32_t count = 0;
        for (hashtbl_link_t *iter = head; iter; iter = iter->next) {
            count++;
        }
        fprintf(fp, "%u,%u\n", i, count);
    }

    fclose(fp);
    printf("Distribution data exported to: %s\n", filename);
    return 0;
}
