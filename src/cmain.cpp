// Created by MarufN on 19/12/2021.
//
// C++ 2017

#include <bits/stdc++.h>
#include <opencv2/highgui.hpp>
using namespace std;
using namespace cv;

vector<Mat> FrontROI(const Mat& img) {
	uint16_t imgRows = img.rows, imgCols = img.cols;
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX = centerX-round(0.05*centerX), currentY = centerY-round(0.10*centerY);
	uint8_t roiSize = 20;

	Mat outputImg = img.clone(), outputROIImg;
	outputROIImg = img.colRange(currentX - roiSize, currentX + roiSize).
			rowRange(currentY - roiSize, currentY + roiSize).clone();
	for (uint_fast32_t j = currentY - roiSize; j < currentY + roiSize; ++j) {
		for (uint_fast32_t k = currentX - roiSize; k < currentX + roiSize; ++k) {
			for (uint_fast8_t l = 0; l < 3; ++l) {
				outputImg.ptr(j, k)[l] = 0;
			}
		}
	}
	return {outputROIImg, outputImg};
}

vector<Mat> BackROI(const Mat& img) {
	uint16_t imgRows = img.rows, imgCols = img.cols;
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX = centerX-round(0.25*centerX), currentY = centerY+round(0.20*centerY);
	uint8_t roiSize = 20;

	Mat outputImg = img.clone(), outputROIImg;
	outputROIImg = img.colRange(currentX - roiSize, currentX + roiSize).
			rowRange(currentY - roiSize, currentY + roiSize).clone();
	for (uint_fast32_t j = currentY - roiSize; j < currentY + roiSize; ++j) {
		for (uint_fast32_t k = currentX - roiSize; k < currentX + roiSize; ++k) {
			for (uint_fast8_t l = 0; l < 3; ++l) {
				outputImg.ptr(j, k)[l] = 0;
			}
		}
	}
	return {outputROIImg, outputImg};
}

int main() {
	// Untuk tampak depan
	string frontImagePath = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\frontside\Maroon\B-2403-FVF.png)";
	Mat frontImg = imread(frontImagePath);
	vector<Mat> frontOutputImg = FrontROI(frontImg);

	imshow("Front ROI Image", frontOutputImg[0]);
	imshow("Front Impact Image", frontOutputImg[1]);

	// Untuk tampak belakang
	string backImagePath = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\backside\Kuning-Back\AB_1076_UR.png)";
	Mat backImg = imread(backImagePath);
	vector<Mat> backOutputImg = BackROI(backImg);

	imshow("Back ROI Image", backOutputImg[0]);
	imshow("Back Impact Image", backOutputImg[1]);

	waitKey(0);

	return 0;
}