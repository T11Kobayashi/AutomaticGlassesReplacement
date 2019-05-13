//
// Created by kobayashi on 18/05/16.
//
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef INCLUDED_FaceUtils_H
#define INCLUDED_FaceUtils_H


class FaceUtilizer {
    private:
        std::vector<int> index{3, 2, 1, 0, 17, 18, 19, 24, 25, 26, 16, 15, 14, 13};
        
    public:
        FaceUtilizer();
        ~FaceUtilizer();

        void Draw_FaceModel(const char* filename, cv::Mat1f landmarks, std::vector<cv::Point2f> uv);
        void OcclusionHandler(const char *headfacefile, const char *filename, cv::Mat1f landmarks, cv::Vec6f pose);
};


#endif //GLTEST_READER_H
