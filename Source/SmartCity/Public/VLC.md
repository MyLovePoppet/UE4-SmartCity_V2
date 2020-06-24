# VLC插件获取RTSP视频流的每一帧图像
小组成员：武大卫，舒钦瑜
## VLC插件的工作原理：![](https://i.niupic.com/images/2020/06/24/8j6k.png)
## 针对VLC插件获取RTSP流的每一帧图像

VLC插件与UE4本身支持的将视频流转化为纹理的格式如下：

	//IMediaTexureSample.h：21行
	enum class EMediaTextureSampleFormat
	{
		/** Format not defined. */
		Undefined,
	
		/** Four 8-bit unsigned integer components (AYUV packing) per texel. */
		CharAYUV,
	
		/** Four 8-bit unsigned integer components (Blue, Green, Red, Alpha) per texel. */
		CharBGRA,
	
		/** Four 10-bit unsigned integer components (Blue, Green, Red) & 2-bit alpha per texel. */
		CharBGR10A2,
	
		/**  Windows bitmap (like CharBGRA, but flipped vertically). */
		CharBMP,
	
		/** NV12 encoded monochrome texture with 8 bits per channel. */
		CharNV12,
	
		/** NV21 encoded monochrome texture with 8 bits per channel. */
		CharNV21,
	
		/** Four 8-bit unsigned integer components (UYVY packing aka. HDYC, IUYV, UYNV, Y422) per texel. */
		CharUYVY,
	
		/** Four 8-bit unsigned integer components (YUY2 packing aka. YUNV, YUYV) per texel. */
		CharYUY2,
	
		/** Four 8-bit unsigned integer components (YVYU packing) per texel. */
		CharYVYU,
	
		/** Three 16-bit floating point components (Red, Green, Blue) per texel. */
		FloatRGB,
	
		/** Four 16-bit floating point components (Red, Green, Blue, Alpha) per texel. */
		FloatRGBA,
	
		/** YUV v210 format which pack 6 pixel using 12 x 10bits components (128 bits block). */
		YUVv210,
	
		/** 4:4:4:4 AY'CbCr 16-bit little endian full range alpha, video range Y'CbCr. */
		Y416,
	};
我们选择进行测试的RTSP流的链接为[rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov](rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov)，经过验证，该视频流的格式为`CharYUY2`,我们目标是在VLC视频流获取到每一帧图像时，要将其转化为RGB图像。

在经过对VLC插件的源码进行分析后，我们发现在其`VlcMediaCallbacks.h`头文件内对类`FVlcMediaCallbacks`的函数`void StaticVideoDisplayCallback(void* Opaque, void* Picture)`声明为将获取到的每一帧图像传入到UE4内部进行处理（具体获取该帧图像的代码被封装在了该插件的.dll文件内，无法进行查看）。
该函数实现如下：

	//VlcMediaCallbacks.cpp:389行
	void FVlcMediaCallbacks::StaticVideoDisplayCallback(void* Opaque, void* Picture)
	{
		auto Callbacks = (FVlcMediaCallbacks*)Opaque;
		auto VideoSample = (FVlcMediaTextureSample*)Picture;//转化为了实现了UE4能够进行处理的纹理图像接口IMediaTextureSample的对象
	
		if ((Callbacks == nullptr) || (VideoSample == nullptr))
		{
			return;
		}
	
		UE_LOG(LogVlcMedia, VeryVerbose, TEXT("Callbacks %llx: StaticVideoDisplayCallback (CurrentTime = %s, Queue = %i)"),
		       Opaque, *Callbacks->CurrentTime.ToString(),
		       Callbacks->Samples->NumVideoSamples()
		);
	
		VideoSample->SetTime(Callbacks->CurrentTime);
	
		// add sample to queue
		//加入到UE4的内部的视频播放队列中进行播放
		Callbacks->Samples->AddVideo(Callbacks->VideoSamplePool->ToShared(VideoSample));
	}


所以对于该函数内的VideoSample对象即为封装了RTSP视频流的每一帧图像的数据，可以从其提供的方法获取该图像的大小以及像素点存储的起始位置。所以我们在该函数结束之前将该原本的视频流的一帧图像的数据保存到本地。

	TArray<uint8> SrcArrays;
	SrcArrays.Append((uint8*)VideoSample->GetBuffer(), VideoSample->GetBufferSize());
	//写入到文件
	FFileHelper::SaveArrayToFile(SrcArrays,TEXT("C://img.dat"));

此时文件存储的信息只有像素点的数据，没有文件头信息，所以其无法直接打开，我们可以使用一个专门查看这种没有格式，只有像素点数据的文件格式的网站[http://rawpixels.net/](http://rawpixels.net/)，我们上传之前保存的数据到该网站后，选择合适的格式后，该网站就能将该没有格式的文件解析成一张图像，如下图所示：
![](https://i.niupic.com/images/2020/06/24/8j6o.jpg)
由于我们的视频流的格式为YUY2格式，图像宽为120，高为160，按照上图的红框内的调整后，我们可以看到我们的没有格式的图像也能被解析出来，解析的图像的结果在正中间。接下来我们编写一个函数用于转换这种YUY2图像格式到我们所熟悉的RGB格式。
	
	YUY2格式是YUV 4:2:2的一种，其中YUV 4:2:2的格式介绍如下：
	每个色差信道的抽样率是亮度信道的一半。对非压缩的8比特量化的图像来说，每个由两个水平方向相邻的像素组成的宏像素需要占用4字节内存。
	下面的四个像素为：[Y0 U0 V0] [Y1 U1 V1] [Y2 U2 V2] [Y3 U3 V3]
	存放的码流为：Y0 U0 Y1 V1 Y2 U2 Y3 V3
	映射出像素点为：[Y0 U0 V1] [Y1 U0 V1] [Y2 U2 V3] [Y3 U2 V3]

	YUY2（和YUYV）格式为每个像素保留Y分量，而UV分量在水平方向上每两个像素采样一次。一个宏像素为4个字节，实际表示2个像素。
	（4:2:2的意思为一个宏像素中有4个Y分量、2个U分量和2个V分量。）图像数据中YUV分量排列顺序如下：
	Y0 U0 Y1 V0    Y2 U2 Y3 V2 …

而根据Microsoft官方文档[http://msdn2.microsoft.com/en-us/library/ms893078.aspx](http://msdn2.microsoft.com/en-us/library/ms893078.aspx)我们将一个YUV宏像素转化到RGB的像素所用的公式如下：
	
	C = Y - 16
	D = U - 128
	E = V - 128
	
	R = clip(( 298 * C           + 409 * E + 128) >> 8)
	G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
	B = clip(( 298 * C + 516 * D           + 128) >> 8)

而我们的YUY2格式的一个宏像素又代表两个RGB像素点，所以代码实现转换YUY2格式到RGB格式的数据如下：

	/**
	 *YUY2图像数据流到RGB图像数据流的转换，由于YUY2格式到RGB的特点，RGB图像的宽会变成原来的两倍
	 *@param SrcBuf 需要进行转换的YUY2格式的数据
	 *@param Dim 原图像数据的大小
	 *@return 转化之后的RGB数据流，顺序存放
	 */
	TArray<uint8> FVlcMediaCallbacks::YUY2ToRGB(TArray<uint8>& SrcBuf, FIntPoint Dim)
	{
		uint8* ptrIn = SrcBuf.GetData();
		uint8 ptrOut[6];
		TArray<uint8> DstBuff;
		for (int32 i = 0; i < Dim.X * Dim.Y; ++i)
		{
			int32 y0 = ptrIn[0];
			int32 u0 = ptrIn[1];
			int32 y1 = ptrIn[2];
			int32 v0 = ptrIn[3];
			ptrIn += 4;//YUY2步长为4
			int32 c = y0 - 16;
			int32 d = u0 - 128;
			int32 e = v0 - 128;
			ptrOut[0] = FMath::Clamp((298 * c + 516 * d + 128) >> 8,0,255); // blue
			ptrOut[1] = FMath::Clamp((298 * c - 100 * d - 208 * e + 128) >> 8,0,255); // green
			ptrOut[2] = FMath::Clamp((298 * c + 409 * e + 128) >> 8,0,255); // red
			c = y1 - 16;
			ptrOut[3] = FMath::Clamp((298 * c + 516 * d + 128) >> 8,0,255); // blue
			ptrOut[4] = FMath::Clamp((298 * c - 100 * d - 208 * e + 128) >> 8,0,255); // green
			ptrOut[5] = FMath::Clamp((298 * c + 409 * e + 128) >> 8,0,255); // red
			DstBuff.Append(ptrOut, 6);//加入到RGB保存的结果内
		}
		return DstBuff;
	}


同样的，我们使用和之前没有转换格式的保存数据的方法`FFileHelper::SaveArrayToFile()`进行像素点保存到文件，我们也使用之前的网站进行解析我们的RGB图像数据。如下图所示![](https://i.niupic.com/images/2020/06/24/8j6R.jpg)
红框内是需要设置的图像数据，需要注意的是由于YUY2格式到RGB的特点，RGB图像的宽会变成原来的两倍，然后将图像格式设置为RGB24即可看到将YUY2格式数据转化RGB格式的数据
