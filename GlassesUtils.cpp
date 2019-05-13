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

#include "GlassesUtils.h"

using namespace std;
using namespace cv;


GlassesUtilizer::GlassesUtilizer()
{

}

GlassesUtilizer::~GlassesUtilizer()
{

}

void GlassesUtilizer::GlassesLoad(const char* filename)
{
    ifstream file;
    file.open(filename);
    if( !file ){
        cout << "Error: cannot open file(" << filename << ")" << endl;
        exit(1);
    }

    string str;

    while(!file.eof()) //while we are still in the file
    {
        getline(file,str); //move one line down
        if(str[0] == 'v') break; //if we have a vertex line, stop
    }

    int v = 0;
    //vector
    while(str[0] == 'v') {

        int i = 0;

        if (str[1] == ' ') {
            while (true) {
                i++;
                while (str[i] == ' ') {
                    i++; //move a space over
                }

                int j = i, k = i;
                while (str[i] != ' ') {
                    i++;
                    k = i;
                }
                vertex.push_back(atof(str.substr(j, k - j).c_str())*0.01);
                v++;
                //moving on to the y coord

                while (str[i] == ' ') {
                    i++;
                }

                int q = i, w = i;
                while (str[i] != ' ') {
                    i++;
                    w = i;
                }
                vertex.push_back(atof(str.substr(q, w - q).c_str())*0.01);
                v++;
                while (str[i] == ' ') {
                    i++;
                }

                int a = i, s = i;
                while (str[i] != ' ') {
                    i++;
                    s = i;
                }
                vertex.push_back(atof(str.substr(a, s - a).c_str())*0.01 - 5.0f);
                break;
            }
            v++;
        }
        getline(file, str);
    }

    //face
    while(true)
    {
        getline(file, str);
        if(str[0] == 'f' ) {
            break;
        }
    }

    int i = 0;
    int cnt = 0;
    
    while(str[0] == 'f')
    {

        while(str[i] == 'f') i++;
        while(str[i] == ' ') i++;
        int j = i, k = i;

        while(str[i] != '/') {
            i++;
            k = i;
        }

        int one = atof(str.substr(j, k - j).c_str());

        while(str[i] != ' ') i++;
        while(str[i] == ' ') i++;
        j = i;
        k = i;

        while(str[i] != '/') {
            i++;
            k = i;
        }

        int two = atof(str.substr(j, k - j).c_str());

        while(str[i] != ' ') i++;
        while(str[i] == ' ') i++;

        j = i;
        k = i;

        while(str[i] != '/') {
            i++;
            k = i;
        }

        int three = atof(str.substr(j, k - j).c_str());

        edges.push_back(tuple<int, int, int>(one, two, three));
        
        getline(file, str);
        cnt++;
        i = 0;
    }
}

void GlassesUtilizer::DrawGlasses()
{
    for (size_t i = 1; i < edges.size(); ++i) {
        int one = std::get<0>(edges[i]);
        int two = std::get<1>(edges[i]);
        int three = std::get<2>(edges[i]);

        struct MaterialStruct {
            GLfloat ambient[4];
            GLfloat diffuse[4];
            GLfloat specular[4];
            GLfloat shininess;
        };
        //ruby(ルビー)
        MaterialStruct ms_ruby  = {
            {0.1745,   0.01175,  0.01175,   1.0},
            {0.61424,  0.04136,  0.04136,   1.0},
            {0.847656, 0.117187, 0.093750,  1.0},
            10.0};

        float r = 217.0f / 256.0f;
        float g = 30.0f / 256.0f;
        float b = 24.0f / 256.0f;
        GLfloat color[] = {r, g, b, 1.0 };
        GLfloat lightpos[] = {0, 300, 800, 1.0};
        // glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);
        glBegin(GL_TRIANGLES);
        glVertex3d(vertex[(one - 1)*3], vertex[(one - 1)*3 + 1], vertex[(one - 1)*3 + 2]);
        glVertex3d(vertex[(two - 1)*3], vertex[(two - 1)*3 + 1], vertex[(two - 1)*3 + 2]);
        glVertex3d(vertex[(three - 1)*3], vertex[(three - 1)*3 + 1], vertex[(three - 1)*3 + 2]);
        glEnd();
    }
}
