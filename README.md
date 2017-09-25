# BLHWScaner
这是一个Windows下的硬件扫描和测试工具

# 开发环境 <br>
VS2010 C++ <br>
Qt4.7.1 <br>

# 程序截图 <br>
主程序: <br>
![](https://github.com/BurnellLiu/BLHWScaner/blob/master/ReadMeImg/main.PNG) <br>

温度监控: <br>
![](https://github.com/BurnellLiu/BLHWScaner/blob/master/ReadMeImg/temp.PNG) <br>

磁盘测试: <br>
![](https://github.com/BurnellLiu/BLHWScaner/blob/master/ReadMeImg/disktest.PNG) <br>

GPU测试： <br>
![](https://github.com/BurnellLiu/BLHWScaner/blob/master/ReadMeImg/gputest.PNG) <br>

# Release Note: <br>

**2017/07/18 V1.4.8 Release** <br>
1. 增加功能: RestartAging测试可以切换为S3或S4测试 <br>
2. 增加功能: RestartAging中增加丢失设备检查 <br>

**2017/07/18 V1.4.8 Release** <br>
1. 增加功能: GPU压力测试 <br>

**2017/06/21 V1.4.7 Release** <br>
1. 增加功能: 温度监视页面显示当前CPU频率 <br>

**2017/04/25 V1.4.6 Release** <br>
1. 增加功能: Win8之后机器显示BIOS模式(Legacy, 或UEFI) <br>

**2017/03/13 V1.4.5 Release** <br>
1. Fix Bug: 磁盘速度测试，4K随机大小设置为512M后，不进行4K随机测试 <br>

**2017/03/08 V1.4.4 Release** <br>
1. 解决BUG: 一些机器上卡死在获取传感器信息步骤 <br>
2. 增加功能: 显示特殊设备提示信息, 如: SSD, 独立显卡等 <br>
3. 增加生物识别设备信息的获取 <br>
4. 增加系统度量信息的获取(是否支持触摸屏) <br>

**2017/02/10 V1.4.3 Release** <br>
1. 支持识别NVME SSD <br>

**2017/02/08 V1.4.2 Release** <br>
1. 解决BUG：不能正确显示AMD显卡温度 <br>

**2017/02/08 V1.4.1 Release** <br>
1. 解决BUG：网卡信息显示错误 <br>

