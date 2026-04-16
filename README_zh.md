# window_manager

-   [简介](#简介)
-   [目录](#目录)
-   [约束](#约束)
-   [接口说明](#接口说明)
-   [相关仓](#相关仓)

## 简介

**窗口子系统** 提供窗口管理和Display管理的基础能力，是系统图形界面显示所需的基础子系统。

其主要的结构如下图所示：

![窗口子系统架构图](./docs/figures/WindowManager.png)

- **Window Manager Client**

    应用进程窗口管理接口层，提供窗口对对象抽象和窗口管理接口，对接原能力和UI框架。

- **Display Manager Client**

    应用进程Display管理接口层，提供Display信息抽象和Display管理接口。

- **Window Manager Server**

    窗口管理服务，提供窗口布局、Z序控制、窗口树结构、窗口拖拽、窗口快照等能力，并提供窗口布局和焦点窗口给多模输入

- **Display Manager Server**

    Display管理服务，提供Display信息、屏幕截图、屏幕亮灭和亮度处理控制，并处理Display与Screen映射关系

## 架构说明
当前部件，即`window_manager`，同时包含了窗口管理服务的两种架构，分别称为“分离架构”和“合一架构”。

主要区别如下图：
![窗口子系统新老架构差异](./docs/figures/WindowManager-Architectures.png)

- **分离架构**
桌面相关的系统应用，例如桌面、壁纸等，均位于应用层，且各自独立进程。因此在这种架构下，应用的启动退出等管理过程就包含了桌面相关的系统应用到系统服务、系统服务到应用的多次IPC通信过程。

- **合一架构**
桌面相关的系统应用，和窗口管理服务进程合一，不再是以独立的进程运行，而是转变为一个个系统**窗口控件**。因此，窗口管理子系统提供了[**窗口控件**](https://gitcode.com/openharmony/arkui_ace_engine/blob/master/frameworks/core/components_ng/pattern/window_scene/scene)，用于实现窗口的控件化管理和窗口控件的布局管理。
此外，也提供了[**屏幕控件**](https://gitcode.com/openharmony/arkui_ace_engine/blob/master/frameworks/core/components_ng/pattern/window_scene/screen)用于屏幕的控件化管理。

### 切换方式
**分离架构**和**合一架构**可以通过全局特性配置项 `window_manager_use_sceneboard` 实现切换。
具体配置方式，请参考：[特性配置规则](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/subsystems/subsys-build-feature.md)

### 不同架构的关键区别
一方面窗口的管理方式转变为通过窗口控件完成，另一方面桌面相关的系统应用转变为系统窗口控件，所以二者在启动退出等任务管理流程上发生了关键性改变。
- **分离架构**
![分离架构下的启动流程](./docs/figures/WindowManager-StartSteps-Separated.png)

- **合一架构**
![合一架构下的启动流程](./docs/figures/WindowManager-StartSteps-Unified.png)

## 目录
```
foundation/window/window_manager/
├── dm                              # Dislplay Manager Client实现代码    
├── dmserver                        # Dislplay Manager Service实现代码  
├── extension                       # Ability Component 窗口相关代码实现目录  
│   ├── extension_connection        # Ability Component 嵌入部分 
│   └── window_extension            # Ability Component 被嵌入部分
├── interfaces                      # 对外接口存放目录   
│   ├── innerkits                   # native接口存放目录   
│   └── kits                        # js/napi接口存放目录  
├── previewer                       # IDE轻量模拟器窗口代码实现目录   
├── resources                       # 框架使用资源文件存放目录   
├── sa_profile                      # 系统服务配置文件
├── snapshot                        # 截屏命令行工具实现代码 
├── test                            # Fuzz测试和系统测试用例存放目录 
├── utils                           # 工具类存放目录  
├── window_scene                    # 合一架构 Window Manager Service实现代码 
├── wm                              # Window Manager Client实现代码  
└── wmserver                        # 分离架构 Window Manager Service实现代码  
```

## 约束
- 语言版本
    - C++11或以上

## 接口说明

- [Window](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkui/arkts-apis-window.md)
- [Display](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkui/js-apis-display.md)

## 相关仓
- [graphic_graphic_2d](https://gitcode.com/openharmony/graphic_graphic_2d)
- [arkui_ace_engine](https://gitcode.com/openharmony/arkui_ace_engine)
- [ability_ability_runtime](https://gitcode.com/openharmony/ability_ability_runtime)
- [multimodalinput_input](https://gitcode.com/openharmony/multimodalinput_input)