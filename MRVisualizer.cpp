//
// Created by kobayashi on 18/05/16.
//
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/xphoto.hpp>
#include <RotationHelpers.h>

#include "MRVisualizer.h"

using namespace std;
using namespace cv;


MRVisualizer::MRVisualizer()
{

}

MRVisualizer::~MRVisualizer()
{

}


void MRVisualizer::DrawBox(cv::Mat image, cv::Vec6f pose, float fx, float fy, float cx, float cy)
{
	float boxVerts[] = { -80, -70, 0,
			80, -70, 0,
			80, -70, -30,
			-80, -70, -30,
			80, -10, -30,
			80, -10, 0,
			-80, -10, 0,
			-80, -10, -30 };

		std::vector<std::pair<int, int>> boxedges;
		boxedges.push_back(std::pair<int, int>(0, 1));
		boxedges.push_back(std::pair<int, int>(1, 2));
		boxedges.push_back(std::pair<int, int>(2, 3));
		boxedges.push_back(std::pair<int, int>(0, 3));
		boxedges.push_back(std::pair<int, int>(2, 4));
		boxedges.push_back(std::pair<int, int>(1, 5));
		boxedges.push_back(std::pair<int, int>(0, 6));
		boxedges.push_back(std::pair<int, int>(3, 7));
		boxedges.push_back(std::pair<int, int>(6, 5));
		boxedges.push_back(std::pair<int, int>(5, 4));
		boxedges.push_back(std::pair<int, int>(4, 7));
		boxedges.push_back(std::pair<int, int>(7, 6));

    float eyeVerts[] = { 
			80, -70, -30,
			-80, -70, -30,
			80, -10, -30,
			-80, -10, -30 };
            
		cv::Mat_<float> box = cv::Mat(8, 3, CV_32F, boxVerts).clone();

        cv::Mat_<float> eyesquare = cv::Mat(4, 3, CV_32F, eyeVerts).clone();

		cv::Matx33f rot = Utilities::Euler2RotationMatrix(cv::Vec3f(pose[3], pose[4], pose[5]));
		cv::Mat_<float> rotBox;

		// Rotate the box
		rotBox = cv::Mat(rot) * box.t();
		rotBox = rotBox.t();

		// Move the bounding box to head position
		rotBox.col(0) = rotBox.col(0) + pose[0];
		rotBox.col(1) = rotBox.col(1) + pose[1];
		rotBox.col(2) = rotBox.col(2) + pose[2];

		// draw the lines
		cv::Mat_<float> rotBoxProj;
		Utilities::Project(rotBoxProj, rotBox, fx, fy, cx, cy);

		std::vector<std::pair<cv::Point2f, cv::Point2f>> lines;
        std::vector<cv::Point> pts;
        for (size_t i = 0; i < rotBoxProj.rows; i++) {
            cv::Point p(rotBoxProj.at<int>(i,0), rotBoxProj.at<int>(i,1));
            pts.push_back(p);
        }
        cv::Mat	back(cv::Size(image.cols, image.rows), CV_8UC3, cv::Scalar(0) );


		for (size_t i = 0; i < boxedges.size(); ++i)
		{
			cv::Mat_<float> begin;
			cv::Mat_<float> end;

			rotBoxProj.row(boxedges[i].first).copyTo(begin);
			rotBoxProj.row(boxedges[i].second).copyTo(end);

			cv::Point2f p1(begin.at<float>(0), begin.at<float>(1));
			cv::Point2f p2(end.at<float>(0), end.at<float>(1));

			lines.push_back(std::pair<cv::Point2f, cv::Point2f>(p1, p2));

		}

        cv::Rect image_rect(0, 0, image.cols, image.rows);
        

		for (size_t i = 0; i < lines.size(); ++i)
		{
			cv::Point2f p1 = lines.at(i).first;
			cv::Point2f p2 = lines.at(i).second;
			// Only draw the line if one of the points is inside the image
			if (p1.inside(image_rect) || p2.inside(image_rect))
			{
				cv::line(image, p1, p2, (0, 0, 0), 1, CV_AA);
			}

		}
}


cv::Mat MRVisualizer::Draw(cv::Mat image, vector<cv::Point2f> sequence_uv, cv::Vec6f pose, const std::vector<cv::Point3f>& eye_landmarks3d, float fx, float fy, float cx, float cy)
{
    cv::Mat edgetest = image.clone();
    DrawBox(edgetest, pose, fx, fy, cx, cy);
    cv::imshow("edge", edgetest);


    cv::Mat virtual_glasses = cv::imread("virtual_glasses.jpg");
    cv::Mat face_tmp = cv::imread("face_model_noglasses.jpg");

    // Generate a Mask_eye
    cv::Mat mask_eye(cv::Size(image.cols, image.rows), CV_8UC1, cv::Scalar(0));
    cv::Mat face_tmp_gray;
    cv::cvtColor(face_tmp, face_tmp_gray, CV_BGR2GRAY);
    cv::threshold(face_tmp_gray, mask_eye, 245, 255, CV_THRESH_BINARY);
    cv::bitwise_not(mask_eye, mask_eye);

    cv::Mat mask_glasses(cv::Size(image.cols, image.rows), CV_8UC1, cv::Scalar(0));
    cv::Mat face_glasses_gray;
    cv::cvtColor(virtual_glasses, face_glasses_gray, CV_BGR2GRAY);
    cv::threshold(face_glasses_gray, mask_glasses, 200, 255, CV_THRESH_BINARY);
    cv::bitwise_not(mask_glasses, mask_glasses);
    
	std::vector<cv::Point> pts1, pts2, pts3;
    for (int i = 0; i < sizeof(contours) / sizeof(contours[0]); i++){
        pts1.push_back(sequence_uv[contours[i]]);
    }
	cv::fillConvexPoly(mask_eye, pts1.data(), sizeof(contours) / sizeof(contours[0]), cv::Scalar(0), cv::LINE_AA);
    for (int i = 0; i < sizeof(contours_leye) / sizeof(contours_leye[0]); i++){
        pts2.push_back(sequence_uv[contours_leye[i]]);
        pts3.push_back(sequence_uv[contours_reye[i]]);
    }
	cv::fillConvexPoly(mask_eye, pts2.data(), sizeof(contours_leye) / sizeof(contours_leye[0]), cv::Scalar(0), cv::LINE_AA);
    cv::fillConvexPoly(mask_eye, pts3.data(), sizeof(contours_reye) / sizeof(contours_reye[0]), cv::Scalar(0), cv::LINE_AA);
    cv::fillConvexPoly(mask_glasses, pts2.data(), sizeof(contours_leye) / sizeof(contours_leye[0]), cv::Scalar(0), cv::LINE_AA);
    cv::fillConvexPoly(mask_glasses, pts3.data(), sizeof(contours_reye) / sizeof(contours_reye[0]), cv::Scalar(0), cv::LINE_AA);


    // alpha blending
    cv::Mat blurmask;
    cv::Mat element4 = (cv::Mat_<uchar>(3,3)<< 0,1,0,1,1,1,0,1,0);
    cv::erode(mask_eye, blurmask, element4, cv::Point(-1,-1), 2);
    cv::blur(blurmask, blurmask, Size(5,5));
    cv::imwrite("mask.png", blurmask);
    
    // imshow("blur", blurmask);
    Mat foreground = face_tmp.clone();
    Mat background = image.clone(); //image
    Mat alpha1 = blurmask.clone();
    // Convert Mat to float data type
    foreground.convertTo(foreground, CV_32FC3);
    background.convertTo(background, CV_32FC3);
    alpha1.convertTo(alpha1, CV_32FC3, 1.0/255);
    // Normalize the alpha mask to keep intensity between 0 and 1
    cv::normalize(alpha1, alpha1, 0.0f, 1.0f, cv::NORM_MINMAX);
    cv::Mat	alpha(cv::Size(foreground.cols, foreground.rows), CV_32FC3, cv::Scalar(0,0,0) );
    std::vector<cv::Mat> alpha_mv;
    cv::split(alpha, alpha_mv);
    alpha_mv[0] = alpha_mv[1] = alpha_mv[2] = alpha1;
    cv::Mat alpha_tmp, alpha32fc3;
    cv::merge(alpha_mv, alpha_tmp);
    alpha_tmp.convertTo(alpha32fc3, CV_32FC3);
    cv::Mat abimage = cv::Mat::zeros(foreground.size(), foreground.type());
    // Multiply the foreground with the alpha matte
    cv::multiply(alpha32fc3, foreground, foreground);
    // Multiply the background with ( 1 - alpha )
    cv::multiply(Scalar::all(1.0)-alpha32fc3, background, background);
    // Add the masked foreground and background.
    cv::add(foreground, background, abimage);
    // Display image
    abimage.convertTo(abimage, CV_8UC3);
    // cv::erode(mask_glasses, mask_glasses, element4, cv::Point(-1,-1), 2);
    // cv::blur(mask_glasses, mask_glasses, Size(3,3));
    virtual_glasses.copyTo(abimage, mask_glasses);
    cv::Mat result;
    cv::resize(abimage, result, cv::Size(), 1.0, 1.0);
    cv::imshow("glasses blending", result);
    
    // // Save result images 
    static int i = 0;
    std::ostringstream oss;
    oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
    // cv::imwrite("output/inpainting_openface" + oss.str() + ".png", abimage);
 
    return abimage;
}