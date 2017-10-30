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

Pic::Pic(VideoCapture capture, String videoPath, long totalFrameNumber, long currentFrame, Mat frame, bool isOpen, bool video_play)
{
	this->capture = capture;
	this->videoPath = videoPath;
	this->totalFrameNumber = totalFrameNumber;
	this->currentFrame = currentFrame;
	this->frame = frame;
	this->isOpen = isOpen;
	this->video_play = video_play;
}
