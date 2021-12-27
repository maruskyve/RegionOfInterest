//
// Created by MarufN on 26/12/2021.
//

#ifndef REGIONOFINTEREST_ENHANCED_ROI_DETECTION_H
#define REGIONOFINTEREST_ENHANCED_ROI_DETECTION_H
#include <bits/stdc++.h>
#include <opencv2/highgui.hpp>
#include "../../CppImageProcessing/improc.h"
using namespace std;
using namespace cv;

class ROIUtil {
public:
	// Generate image with green Bounding Box
	static Mat BoundingBox(const Mat& inputImg, tuple<uint16_t, uint16_t> rowRange, tuple<uint16_t, uint16_t> colRange, uint8_t borderSize) {
		uint32_t fRows = get<0>(rowRange), lRows = get<1>(rowRange),
				fCols = get<0>(colRange), lCols = get<1>(colRange);
		uint32_t tBorder = fRows+borderSize, bBorder = lRows-borderSize,
				lBorder = fCols+borderSize, rBorder = lCols-borderSize;
		Mat outputImg = inputImg.clone();

		for (uint_fast32_t i = fRows-borderSize; i < lRows+borderSize; ++i) {
			for (uint_fast32_t j = fCols-borderSize; j < lCols+borderSize; ++j) {
				uint8_t *imgPix = outputImg.ptr(i, j);

				for (uint_fast8_t k = 0; k < 3; ++k) {
					if (i <= tBorder-borderSize || i >= bBorder+borderSize ||
					    j <= lBorder-borderSize || j >= rBorder+borderSize) {
						imgPix[k] = k == 1? 255 : 0;  // (0, 255, 0) Green color space
					}
				}
			}
		}
		cout << "SDSD";
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

	static bool FindHighDominant(const Mat& inputImg) {  // Search for min region white edge (cTop, cBottom)
		Mat inputImgEdge;
		inputImgEdge = ops.Grayscale(inputImg.clone(), "yiq");
		inputImgEdge = ops.Thresholding(inputImgEdge, 127);
//		Canny(inputImgEdge, inputImgEdge, 0, 127, 3);
		inputImgEdge = Convolution::EdgeDetection(inputImgEdge, 2);
		int16_t imgRows = inputImgEdge.rows, imgCols = inputImgEdge.cols;
		int16_t centerX = round(imgRows/2), centerY = round(imgCols/2);

		vector<Mat> outputImg = {inputImgEdge.clone(), inputImgEdge.clone()}; // sequence(cTop, cBottom)
		vector<uint16_t> intensityFreq = {0, 0};  // sequence(cTopHighFreq, cBottomHighFreq)

		outputImg[0] = outputImg[0].rowRange(round(.25*centerY), centerY).
				colRange(centerX-round(.5*centerX), centerX+round(.5*centerX));
		outputImg[1] = outputImg[1].rowRange(centerY, centerY + round(.25*centerY)).
				colRange(centerX-round(.5*centerX), centerX+round(.5*centerX));

		for (uint_fast32_t i = 0; i < outputImg.size(); ++i) {
			for (uint_fast32_t j = 0; j < outputImg[i].rows; ++j) {
				for (uint_fast32_t k = 0; k < outputImg[k].cols; ++k) {
					uint8_t *imgPix = outputImg[i].ptr(j, k);
					for (uint_fast32_t l = 0; l < 3; ++l) {
						intensityFreq[i] += imgPix[l] == 0? 1: 0;  // White edge priority
					}
				}
			}
		}

		uint16_t intensityMin = min(intensityFreq[0], intensityFreq[1]);  // Determine the most minimum of both image high frequency
		bool intensityCand = intensityMin == intensityFreq[1];  // Determine which

		return intensityCand;
	};

};

vector<Mat> StdROIDetection(const Mat& img, const uint16_t& roiSize) {
	uint16_t imgRows = img.rows, imgCols = img.cols;
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX = centerX-round(roiSize/2), currentY = centerY-round(roiSize/2);
	uint16_t ROISIZE = round(roiSize/2);

	Mat outputImg = img.clone(), outputROIImg;
	outputROIImg = img.colRange(currentX - ROISIZE, currentX + ROISIZE).
			rowRange(currentY - ROISIZE, currentY + ROISIZE).clone();

	outputImg = ROIUtil::BoundingBox(outputImg, make_tuple(currentY-ROISIZE, currentY+ROISIZE),
			make_tuple(currentX-ROISIZE, currentX+ROISIZE), 2);

	return {outputROIImg, outputImg};
}

vector<Mat> EnhancedROIDetection(const Mat& inputImg, const uint16_t & roiSize) {
	uint16_t imgRows = inputImg.rows, imgCols = inputImg.cols;
	uint16_t centerX = round(imgCols/2), centerY = round(imgRows/2);
	uint16_t currentX = centerX, currentY = centerY;
	float_t CRITERION = 1;
	uint16_t ROISIZE = round(roiSize/2);

	Mat outputImg = inputImg.clone(), outputROIImg;
	Mat heqImg = Operation::HistogramEqualization(inputImg, "adaptive");

	try {
		bool regionStat = ROIUtil::FindHighDominant(outputImg);
		cout << "decisionVal: " << regionStat << endl;

		while (true) {
			outputROIImg = heqImg.colRange(currentX - roiSize, currentX + roiSize).
					rowRange(currentY - roiSize, currentY + roiSize).clone();

			outputROIImg = ops.Brightness(outputROIImg, +100);
			outputROIImg = ops.Grayscale(outputROIImg, "yiq");
			outputROIImg = ops.Thresholding(outputROIImg, 175);

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
			if (refRatio >= CRITERION) {
//			imshow("", outputROIImg);
				cout << "PERFECT IMAGE with," << endl;
				cout << "\t-whitePercentage: " << refRatio << endl;
				cout << "\t-center(X,Y): (" << currentX << ", " << currentY << ")" << endl;
				cout << "\t-imgSize(Cols,Rows): (" << imgCols << "," << imgRows << ")" << endl;
				outputROIImg = inputImg.colRange(currentX - ROISIZE, currentX + ROISIZE).
						rowRange(currentY - ROISIZE, currentY + ROISIZE);
				outputImg = ROIUtil::BoundingBox(outputImg, make_tuple(currentY-ROISIZE, currentY+ROISIZE),
						make_tuple(currentX-ROISIZE, currentX+ROISIZE), 2);
				break;
			} else {
				cout << "NOT PERFECT IMAGE" << endl;
				cout << currentY << " from center " << centerY << ", max/cols " << imgCols <<
				     ", regStat " << regionStat << endl << endl;
				// Handler: Avoid ROI image position out of input image by assign to default ROIDetection
				if ((regionStat == 0 && currentY == roiSize) ||
				    (regionStat == 1 && imgCols-roiSize == currentY)) {
					cout << "Couldn't find solution, throw to stdROI algorithm" << endl;
					vector<Mat> alternativeOutputImg = StdROIDetection(inputImg, roiSize);
					outputROIImg = alternativeOutputImg[0];
					outputImg = alternativeOutputImg[1];
					break;
				} else {
					if (!regionStat) {  // For image with center top dominant white edge
						currentY -= 1;
					} else {
						currentY += 1;
					}
				}
			}
		}
	} catch (Exception & ex) {
		vector<Mat> altOutputImg = StdROIDetection(inputImg, roiSize);
		outputROIImg = altOutputImg[0];
		outputImg = altOutputImg[1];
	}
	return {outputROIImg, outputImg};
}

#endif //REGIONOFINTEREST_ENHANCED_ROI_DETECTION_H
