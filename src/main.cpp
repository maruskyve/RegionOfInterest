////
//// Created by MarufN on 19/12/2021.
////
//// C++ 2017

#include <bits/stdc++.h>
#include <opencv2/highgui.hpp>
#include "../../CppImageProcessing/improc.h"
#include "winutil.h"
using namespace std;
using namespace cv;


class ROIUtil {
public:
	// Generate image with green Bounding Box
	static Mat BoundingBox(const Mat& inputImg, tuple<uint16_t, uint16_t> rowRange, tuple<uint16_t, uint16_t> colRange, uint8_t borderSize) {
		uint16_t fRows = get<0>(rowRange), lRows = get<1>(rowRange),
				fCols = get<0>(colRange), lCols = get<1>(colRange);
		uint16_t tBorder = fRows+borderSize, bBorder = lRows-borderSize,
				lBorder = fCols+borderSize, rBorder = lCols-borderSize;
		Mat outputImg = inputImg.clone();

		for (uint_fast16_t i = fRows-borderSize; i < lRows+borderSize; ++i) {
			for (uint_fast16_t j = fCols-borderSize; j < lCols+borderSize; ++j) {
				uint8_t *imgPix = outputImg.ptr(i, j);

				for (uint_fast8_t k = 0; k < 3; ++k) {
					if (i <= tBorder-borderSize || i >= bBorder+borderSize ||
					j <= lBorder-borderSize || j >= rBorder+borderSize) {
						imgPix[k] = k == 1? 255 : 0;  // (0, 255, 0) Green color space
					}
				}
			}
		}

		return outputImg;
	}

	// Image extractor including (ROI image & output Bounding Box image)
	static vector<Mat> GenerateROI(const Mat& inputImg, uint16_t roiSize,
	                        uint16_t currentX, uint16_t currentY) {
		uint16_t ROISIZE = roiSize/2, ROIWEIGHT = roiSize%2 != 0? 1: 0;
		// ROIWEIGHT -> ROI size weighting for odd roiSize/2 result

		Mat outputROIImg = inputImg.colRange(currentX-ROISIZE, currentX+ROISIZE+ROIWEIGHT).
				rowRange(currentY-ROISIZE, currentY + ROISIZE+ROIWEIGHT).clone();
		Mat outputBBImg = BoundingBox(inputImg, make_tuple(currentY-ROISIZE, currentY+ROISIZE+ROIWEIGHT),
				make_tuple(currentX-ROISIZE, currentX+ROISIZE+ROIWEIGHT), 2).clone();

		return {outputROIImg, outputBBImg};
	}
};

/* Minus=(<1, >0), Absolute=(1), Plus=(>1, <2)
 * All ROI method set center point is based on the length of image rows and cols ratio
 * that based on both given ratio
 *
 * Best ratio: Front(x=0.694, y=0.572), Back(x=0.31, y=0.72)
 *
 * Finally, ROI position shown as illustration below:
 * Image ROI Illustration
	> Frontside ROI
	  ______________________________
	 |                              |
	 |                              |
	 |              +               |
	 |                    X         |
	 |                              |
	 |______________________________|
	> Backside ROI
      ______________________________
	 |                              |
	 |                              |
	 |              +               |
	 |        X                     |
	 |                              |
	 |______________________________|
	 */
vector<Mat> aioROIGenerator(const Mat& inputImg, const uint16_t& roiSize,
		const tuple<float_t, float_t>& ratio) {
	uint16_t inputRows = inputImg.rows, inputCols = inputImg.cols;
	uint16_t currentX = get<0>(ratio)*float_t(inputCols),
	currentY = get<1>(ratio)*float_t(inputRows);

	return ROIUtil::GenerateROI(inputImg, roiSize, currentX, currentY);  // sequence(ROI, Bounding Box Image)
}



void MultiFiles() {
	string mainDir = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset)",
			mainOutputDir = R"(E:\output)", mainFlatOutputDir = R"(E:\output_flat)";
	vector<string> sides = WinUtil::DirContents(mainDir);
	vector<string> failFiles, currentTopFolder = {"backside", "frontside"};;
	uint16_t totalFile = 0;

	WinUtil::Remove(mainOutputDir);
	WinUtil::Remove(mainFlatOutputDir);
	WinUtil::MakeDir(mainFlatOutputDir);

	for (uint_fast32_t i = 0; i < sides.size(); ++i) {
//		cout << sides[i] << endl;
		string side = WinUtil::MakeDir(mainOutputDir+"\\"+sides[i]);
		vector<string> sideColors = WinUtil::DirContents(mainDir+"\\"+sides[i]);

		for (uint_fast32_t j = 0; j < sideColors.size(); ++j) {
			string sideColor = WinUtil::MakeDir(side+"\\"+sideColors[j]);
//			cout << "\t" << sideColors[j] << endl;
			vector<string> images = WinUtil::DirContents(
					mainDir+"\\"+sides[i]+"\\"+sideColors[j]);

			for (uint_fast32_t k = 0; k < images.size(); ++k) {
//				cout << "\t\t" << images[k] << " - ";
				string imagePath = mainDir+"\\"+sides[i]+"\\"+sideColors[j]+"\\"+images[k];
				try {
					bool criterion = sides[i] == currentTopFolder[1];  // Dir sequence marker
					if (WinUtil::IsFile(imagePath) && criterion) {
						Mat img = imread(imagePath);
						float_t bX=.694, bY=.572, fX =.46, fY=.49;
						vector<Mat> outputImg = aioROIGenerator(img, 15, make_tuple(fX, fY));  // sequence(ROI, outputImg)
						imwrite(sideColor+"\\ROI-"+images[k], outputImg[0]);
						imwrite(sideColor+"\\Impact-"+images[k], outputImg[1]);
						imwrite(mainFlatOutputDir+"\\ROI-"+images[k], outputImg[0]);
						totalFile += 1;
					}
				} catch (Exception e) {
					failFiles.emplace_back(sides[i]+"/"+sideColors[j]+"/"+images[k]);
					continue;
				}
			}
		}
//		cout << sides[i] << endl;
	}
	for (int l = 0; l < failFiles.size(); ++l) {
		cout << (l == 0? "\nFAIL PROCESSED FILES ("+to_string(failFiles.size()-2)+"):\n ": "")
		<< "\t> " << failFiles[l] << endl;
	}
	cout << endl << "TOTAL FILE: " << totalFile << endl;
}

void DirParser(string dirPath) {
	dirPath = R"(E:\)";
	vector<string> subdirSets = WinUtil::DirContents(dirPath),
	outputDir;

	struct fUtil {
		vector<string> recursive(string& targetPath) {

		}
	};

	for (int i = 0; i < subdirSets.size(); ++i) {
		try {
			string targetPath = dirPath+"\\"+subdirSets[i];
			if (WinUtil::IsDir(targetPath)) {
				cerr << "DIR OK -> " << subdirSets[i] << endl;
				vector<string> newDir = WinUtil::DirContents(targetPath);
			} else {
				cerr << "DIR FAIL -> " << subdirSets[i] << endl;
			}
		} catch (Exception& e) {
			cerr << e.what() << endl;

		}

	}

}

int main() {
	// !!! Execution time counting begin
	auto start = chrono::high_resolution_clock::now();
	ios_base::sync_with_stdio(false);

//	DirParser("");
	MultiFiles();

	string sampleFile = "B_1756_SDY.png";
	string sampleFPath = R"(C:\Users\MarufN\Documents\Coding\C++\RegionOfInterest\assets\dataset\backside\Kuning-Back\)"+sampleFile;
	sampleFPath = R"(E:\output_flat\ROI-B_2683_UKL.png)";
	Mat sampleImg = imread(sampleFPath);

	// !!! Execution time counting end
	auto end = chrono::high_resolution_clock::now();
	double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	time_taken *= 1e-9;
	cout << "\nMain execution time: " << fixed
	     << time_taken << setprecision(9) << " seconds";

	return 0;
}
