#pragma once
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class Pic
{
public:
	Pic();
	Pic(VideoCapture capture, CString videoPath, long totalFrameNumber, long currentFrame, Mat frame, bool isOpen, bool isOpenAndDBClick, bool video_play);

public:
	VideoCapture capture;
	CString videoPath;
	Mat frame;
	long totalFrameNumber;
	long currentFrame;
	bool isOpen;
	bool isOpenAndDBClick;
	bool video_play;
};
