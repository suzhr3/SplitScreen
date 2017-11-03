#include "stdafx.h"
#include "Pic.h"
#include "CvvImage.h"

using namespace std;
using namespace cv;

Pic::Pic()
{
	currentFrame = 0;

	isOpen = false;

	video_play = false;
}

//含参构造函数
Pic::Pic(VideoCapture capture, String videoPath, long totalFrameNumber, long currentFrame, Mat frame, bool isOpen, bool video_play)
{
	this->capture = capture;

	this->videoPath = videoPath;

	this->totalFrameNumber = totalFrameNumber;

	this->currentFrame = currentFrame;

	this->frame = frame;
	
	//用于判断当前屏幕是否打开了
	this->isOpen = isOpen;
	
	//用于判断当前屏幕是否有视频正在播放
	this->video_play = video_play;
}
