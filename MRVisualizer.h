//
// Created by kobayashi on 18/05/16.
//
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef INCLUDED_MRVisualizer_H
#define INCLUDED_MRVisualizer_H


class MRVisualizer {
    private:
        int contours[14] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 29};
        int contours_leye[6] = {36, 37, 38, 39, 40, 41};
        int contours_reye[6] = {42, 43, 44, 45, 46, 47};
        
    public:
        MRVisualizer();
        ~MRVisualizer();

        void DrawBox(cv::Mat image, cv::Vec6f pose, float fx, float fy, float cx, float cy);
        cv::Mat Draw(cv::Mat image, std::vector<cv::Point2f> sequence_uv, cv::Vec6f pose, const std::vector<cv::Point3f>& eye_landmarks3d, float fx, float fy, float cx, float cy);
};


#endif //GLTEST_READER_H
