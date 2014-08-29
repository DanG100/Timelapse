#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>


using namespace cv;
using namespace std;

void getImage();
void saveImage();
int compareImage();
void updateImages();
void editLog();

#define THRESHOLD 15

Mat newImage,  HSV_oldImage, HSV_procImage, oldImage;

int runCounter = 0;
int compareStatus, compareValue;

VideoCapture cap;

int main() {
	//SETUP THE IMAGE AND CAMERA
	char inputKey = waitKey(1);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	while(1){

		cap.open(0);
		cap.grab();
		getImage();
		updateImages();
		if (compareImage()){
			saveImage();
		}
		inputKey = waitKey(1);

		editLog();
		runCounter++;
		msleep(30000);
	}
	return (1);
}
void getImage(){
	cap >> newImage;
}
int compareImage(){
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };
	float h_ranges[] = { 0, 256 };
	float s_ranges[] = { 0, 180 };
	const float* ranges[] = { h_ranges, s_ranges };
	int channels[] = { 0};

	MatND oldImage, procImage;

	calcHist( &HSV_oldImage, 1, channels, Mat(), oldImage, 2, histSize, ranges, true, false );
	normalize( oldImage, oldImage, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &HSV_procImage, 1, channels, Mat(), procImage, 2, histSize, ranges, true, false );
	normalize( procImage, procImage, 0, 1, NORM_MINMAX, -1, Mat() );

	double oldImage_procImageCorrel = compareHist( oldImage, procImage, CV_COMP_CORREL);
	double oldImage_procImageChisqr = compareHist( oldImage, procImage, CV_COMP_CHISQR);
	double oldImage_procImageInter = compareHist( oldImage, procImage, CV_COMP_INTERSECT);
	double oldImage_procImageHelli = compareHist( oldImage, procImage, CV_COMP_HELLINGER);
	int compareValue = (oldImage_procImageCorrel + oldImage_procImageChisqr + oldImage_procImageInter + oldImage_procImageHelli)/4;
	if( compareValue < THRESHOLD){
		return 1;
	}
	else{
		return 0;
	}

}
void saveImage(){
	 imwrite("/timelapse/image%d",runCounter, newImage);
	 cvtColor(newImage, HSV_oldImage, CV_BGR2HSV);
}
void updateImages(){
	if(runCounter!=0){
		cvtColor(newImage, HSV_procImage, CV_BGR2HSV );
		cvtColor(newImage, HSV_oldImage, CV_BGR2HSV );
	}
	else{
		cvtColor(newImage, HSV_procImage, CV_BGR2HSV );
	}
}
void editLog(){
	ofstream myfile;
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	myfile.open("/timelapse/log.csv", ios::app | ios::out | ios::binary);
	myfile << "image%d,",runCounter;
	myfile << asctime(timeinfo);
	myfile << ",Compare: %d \n",compareValue;
	myfile.close();
}

