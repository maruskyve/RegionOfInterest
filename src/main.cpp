////
//// Created by MarufN on 19/12/2021.
////
//// C++ 2017

#include <bits/stdc++.h>
//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <fileapi.h>
using namespace std;
using namespace cv;

class Util {
public:
	static vector<string> DirContents(const string& pathString) {
		const char *path = pathString.c_str();
		DIR* dir;
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

	static bool IsDir(const string& pathString) {
		DWORD attrib = GetFileAttributes(reinterpret_cast<LPCSTR>(pathString.c_str()));
		return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	static string MakeDir(const string& pathString) {
		const char *path = pathString.c_str();
		if (mkdir(path) == -1) {}
		return pathString;
	}

}; Util util;

class Ops {
public:
	static Mat Grayscale(Mat& inputImg) {  // Grayscaling menggunakan teknik rgb to yiq
		Mat outputImg = inputImg.clone();

		for (uint_fast32_t i = 0; i < outputImg.rows; ++i) {
			for (uint_fast32_t j = 0; j < outputImg.cols; ++j) {
				vector<vector<float_t>> yiq{{0.299, 0.587, 0.114},
				                            {0.596, -0.274, -0.322},
				                            {0.211, -0.523, 0.312}};
				uint8_t *imgPix = outputImg.ptr(i, j);
				uint8_t intensity = 0;

				for (uint_fast8_t k = 0; k < 3; ++k) {
					intensity += imgPix[k] * yiq[0][k];
				}
				for (uint_fast8_t l = 0; l < 3; ++l) {
					imgPix[l] = intensity;
				}
			}
		}
		return outputImg;
	}

	static Mat Brightness(Mat& inputImg, int16_t bVal) {  // Penyesuaian kecerahan citra (+/-)
		Mat outputImg = inputImg.clone();

		for (uint_fast32_t i = 0; i < outputImg.rows; ++i) {
			for (uint_fast32_t j = 0; j < outputImg.cols; ++j) {
				uint8_t *imgPix = outputImg.ptr(i, j);
				for (int_fast8_t k = 0; k < 3; ++k) {
					if (bVal >= 0) {
						imgPix[k] = (imgPix[k] + bVal) >= 255? 255: imgPix[k] + bVal;
					} else {
						imgPix[k] = (imgPix[k] - bVal) <= 0? 0: imgPix[k] - bVal;
					}
				}
			}
		}
		return outputImg;
	}

	static Mat Thresholding(Mat& inputImg, uint8_t tVal) {  // Binerisasi/thresholding untuk mengetahui warna terang dan gelap
		Mat outputImg = inputImg.clone();

		for (uint_fast32_t i = 0; i < outputImg.rows; ++i) {
			for (uint_fast32_t j = 0; j < outputImg.cols; ++j) {
				uint8_t *imgPix = outputImg.ptr(i, j);

				for (uint_fast8_t k = 0; k < 3; ++k) {
					imgPix[k] = imgPix[k] > tVal ? 255 : 0;
				}
			}
		}
		return outputImg;
	}
}; Ops ops;

vector<Mat> ComplexROIDetection(Mat& img) {
	uint16_t imgRows = img.rows, imgCols = img.cols;
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX = centerX, currentY = centerY;
	uint8_t roiSize = 40;
	static bool centerStat = false;

	Mat outputImg = img.clone(), outputROIImg;
	for (uint_fast8_t i = 0; i < 2; ++i) {
		while (true) {
			outputROIImg = img.colRange(currentX - roiSize, currentX + roiSize).
					rowRange(currentY - roiSize, currentY + roiSize).clone();

			outputROIImg = ops.Brightness(outputROIImg, +50);
			outputROIImg = ops.Grayscale(outputROIImg);
			outputROIImg = ops.Thresholding(outputROIImg, 150);

			float_t minVal = 0, maxVal = 0;

			for (uint_fast32_t j = 0; j < outputROIImg.rows; ++j) {
				for (uint_fast32_t k = 0; k < outputROIImg.cols; ++k) {
					uint8_t *imgPix = outputROIImg.ptr(j, k);

					for (uint_fast8_t l = 0; l < 3; ++l) {
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
				for (uint_fast32_t j = currentY - roiSize; j < currentY + roiSize; ++j) {
					for (uint_fast32_t k = currentX - roiSize; k < currentX + roiSize; ++k) {
						for (uint_fast8_t l = 0; l < 3; ++l) {
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
					if (!centerStat) {
						currentY = centerY;
						centerStat = true;
					}
					currentY += 1;
				}
			}
		}
	}
	return {outputROIImg, outputImg};
}


vector<Mat> FrontROIDetection(Mat& img) {
	uint16_t imgRows = img.rows, imgCols = img.cols;  // Menyimpan data panjang baris dan kolom citra
	// Menyimpan data index tengah pada masing-masing baris dan kolom
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

vector<Mat> ROIDetection(Mat& img) {
	uint16_t imgRows = img.rows, imgCols = img.cols;  // Menyimpan data panjang baris dan kolom citra
	// Menyimpan data index tengah pada masing-masing baris dan kolom
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

void MultiFiles() {
	string mainDir = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset)";
	vector<string> sides = Util::DirContents(mainDir);
	string mainOutputDir = R"(E:\output)";

	for (uint_fast32_t i = 0; i < sides.size(); ++i) {
//		cout << sides[i] << endl;
		string side = Util::MakeDir(mainOutputDir+"\\"+sides[i]);
		vector<string> sideColors = Util::DirContents(mainDir+"\\"+sides[i]);

		for (uint_fast32_t j = 0; j < sideColors.size(); ++j) {
			string sideColor = Util::MakeDir(side+"\\"+sideColors[j]);
//			cout << "\t" << sideColors[j] << endl;
			vector<string> images = Util::DirContents(
					mainDir+"\\"+sides[i]+"\\"+sideColors[j]);

			for (uint_fast32_t k = 0; k < images.size(); ++k) {
//				cout << "\t\t" << images[k] << endl;
				string imagePath = mainDir+"\\"+sides[i]+"\\"+sideColors[j]+"\\"+images[k];
				try {
					Mat img = imread(imagePath);
					vector<Mat> outputImg = ROIDetection(img);  // seq(ROI, outputImg)
//					cout << "\t" << outputImg.size() << endl;
					imwrite(sideColor+"\\ROI-"+images[k], outputImg[0]);
					imwrite(sideColor+"\\Impact-"+images[k], outputImg[1]);
				} catch (Exception e) {
					continue;
				}
			}
		}
//		cout << sides[i] << endl;
	}

	cout << (Util::IsDir((string) mainDir) == 0? "ITS A FILE": "ITS A DIR") << endl;
}

int main() {
	MultiFiles();

	// !!! Execution time counting begin
	auto start = chrono::high_resolution_clock::now();
	ios_base::sync_with_stdio(false);

	string file = "B_2601_UFL.png";
	string fpath = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\frontside\Kuning\)"+file;
	Mat img = imread(fpath);

	vector<Mat> outputImg = ComplexROIDetection(img);  // seq(ROI, outputImg)

	// !!! Execution time counting end
	auto end = chrono::high_resolution_clock::now();
	double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	time_taken *= 1e-9;
	cout << "\nMain execution time: " << fixed
	     << time_taken << setprecision(9) << " seconds";

//	imwrite("E:/Z_ROI.png", outputImg[0]);
//	imwrite("E:/Z_OutputImg.png", outputImg[1]);
//	imshow("Output Image", outputImg[0]);
//	imshow("ROI Image", outputImg[1]);
//	waitKey(0);

	return 0;
}
