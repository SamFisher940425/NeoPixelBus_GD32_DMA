# 简介

NeoPixelBus是Arduino平台上著名的LED驱动库，可支持多种总线型LED和数码管等设备

本项目将其移植到GD32平台上，基于SPL标准外设库开发，使用TIMER定时器PWM输出功能，配合DMA功能实现数据发送

# 目录结构

examples文件夹中存储官方库中提供的的各种样例，使用Arduino工程文件格式，文件中Setup函数为初始化内容，Loop函数为主循环内容，可供参考

NeoPixelBus文件夹中存储NeoPixel库源码

systick文件夹存储GD32内置的Systick定时器相关函数

LED文件夹存储类的实例化，LED初始化，基础灯效等函数

# 使用方法

将`NeoPixelBus`和`systick`两文件夹至于同级目录（因为头文件之间有相对路径包含关系，推荐使用PlatformIO开发，文件结构兼容度更好）

按需要配置Systick时钟，默认设置是生成1us的中断，并需要在中断中调用`Time_Accumulation()`函数累加us级计时结果变量

按需要修改`NeoPixelBus\internal\methods\NeoArmMethod.h`中头文件引用

按需要修改上述文件中`dma_pwm_init`和`dma_pwm_send`两函数的具体实现，注意要在外部初始化TIMER的PWM输出功能，此处只初始化了DMA

编译后，参考样例代码进行测试

# Tips

实例化NeoPixelBus时，需要按照实际情况选择颜色模板和灯珠型号，即可正常驱动灯珠。目前已测试的灯珠有WS2812和WS2816系列
