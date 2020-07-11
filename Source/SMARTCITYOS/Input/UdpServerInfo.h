#pragma once

enum class EOperationType:unsigned char
{
    //飞行模式
    FLY_MODE_START,
    //定义开始    {type:"FLY_MODE_START",coordinateX:960.0,coordinateY:540.0}
    FLY_MODE_DRAG,
    //拖拽         {type:"FLY_MODE_DRAG",coordinateX:960.0,coordinateY:540.0}
    FLY_MODE_SCALE,
    //缩放        {type:"FLY_MODE_SCALE",coordinateX:960.0,coordinateY:540.0,scale:1.01}      scale表示当前与上一次的放大缩小的比率
    FLY_MODE_ROTATE,
    //旋转       {type:"FLY_MODE_ROTATE",coordinateX:960.0,coordinateY:540.0,rotate:30.0}    rotate表示当前与上一次的旋转的角度（角度制）


    //地面模式
    GROUND_MODE_MOVE,
    //移动    {type:"GROUND_MODE_MOVE",VeticalAxis:0.6,HorizontalAxis:0.8}        //axis为-1~1之间的值，操纵杆为单位圆，返回x值和y值即可 在按住不放或者是改变位置时发送数据

    GROUND_MODE_JUMP,
    //跳跃            {type:"GROUND_MODE_JUMP"}
    GROUND_MODE_START_SPRINT,
    //开始加速          {type:"GROUND_MODE_START_SPRINT"}
    GROUND_MODE_STOP_SPRINT,
    //结束加速          {type:"GROUND_MODE_STOP_SPRINT"}

    GROUND_MODE_CAMERA_MOVE //视角上下左右摇晃    {type:"GROUND_MODE_CAMERA_MOVE",distanceX:950, distanceY:264}
};

namespace EOperationPort
{
    const size_t FLY_MODE_START_PORT = 9096;
    const size_t FLY_MODE_DRAG = 9098;
    //拖拽
    const size_t FLY_MODE_SCALE = 9097;
    const size_t FLY_MODE_ROTATE = 9096;
    const size_t SEND_BACK_PORT = 9077;
}

namespace PhonePCScreenSize
{
    const FVector2D PhoneScreenSize = FVector2D(2244.0f, 1080.f);
    const FVector2D PCScreenSize = FVector2D(1920.0f, 980.f);
}

namespace SendBackMessage
{
    const FString MESSAGE_CHANGE_TO_GROUND_MODE = TEXT("{type=\"CHANGE_TO_GROUND_MODE\"}");
}
