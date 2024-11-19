#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SZ 1000           // 假设你的数据数组大小
#define THRESHOLD_MULTIPLIER 1.2 // 均值乘以 1.2 的倍数

// 函数声明
double calculate_mean(double *data, int size);
int count_above_mean_regions(double *data, int size, double mean_line);

int main()
{
    // 假设你已经有了这个数组，以下是示例数据
    double heart_rate_value[BUFFER_SZ] = {/* 填充你的数据 */};

    // 计算均值
    double mean_value = calculate_mean(heart_rate_value, BUFFER_SZ);
    double mean_line = mean_value * THRESHOLD_MULTIPLIER; // 计算均值线

    // 输出均值和均值线
    printf("Mean value: %.2f\n", mean_value);
    printf("Mean line (mean * 1.2): %.2f\n", mean_line);

    // 统计大于均值线的独立区域个数
    int region_count = count_above_mean_regions(heart_rate_value, BUFFER_SZ, mean_line);
    printf("Number of regions above the mean line: %d\n", region_count);

    return 0;
}

// 计算数组的均值
double calculate_mean(double *data, int size)
{
    double sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        sum += data[i];
    }
    return sum / size;
}

// 统计大于均值线的独立区域的个数
int count_above_mean_regions(double *data, int size, double mean_line)
{
    int count = 0;
    int in_region = 0; // 标志位，表示当前是否在一个大于均值线的区域内

    for (int i = 0; i < size; i++)
    {
        if (data[i] > mean_line)
        { // 如果当前数据点大于均值线
            if (!in_region)
            { // 如果当前不在区域内，说明新区域开始
                count++;
                in_region = 1; // 进入大于均值线的区域
            }
        }
        else
        {                  // 当前数据点小于或等于均值线
            in_region = 0; // 离开区域
        }
    }

    return count;
}
