//
// Created by MarufN on 19/12/2021.
//
// C++ 2017

#include <bits/stdc++.h>
//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dirent.h>
using namespace std;
using namespace cv;

class Util {
public:
	static vector<string> DirFiles(const char* directory) {
		const char *path = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\frontside\Merah)";
		DIR *dir;
		struct dirent *diread;
		vector<string> files;
		if ((dir = opendir(path)) != nullptr) {
			while ((diread = readdir(dir)) != nullptr) {
				files.emplace_back(diread -> d_name);
			}
			closedir(dir);
		} else {
			perror("opendir");
			EXIT_FAILURE;
		}
		return files;
	}


};
Util Util;


Mat Grayscale(Mat& inputImg) {  // Grayscaling menggunakan teknik rgb to yiq
	Mat outputImg = inputImg.clone();

	for (int i = 0; i < outputImg.rows; ++i) {
		for (int j = 0; j < outputImg.cols; ++j) {
			vector<vector<float_t>> yiq{{0.299, 0.587, 0.114},
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

Mat Brightness(Mat& inputImg, int16_t bVal) {
	Mat outputImg = inputImg.clone();

	for (int i = 0; i < outputImg.rows; ++i) {
		for (int j = 0; j < outputImg.cols; ++j) {
			uint8_t *imgPix = outputImg.ptr(i, j);
			for (int k = 0; k < 3; ++k) {
				if (bVal >= 0) {
					imgPix[k] = (imgPix[k] + bVal) >= 255? 255: imgPix[k] + bVal;
				} else {
					imgPix[k] -= bVal;
				}
			}
		}
	}
	return outputImg;
}

Mat Thresholding(Mat& inputImg, uint8_t tVal) {  // Operasi thresholding untuk mengetahui warna terang dan gelap
	Mat outputImg = inputImg.clone();

	for (int i = 0; i < outputImg.rows; ++i) {
		for (int j = 0; j < outputImg.cols; ++j) {
			uint8_t *imgPix = outputImg.ptr(i, j);

			for (int k = 0; k < 3; ++k) {
				imgPix[k] = imgPix[k] > tVal? 255: 0;
			}
		}
	}
	return outputImg;
}

pair<Mat, Mat> ROIDetection(const string& filepath) {
	// Inisialisasi lokasi file citra pada sistem
	string filename = "B_1061_JFB";

	Mat img = imread(filepath);  // Menyimpan data citra berupa array multidimensi
	uint16_t imgRows = img.rows, imgCols = img.cols;  // Menyimpan data panjang baris dan kolom citra
	// Menyimpan data index tengah pada masing-masing baris dan kolom
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX = centerX, currentY = centerY;
	uint8_t roiSize = 40;

	Mat outputImg = img.clone(), outputROIImg;
	for (int i = 0; i < 2; ++i) {
		while (true) {
			outputROIImg = img.colRange(currentX - roiSize, currentX + roiSize).
					rowRange(currentY - roiSize, currentY + roiSize).clone();

			outputROIImg = Brightness(outputROIImg, +50);
			outputROIImg = Grayscale(outputROIImg);
			outputROIImg = Thresholding(outputROIImg, 150);

			float_t minVal = 0, maxVal = 0;

			for (int j = 0; j < outputROIImg.rows; ++j) {
				for (int k = 0; k < outputROIImg.cols; ++k) {
					uint8_t *imgPix = outputROIImg.ptr(j, k);

					for (int l = 0; l < 3; ++l) {
						minVal += imgPix[l] == 0 ? 1 : 0;
						maxVal += imgPix[l] == 255 ? 1 : 0;
					}
				}
			}

			float_t val = max(minVal, maxVal);
			float_t refRatio = val / (minVal + maxVal);  // White percentage || for white color majority
			cout << minVal << " - " << maxVal << endl;
			cout << refRatio << endl;

			if (refRatio >= 0.95) {
				imshow("", outputROIImg);
				cout << "PERFECT IMAGE with," << endl;
				cout << "\t-whitePercentage: " << refRatio << endl;
				cout << "\t-center(X,Y): (" << currentX << ", " << currentY << ")" << endl;
				cout << "\t-imgSize(Cols,Rows): (" << imgCols << "," << imgRows << ")" << endl;
				outputROIImg = img.colRange(currentX - roiSize, currentX + roiSize).
						rowRange(currentY - roiSize, currentY + roiSize);
				for (int j = currentY - roiSize; j < currentY + roiSize; ++j) {
					for (int k = currentX - roiSize; k < currentX + roiSize; ++k) {
						for (int l = 0; l < 3; ++l) {
							outputImg.ptr(j, k)[l] = 0;
						}
					}
				}
				break;
			} else {
				cout << "NOT PERFECT IMAGE" << endl;
				cout << currentY << " from " << centerY << endl << endl;
				if (i == 0) {
					currentY -= 1;
				} else {
					currentY += 1;
				}
			}
		}
	}

	imwrite("E:/Z_ROI.png", outputROIImg);
	imwrite("E:/Z_OutputImg.png", outputImg);
	imshow("Output Image", outputImg);
	imshow("ROI Image", outputROIImg);
	waitKey(0);
	return make_pair(outputImg, outputROIImg);
}

void WriteCsv() {
	ofstream myCsv;
	myCsv.open("../build/data.csv");
	for (int i = 0; i < 4; ++i) {
		myCsv << i << endl;
		cout << i <<endl;
	}
	myCsv.close();
}
void dirFiles() {
	vector<string> files = Util::DirFiles("");
//	for (auto& file: files) {
//		cout << file << endl;
//	}

}

int main() {
	dirFiles();
	string file = "B_2601_UFL.png";
	string fpath = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\frontside\Kuning\)"+file;
	cout << fpath;
	ROIDetection(fpath);
	return 0;
}
