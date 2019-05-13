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

#include "FaceUtils.h"

using namespace std;
using namespace cv;


FaceUtilizer::FaceUtilizer()
{

}

FaceUtilizer::~FaceUtilizer()
{

}


void FaceUtilizer::Draw_FaceModel(const char *filename, cv::Mat1f landmarks, vector<cv::Point2f> uv)
{
    ifstream file;
    file.open(filename);

    if( !file ){
        cout << "Error: cannot open file(" << filename << ")" << endl;
        exit(1);
    }

    string str;

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

        while(str[i] != ' ') {//'/'
            i++;
            k = i;
        }

        int one = atof(str.substr(j, k - j).c_str());

        while(str[i] != ' ') i++;
        while(str[i] == ' ') i++;
        //i +=1;
        j = i;
        k = i;

        while(str[i] != ' ') {//'/'
            i++;
            k = i;
        }

        int two = atof(str.substr(j, k - j).c_str());

        while(str[i] != ' ') i++;
        while(str[i] == ' ') i++;
        //i+=1;

        j = i;
        k = i;

        while(str[i] != ' ') {//'/'
            i++;
            k = i;
        }

        int three = atof(str.substr(j, k - j).c_str());

        glBegin(GL_TRIANGLES);
        glTexCoord2f(uv[one - 1].x/ 640, 1.0 - (uv[one - 1].y/ 480));
        glVertex3f(landmarks.at<float>(one - 1)*0.01f, -landmarks.at<float>((one - 1) + 68*1)*0.01f, -landmarks.at<float>((one - 1) + 68*2)*0.01f);
        glTexCoord2f(uv[two - 1].x/ 640, 1.0 - (uv[two - 1].y/ 480));
        glVertex3f(landmarks.at<float>(two - 1)*0.01f, -landmarks.at<float>((two - 1)+ 68*1)*0.01f, -landmarks.at<float>((two - 1)+ 68*2)*0.01f);
        glTexCoord2f(uv[three - 1].x/ 640, 1.0 - (uv[three - 1].y/ 480));
        glVertex3f(landmarks.at<float>(three - 1)*0.01f, -landmarks.at<float>((three - 1)+ 68*1)*0.01f, -landmarks.at<float>((three - 1) + 68*2)*0.01f);
        glEnd();
        getline(file, str);
        cnt++;
        i = 0;
    }
}

void FaceUtilizer::OcclusionHandler(const char *headfacefile, const char *filename, cv::Mat1f landmarks, cv::Vec6f pose)
{
    ifstream file;
    file.open(filename);

    if( !file ){
        cout << "Error: cannot open file(" << filename << ")" << endl;
        exit(1);
    }

    string str;

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

        while(str[i] != ' ') {//'/'
            i++;
            k = i;
        }

        int one = atof(str.substr(j, k - j).c_str());

        while(str[i] != ' ') i++;
        while(str[i] == ' ') i++;
        //i +=1;
        j = i;
        k = i;

        while(str[i] != ' ') {//'/'
            i++;
            k = i;
        }

        int two = atof(str.substr(j, k - j).c_str());

        while(str[i] != ' ') i++;
        while(str[i] == ' ') i++;
        //i+=1;

        j = i;
        k = i;

        while(str[i] != ' ') {//'/'
            i++;
            k = i;
        }

        int three = atof(str.substr(j, k - j).c_str());

        glColor3f(1.0, 1.0, 1.0);
        GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
        glMaterialfv(GL_FRONT, GL_AMBIENT, white);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        glBegin(GL_TRIANGLES);
        glVertex3f(landmarks.at<float>(one - 1)*0.01f, -landmarks.at<float>((one - 1) + 68*1)*0.01f, -landmarks.at<float>((one - 1) + 68*2)*0.01f);
        glVertex3f(landmarks.at<float>(two - 1)*0.01f, -landmarks.at<float>((two - 1)+ 68*1)*0.01f, -landmarks.at<float>((two - 1)+ 68*2)*0.01f);
        glVertex3f(landmarks.at<float>(three - 1)*0.01f, -landmarks.at<float>((three - 1)+ 68*1)*0.01f, -landmarks.at<float>((three - 1) + 68*2)*0.01f);
        glEnd();
        getline(file, str);
        cnt++;
        i = 0;
    }

    std::vector<float> headvec;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            for(int k = 0; k < index.size(); k++) {
                headvec.push_back(landmarks.at<float>(index[k] + 68*i));
            }
        }
    }

    cv::Matx33f rot = Utilities::Euler2RotationMatrix(cv::Vec3f(pose[3], pose[4], pose[5]));
    cv::Matx33f inv = rot.inv();

    cv::Mat_<float> head(headvec);
    head = head.reshape(42);
    
    head.row(0) = head.row(0) - pose[0];
	head.row(1) = head.row(1) - pose[1];
	head.row(2) = head.row(2) - pose[2];
    cv::Mat iniHead = cv::Mat(inv) * head;

    for(int i = 1; i < 3; i++) {
        for(int j = 0; j < 14; j++) {
            iniHead.at<float>(2, j + i * 14) = iniHead.at<float>(2, j + i * 14) + i * 30.0f;
            if(j >= 4 && j < 10) {
                iniHead.at<float>(1, j + i * 14) = iniHead.at<float>(1, j + i * 14) - 80.0f;
            }
        }
    }

    cv::Mat rotHead = cv::Mat(rot) * iniHead;
    rotHead.row(0) = rotHead.row(0) + pose[0];
	rotHead.row(1) = rotHead.row(1) + pose[1];
	rotHead.row(2) = rotHead.row(2) + pose[2];

    // Draw head face
    ifstream headface;
    headface.open(headfacefile);

    if( !headface ){
        cout << "Error: cannot open file(" << headfacefile << ")" << endl;
        exit(1);
    }

    string hstr;
    
    while(true)
    {
        getline(headface, hstr);
        if(hstr[0] == 'f' ) {
            break;
        }
    }
    
    int hi = 0;
    int hcnt = 0;
    while(hstr[0] == 'f')
    {

        while(hstr[hi] == 'f') hi++;
        while(hstr[hi] == ' ') hi++;
        int j = hi, k = hi;

        while(hstr[hi] != ' ') {//'/'
            hi++;
            k = hi;
        }

        int one = atof(hstr.substr(j, k - j).c_str());
        

        while(hstr[hi] != ' ') hi++;
        while(hstr[hi] == ' ') hi++;
        //i +=1;
        j = hi;
        k = hi;

        while(hstr[hi] != ' ') {//'/'
            hi++;
            k = hi;
        }

        int two = atof(hstr.substr(j, k - j).c_str());
        
        while(hstr[hi] != ' ') hi++;
        while(hstr[hi] == ' ') hi++;
        //i+=1;

        j = hi;
        k = hi;

        while(hstr[hi] != ' ') {//'/'
            hi++;
            k = hi;
        }

        int three = atof(hstr.substr(j, k - j).c_str());

        glBegin(GL_TRIANGLES);
        glVertex3f(rotHead.at<float>(0,(one - 1))*0.01f, -rotHead.at<float>(1,(one - 1))*0.01f, -rotHead.at<float>(2,(one - 1))*0.01f);
        glVertex3f(rotHead.at<float>(0,(two - 1))*0.01f, -rotHead.at<float>(1,(two - 1))*0.01f, -rotHead.at<float>(2,(two - 1))*0.01f);
        glVertex3f(rotHead.at<float>(0,(three - 1))*0.01f, -rotHead.at<float>(1,(three - 1))*0.01f, -rotHead.at<float>(2,(three - 1))*0.01f);
        glEnd();
        getline(headface, hstr);
        hcnt++;
        hi = 0;
    }

}
