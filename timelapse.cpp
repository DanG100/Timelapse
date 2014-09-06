#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace cv;
using namespace std;

void getImage();
void saveImage();
int compareImage();
void updateImages();
void editLog();

#define THRESHOLD 15
#define DEBUG printf("LINE: %d\n",__LINE__);
Mat newImage,  HSV_oldImage, HSV_procImage, oldImage;
String s;
int runCounter = 0;
float compareValue;
bool imageSaved;

VideoCapture cap;

int main() {
	//SETUP THE IMAGE AND CAMERA
	char inputKey = waitKey(1);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	cap.open(0);
	while(1){
		DEBUG
		
		DEBUG
		cap.grab();
		DEBUG
		getImage();
		DEBUG
		updateImages();
		if (compareImage()){
			saveImage();
		}
		inputKey = waitKey(1);
		editLog();
		runCounter++;
		sleep(10);

	}
	cap.release();
	return 1;
}
void getImage(){
	cap >> newImage;
}
int compareImage(){
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };
	float h_ranges[] = { 0,180};
	float s_ranges[] = { 0, 256};
	const float* ranges[] = { h_ranges, s_ranges };
	int channels[] = {0,1};

	MatND oldImage, procImage;

	calcHist( &HSV_oldImage, 1, channels, Mat(), oldImage, 2, histSize, ranges, true, false );
	normalize( oldImage, oldImage, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &HSV_procImage, 1, channels, Mat(), procImage, 2, histSize, ranges, true, false );
	normalize( procImage, procImage, 0, 1, NORM_MINMAX, -1, Mat() );
	double oldImage_procImageCorrel = compareHist( oldImage, procImage, CV_COMP_CORREL);
	cvtColor(newImage, HSV_oldImage, CV_BGR2HSV);
	compareValue = oldImage_procImageCorrel;
	if( oldImage_procImageCorrel < 0.99){
		imageSaved = true;
		printf("Diff & compare value: %f\n", oldImage_procImageCorrel);
		return 1;

	}
	else{
		printf("Same & compare value: %f\n", oldImage_procImageCorrel);
		imageSaved = false;
		return 0;

	}

}
void saveImage(){

	stringstream ss;
	ss << "images/image" << runCounter << ".png";
	s = ss.str();
   	 vector<int> compression_params;
   	 compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(1);

	imwrite(s, newImage, compression_params);

}
void updateImages(){
	if(runCounter==0){
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
	myfile.open("images/log.csv", ios::app | ios::out | ios::binary);
	myfile << "Run Counter: "<< runCounter << ",";
	myfile << "Compare: " << compareValue << ",";
	myfile << "Image is Saved: " << imageSaved << ",";
	myfile << asctime(timeinfo);
	myfile.close();
}

