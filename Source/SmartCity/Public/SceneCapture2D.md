# ue4 多相机分屏与小地图效果实现教程
参考教程：[https://www.cnblogs.com/sevenyuan/p/7687424.html](https://www.cnblogs.com/sevenyuan/p/7687424.html)，原教程有些错误，故重写如下：

通过使用ue4的UI和rendertarget来实现，优点就是可以随意设置，缺点就是略烦（其实还可以）。

## 场景中拖入scene capture 2d
![](https://i.imgur.com/iu1Uetx.jpg)

具体可以看文档[https://docs.unrealengine.com/latest/INT/Resources/ContentExamples/Reflections/1_7/](https://docs.unrealengine.com/latest/INT/Resources/ContentExamples/Reflections/1_7/)

这种可以做镜子，小地图，监控等等效果

## content栏右击建立 materials & textures ->Render Target
![](https://i.imgur.com/3QlFE7A.png)

建立以后拖入到刚刚2d摄像机的target texture里面

![](https://i.imgur.com/VVDVoqf.png)

然后双击Render Target可以看到摄像机画面（也可以做画面比例大小分辨率甚至是滤镜的设置，这个感觉比unity方便好多）
（4.12.3貌似有bug就是Render Target如果是默认选项是看不到摄像机预览图的，但是后面做上去是对的，如果scene capture 2d模式选成final color话就能看到）

## 右击Render Target然后选第一项create material
![](https://i.imgur.com/oXaSITP.png)

然后双击打开按照下面左侧选择类型然后连线

![](https://i.imgur.com/zOk38lQ.png)


## content里新建widge blueprint


**这里不用设置蓝图的构造器，只需设置控件即可，原博客有误**

![](https://i.imgur.com/67S9OPA.png)

## 更改关卡蓝图

![](https://i.imgur.com/xFobY1O.png)

英文版如下：

![](https://i.imgur.com/QSlTwTa.png)


# SceneCapture2D的工作流程

![](https://i.imgur.com/DX5Kx2M.jpg)