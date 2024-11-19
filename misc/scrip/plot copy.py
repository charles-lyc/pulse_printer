import matplotlib.pyplot as plt
import pandas as pd

# 读取 CSV 文件
file_path = 'raw.csv'  # 请确保 raw.csv 在当前工作目录中，或指定完整路径
data = pd.read_csv(file_path, header=None, names=['Column1', 'Column2'])

# 生成时间轴（假设采样间隔为 10ms）
time = [i * 0.01 for i in range(len(data))]  # 时间单位为秒

# 提取两列数据
column1 = data['Column1']
column2 = data['Column2']
column2*=10

# 绘制曲线
plt.figure(figsize=(10, 6))
plt.plot(time, column1, label='Column 1', color='blue')
plt.plot(time, column2, label='Column 2', color='red')
plt.xlabel('Time (s)')
plt.ylabel('Value')
plt.title('Raw Data Visualization')
plt.legend()
plt.grid(True)

# 显示图表
plt.show()
