#include <iostream>
#include <bits/stdc++.h>
//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat Grayscale(Mat& inputImg) {  // Grayscaling menggunakan teknik rgb to yiq
	Mat outputImg = inputImg.clone();

	for (int i = 0; i < outputImg.rows; ++i) {
		for (int j = 0; j < outputImg.cols; ++j) {
			vector<vector<float>> yiq{{0.299, 0.587, 0.114},
							          {0.596, -0.274, -0.322},
							          {0.211, -0.523, 0.312}};
			uint8_t *imgPix = outputImg.ptr(i, j);
			uint8_t intensity = 0;

			for (int k = 0; k < 3; ++k) {
				intensity += imgPix[k] * yiq[0][k];
			}
			for (int l = 0; l < 3; ++l) {
				imgPix[l] = intensity;
			}
		}
	}
	return outputImg;
}

Mat Thresholding(Mat& inputImg, uint8_t tVal) {
	Mat outputImg = inputImg.clone();

	for (int i = 0; i < outputImg.rows; ++i) {
		for (int j = 0; j < outputImg.cols; ++j) {
			uint_fast8_t *imgPix = outputImg.ptr(i, j);

			for (int k = 0; k < 3; ++k) {
				imgPix[k] = imgPix[k] > tVal? 255: 0;
			}
		}
	}
	return outputImg;
}

void ROIDetection() {
	// stdint ref: https://en.wikibooks.org/wiki/C_Programming/stdint.h
	// Inisialisasi lokasi file citra pada sistem
	string fp = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\frontside\Kuning\B_1061_JFB.png)";

	Mat img = imread(fp);  // Menyimpan data citra berupa array multidimensi
	uint16_t imgRows = img.rows, imgCols = img.cols;  // Menyimpan data panjang baris dan kolom citra
	// Menyimpan data index tengah pada masing-masing baris dan kolom
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX, currentY;
	uint8_t roiSize = 30;

	Mat outputImg;
	while (true) {
		outputImg = img.colRange(centerX-roiSize, centerX+roiSize).
				rowRange(centerY-roiSize, centerY+roiSize);

		outputImg = Grayscale(outputImg);
		outputImg = Thresholding(outputImg, 150);

		float_t minVal, maxVal = 0;

		for (int i = 0; i < outputImg.rows; ++i) {
			for (int j = 0; j < outputImg.cols; ++j) {
				uint8_t *imgPix = outputImg.ptr(i, j);

				for (int k = 0; k < 3; ++k) {
					minVal += imgPix[k] == 0? 1: 0;
					maxVal += imgPix[k] == 255? 1: 0;
				}
			}
		}

		float_t total = minVal+maxVal;
		float_t whiteRatio = maxVal/total;  // White percentage || for white color majority
		cout << whiteRatio << endl;
		if (whiteRatio >= 0.94) {
			cout << "PERFECT IMAGE with," << endl;
			cout << "\t-whitePercentage: " << whiteRatio << endl;
			cout << "\t-center(X,Y): (" << centerX << ", " << centerY << ")" << endl;
			cout << "\t-imgSize(Cols,Rows): (" << imgCols << "," << imgRows << ")" << endl;
			outputImg = img.colRange(centerX-roiSize, centerX+roiSize).
					rowRange(centerY-roiSize, centerY+roiSize);
			break;
		} else {
			cout << "NOT PERFECT IMAGE" << endl;
			cout << centerY << endl;
			centerY -=1;
		}
	}

//	cout << centerX-roiSize << "  " << centerX << "   " << centerX+roiSize << endl;
//	cout << centerY-roiSize << "  " << centerY << "   " << centerY+roiSize << endl;

	imwrite("E:/ROI.png", outputImg);
	imshow("IMAGE", outputImg);
	waitKey(0);
}

int main() {
	ROIDetection();
	return 0;
}
