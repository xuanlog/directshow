环境：Vs2019 + Qt插件 + directshow

这是利用 QCamera 和 directshow 编写的摄像头调用代码
可以设置电力线频率、亮度等基本参数，可以切换摄像头
#设置界面为了保证界面的统一性，因此为额外设计并通过调用操作单个参数的 directshow API 实现，有需要的朋友可以看看
#写的有点粗糙，所以不支持启动程序后插入或拔出设备，请在启动程序前插入至少一台摄像头设备以保证程序正常运行
