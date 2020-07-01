# 获取三维世界的某个点在屏幕上的位置
使用`ProjectWorldLocationToScreen()`函数，该函数声明如下：
```C++
//PlayerController.h/673行
/**
 * Convert a World Space 3D position into a 2D Screen Space position.
 * @return true if the world coordinate was successfully projected to the screen.
 */
bool ProjectWorldLocationToScreen(FVector WorldLocation, FVector2D& ScreenLocation, bool bPlayerViewportRelative = false) const;
```
该函数接收两个参数，第一个是三维世界的坐标，第二个是返回到的屏幕上的位置，在计算完结果时将数据保存到该FVector2D内；该函数的返回值是给定的位置是否成功投影到屏幕上。
需要注意的是即使是屏幕内无法看到三维坐标的位置，其也有可能正确计算出来结果，但是计算的结果点不在ViewPort内部，如果需要在ViewPort内部，除了需要判断函数的返回值之外，还需要判断返回的点是否在ViewPort内部。

# RGB颜色数组转为FRHITexture2D
参考[https://zhuanlan.zhihu.com/p/45171840](https://zhuanlan.zhihu.com/p/45171840)的写法。
```C++
//可能存在的问题：CreateTexture2D的Flag参数有可能还有其他的必须的Flag；
FTexture2DRHIRef RGBToRHITexture(TArray<FColor> ColorArray, FVector2D OutputDim)
{
    FTexture2DRHIRef OutputTexture;
    ENQUEUE_RENDER_COMMAND(CaptureCommand)(
        [ColorArray,OutputDim,&OutputTexture](FRHICommandListImmediate& RHICmdList)
        {
            FRHIResourceCreateInfo CreateInfo;
            //Flags位于ETextureCreateFlags内，暂时只填了两个Flag，后续添加可以使用|运算符
            OutputTexture = RHICreateTexture2D(OutputDim.X, OutputDim.Y,EPixelFormat::PF_R8G8B8A8, 1, 1,
                                               ETextureCreateFlags::TexCreate_ShaderResource | ETextureCreateFlags::TexCreate_CPUWritable, CreateInfo);
            uint32 LolStride = 0;
            char* TextureDataPtr = (char*)RHICmdList.LockTexture2D(
                OutputTexture, 0, EResourceLockMode::RLM_WriteOnly, LolStride, false);
            for (uint32 Row = 0; Row < OutputDim.Y; ++Row)
            {
                uint32* PixelPtr = (uint32*)TextureDataPtr;
                for (uint32 Col = 0; Col < OutputDim.X; ++Col)
                {
                    //RGB数据写入
                    FColor Color = ColorArray[Row * OutputDim.X + Col];
                    uint8 r = Color.R;
                    uint8 g = Color.G;
                    uint8 b = Color.B;
                    uint8 a = 255;
                    *PixelPtr = r | (g << 8) | (b << 16) | (a << 24);
                    PixelPtr++;
                }
                //下一行
                TextureDataPtr += LolStride;
            }
            RHICmdList.UnlockTexture2D(OutputTexture,0,false);
        }
    );
}
```

# RGB颜色数组转成UTexture2D
```C++
/**
 * 将一个颜色数组转化为Texture2D
 * @param ColorArray 输入的FColor数组
 * @param OutputDim 输入的图像的大小
 * @return 返回的生成的UTexture2D的智能指针
 */
TSharedPtr<UTexture2D> RGBToTexture(TArray<FColor> ColorArray, FVector2D OutputDim)
{
	uint32 Width = OutputDim.X;
	uint32 Height = OutputDim.Y;

	TSharedPtr<UTexture2D> TextureToBeWrite = MakeShareable(UTexture2D::CreateTransient(Width, Height));
    //以下三个设置是为了能够写入数据的权限
	TextureToBeWrite->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    TextureToBeWrite->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	TextureToBeWrite->SRGB = 0;

	FTexture2DMipMap& MipMap = TextureToBeWrite->PlatformData->Mips[0];
	void* Data = MipMap.BulkData.Lock(LOCK_READ_WRITE);

	uint32 stride = sizeof(FColor);
	FMemory::Memcpy(Data, ColorArray.GetData(), Width * Height * stride);
	MipMap.BulkData.Unlock();
	TextureToBeWrite->UpdateResource();
	return TextureToBeWrite;
}
```
