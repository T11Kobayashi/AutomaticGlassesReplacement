//
// Created by kobayashi on 18/05/16.
//
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef INCLUDED_GlassesUtilizer_H
#define INCLUDED_GlassesUtilizer_H


class GlassesUtilizer {
    private:
        std::vector<float> vertex;
        std::vector<std::tuple<int, int, int>> edges;
        
    public:
        GlassesUtilizer();
        ~GlassesUtilizer();

        void GlassesLoad(const char* filename);
        void DrawGlasses();
};


#endif //GLTEST_READER_H
