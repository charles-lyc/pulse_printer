import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import find_peaks

# 1. 读取原始数据
data = pd.read_csv('raw.csv', header=None).squeeze()

# 2. 计算原始数据的均值
mean_value = np.mean(data)

# 3. 计算新的均值线，均值线往上加10
new_mean_value = mean_value + 15

# 4. 寻找波峰
# 使用 scipy.find_peaks 函数找到波峰
peaks, _ = find_peaks(data, height=new_mean_value)  # 找到均值线以上的波峰

# 5. 计算每个波峰的质心（基于均值线以上的面积）
centroids = []
for peak in peaks:
    # 提取一个窗口范围（例如：pe
    # ak前后各10个数据点）
    window_size = 10
    start = max(peak - window_size, 0)
    end = min(peak + window_size, len(data))

    # 获取波峰附近的数据片段
    segment = data[start:end]
    indices = np.arange(start, end)

    # 只考虑大于新均值线的部分
    above_mean_segment = segment[segment > new_mean_value]
    above_mean_indices = indices[segment > new_mean_value]

    if len(above_mean_segment) > 0:
        # 计算均值线以上区域的质心：Σx * f(x) / Σf(x)
        centroid = np.sum(above_mean_indices * above_mean_segment) / np.sum(above_mean_segment)
        centroids.append(centroid)

# 转换为 NumPy 数组
centroids = np.array(centroids)

# 去除重复的质心，保留一个
# 这里我们可以选择相邻质心位置较近的合并成一个
unique_centroids = []
threshold = 5  # 设置一个阈值，表示两个质心之间距离小于此值时认为它们是重复的
for centroid in centroids:
    if not unique_centroids or abs(unique_centroids[-1] - centroid) > threshold:
        unique_centroids.append(centroid)

# 转换为 NumPy 数组
unique_centroids = np.array(unique_centroids)

# 6. 输出质心横坐标位置及总个数/总时间
sampling_interval = 0.01  # 每个采样点的间隔，单位为秒（10ms）
total_samples = len(data)  # 总采样点数
total_time = total_samples * sampling_interval  # 总时间，单位秒

print(f"Total number of centroids: {len(unique_centroids)}")
print(f"Total time: {total_time:.2f} seconds")

# 输出质心的横坐标位置
print("Centroid positions (in samples):")
for centroid in unique_centroids:
    print(f"{centroid:.2f}")

# 7. 绘制原始数据和均值线，并标记波峰质心
plt.figure(figsize=(12, 6))

# 绘制原始信号
plt.plot(data, label='Original Signal', color='tab:blue')

# 绘制新的均值线
plt.axhline(y=new_mean_value, color='r', linestyle='--', label='Mean Line + 10')

# 绘制质心标记
# 这里的 `peaks[:len(unique_centroids)]` 确保我们只在 unique_centroids 数组的长度范围内进行标记
plt.scatter(unique_centroids, data[peaks[:len(unique_centroids)]], color='tab:orange', zorder=5, label='Centroids', marker='X')

# 添加图例和标签
plt.title('Signal with Peaks and Centroids')
plt.xlabel('Samples')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend()

# 显示图形
plt.tight_layout()
plt.show()
