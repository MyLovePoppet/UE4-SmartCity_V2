﻿#pragma once

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

    //{type:"FLY_MODE_RETURN_NORTH"}
    FLY_MODE_RETURN_NORTH,


    
    //地面模式
    GROUND_MODE_MOVE,
    //移动    {type:"GROUND_MODE_MOVE",VeticalAxis:0.6,HorizontalAxis:0.8}        //axis为-1~1之间的值，操纵杆为单位圆，返回x值和y值即可 在按住不放或者是改变位置时发送数据

    GROUND_MODE_JUMP,
    //跳跃            {type:"GROUND_MODE_JUMP"}
    GROUND_MODE_START_SPRINT,
    //暂时无用
    //开始加速          {type:"GROUND_MODE_START_SPRINT"}
    GROUND_MODE_STOP_SPRINT,
    //暂时无用
    //结束加速          {type:"GROUND_MODE_STOP_SPRINT"}

    GROUND_MODE_CAMERA_MOVE //视角上下左右摇晃    {type:"GROUND_MODE_CAMERA_MOVE",distanceX:1.50, distanceY:2.1}
};

namespace EOperationPort
{
    const size_t FLY_MODE_START_PORT = 9096;
    const size_t FLY_MODE_DRAG = 9098;
    //拖拽
    const size_t FLY_MODE_SCALE = 9097;
    const size_t FLY_MODE_ROTATE = 9096;
    const size_t GROUND_MODE_MOVE = 9095;
    const size_t GROUND_MODE_CAMERA = 9094;
    const size_t SEND_BACK_PORT = 9077;
}

namespace PhonePCScreenSize
{
    const FVector2D PhoneScreenSize = FVector2D(2560.0f, 1600.f);
    const FVector2D PCScreenSize = FVector2D(1920.0f, 980.f);
}

namespace SendBackMessage
{
    const FString MESSAGE_MODE_CHANGE_TO_GROUND = TEXT("{\"type\":\"MODE_CHANGE_TO_GROUND\"}");
    const FString MESSAGE_MODE_CHANGE_TO_FLY = TEXT("{\"type\":\"MODE_CHANGE_TO_FLY\"}");
    const FString MESSAGE_SEND_BACK_CURRENT_DEGREE = TEXT("{\"type\":\"CURRENT_DEGREE\",\"degree\":?}");
}
