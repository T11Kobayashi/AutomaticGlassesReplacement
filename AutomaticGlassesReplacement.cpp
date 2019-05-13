// Edited file of FeatureExtraction.cpp
// Local includes
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "LandmarkCoreIncludes.h"

#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <Face_utils.h>
#include <FaceAnalyser.h>
#include <GazeEstimation.h>
#include <RecorderOpenFace.h>
#include <RecorderOpenFaceParameters.h>
#include <ImageCapture.h>
#include <SequenceCapture.h>
#include <Visualizer.h>
#include <VisualizationUtils.h>
#include <chrono>

#include "GlassesUtils.h"
#include "FaceUtils.h"
#include "MRVisualizer.h"

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define OBJGLASSES "JINStest.obj" //"JINS3DGlasses.obj"
#define OBJFILE "face2_tmp_3Dlandmarks.obj"
#define IMGFILE "face.jpg"
#define HEADFACEFILE "head.txt"
#define FX 535.13536076f
#define FY 533.03406853f
#define CX 313.60435975f
#define CY 235.82326887f

GlassesUtilizer glasses_utilizer;
FaceUtilizer face_utilizer;
MRVisualizer mr_visualizer;


cv::Vec6f pose;
cv::Vec6d pose_estimate;
cv::Mat temp;
cv::Mat1f landmarks;
vector<cv::Point2f> uv;

#ifndef CONFIG_DIR
#define CONFIG_DIR "~"
#endif

#define INFO_STREAM( stream ) \
std::cout << stream << std::endl

#define WARN_STREAM( stream ) \
std::cout << "Warning: " << stream << std::endl

#define ERROR_STREAM( stream ) \
std::cout << "Error: " << stream << std::endl

static void printErrorAndAbort(const std::string & error)
{
	std::cout << error << std::endl;
}

#define FATAL_STREAM( stream ) \
printErrorAndAbort( std::string( "Fatal error: " ) + stream )

using namespace std;

vector<string> get_arguments(int argc, char **argv)
{

	vector<string> arguments;

	// First argument is reserved for the name of the executable
	for (int i = 0; i < argc; ++i)
	{
		arguments.push_back(string(argv[i]));
	}
	return arguments;
}

void init(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(FRAME_WIDTH, FRAME_HEIGHT);
    glutInitWindowPosition(600, 150);
    glutCreateWindow("Generic RPG 3D");
	
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

GLuint loadTexture(std::string filename)
{
    // テクスチャIDの生成
    GLuint texID;
    glGenTextures(1, &texID);

    // ファイルの読み込み
    cv::Mat image = cv::imread(filename);

    if(image.empty()){
        std::cout << "image empty" << std::endl;
    }else {
        cv::flip(image, image, 0);

        // テクスチャをGPUに転送
        glBindTexture(GL_TEXTURE_2D, texID);

        // テクスチャの設定
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());
    }

    return texID;
}

void display(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    cv::flip(temp, temp, 0);
    cv::cvtColor(temp, temp, cv::COLOR_BGR2RGB);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glLoadIdentity();

	glTranslatef(0.0f, 0.0f, 0.8f);

    // glDrawPixels(temp.cols, temp.rows, GL_RGB, GL_UNSIGNED_BYTE, temp.data);
	glClear(GL_DEPTH_BUFFER_BIT);

    // glEnable(GL_TEXTURE_2D);//テクスチャ有効
    // face_utilizer.Draw_FaceModel(OBJFILE, landmarks, uv);
	// glDisable(GL_TEXTURE_2D);//テクスチャ無効	
	
    cv::Mat output_face(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
    glReadPixels(0, 0, output_face.cols, output_face.rows, GL_BGR, GL_UNSIGNED_BYTE, output_face.data);
	cv::flip(output_face, output_face, 0);
	cv::imwrite("face_model_noglasses.jpg", output_face);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	face_utilizer.OcclusionHandler(HEADFACEFILE, OBJFILE, landmarks, pose_estimate);
	
	glTranslatef(0.0f, 0.0f, 0.2f);
	glTranslatef(pose_estimate[0]*0.01f, -pose_estimate[1]*0.01f, -pose_estimate[2]*0.01f);
    glRotatef(pose[3] - 10.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(pose[4], 0.0f, 1.0f, 0.0f);
    glRotatef(pose[5], 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.35f, 5.1f);

	glasses_utilizer.DrawGlasses();
	
    glutSwapBuffers();

    GLuint tex = loadTexture(IMGFILE);
    //テクスチャをバインド
    glBindTexture(GL_TEXTURE_2D, tex);

    cv::Mat output(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
    glReadPixels(0, 0, output.cols, output.rows, GL_BGR, GL_UNSIGNED_BYTE, output.data);
    cv::flip(output, output, 0);
    cv::imwrite("virtual_glasses.jpg", output);
}

void reshape( int width, int height )
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glasses_utilizer.GlassesLoad(OBJGLASSES);

	cv::VideoCapture cap(0);//デバイスのオープン
	
    if(!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
    {
        //読み込みに失敗したときの処理
        return -1;
    }

    while(1)//無限ループ
    {
        cv::Mat frame;
        cap >> frame; // get a new frame from camera

        cv::imshow("window", frame);//画像を表示．

        int key = cv::waitKey(1);
        if(key == 113)//qボタンが押されたとき
        {
            break;//whileループから抜ける．
        }
        else if(key == 115)//sが押されたとき
        {
            //フレーム画像を保存する．
            cv::imwrite(IMGFILE, frame);
			std::cout << "saved" << std::endl;
        }
    }
    cv::destroyAllWindows();
	cap.release();

	// OpenFace for template image
	cv::Mat rgb_image = cv::imread(IMGFILE);
	cv::Mat gray_image;
	cv::cvtColor(rgb_image, gray_image, cv::COLOR_RGB2GRAY);


	cv::VideoWriter video_writer = cv::VideoWriter("AR_video.avi", CV_FOURCC_DEFAULT, 30, cv::Size(640, 480), true);

	// input from USB camera
	// cv::VideoWriter input_writer = cv::VideoWriter("input.avi", CV_FOURCC_DEFAULT, 30, cv::Size(640, 480), true);

	// cv::VideoCapture vcap(0);//デバイスのオープン
    
    // if(!vcap.isOpened())//カメラデバイスが正常にオープンしたか確認．
    // {
    //     //読み込みに失敗したときの処理
    //     return -1;
    // }

    // while(1)//無限ループ
    // {
    //     cv::Mat frame;
    //     vcap >> frame; // get a new frame from camera

	// 	input_writer << frame;
    //     cv::imshow("window", frame);//画像を表示．

    //     int key = cv::waitKey(1);
    //     if(key == 113)//qボタンが押されたとき
    //     {
	// 		input_writer << cv::imread("example.png");
    //         break;//whileループから抜ける．
    //     }        
    // }
    // cv::destroyAllWindows();
	
	// Convert arguments to more convenient vector form
	vector<string> arguments = get_arguments(argc, argv);

	// no arguments: output usage
	if (arguments.size() == 1)
	{
		cout << "For command line arguments see:" << endl;
		cout << " https://github.com/TadasBaltrusaitis/OpenFace/wiki/Command-line-arguments";
		return 0;
	}

	// Load the modules that are being used for tracking and face analysis = cv::imread("face.png", CV_8UC1)
	// Load face landmark detector
	LandmarkDetector::FaceModelParameters det_parameters(arguments);
	// Always track gaze in feature extraction
	LandmarkDetector::CLNF face_model(det_parameters.model_location);

	if (!face_model.loaded_successfully)
	{
		cout << "ERROR: Could not load the landmark detector" << endl;
		return 1;
	}

	Utilities::SequenceCapture sequence_reader;

	// A utility for visualizing the results
	Utilities::Visualizer visualizer(arguments);

	// Tracking FPS for visualization
	Utilities::FpsTracker fps_tracker;
	fps_tracker.AddFrame();

	bool success = LandmarkDetector::DetectLandmarksInVideo(rgb_image, face_model, det_parameters, gray_image);
	uv = LandmarkDetector::CalculateAllLandmarks(face_model);

	init(argc, argv);
	//start
    std::chrono::system_clock::time_point start,end;
    start = std::chrono::system_clock::now();
	while (true) // this is not a for loop as we might also be reading from a webcam
	{

		// The sequence reader chooses what to open based on command line arguments provided
		if (!sequence_reader.Open(arguments))
			break;

		INFO_STREAM("Device or file opened");

		if (sequence_reader.IsWebcam())
		{
			INFO_STREAM("WARNING: using a webcam in feature extraction, Action Unit predictions will not be as accurate in real-time webcam mode");
			INFO_STREAM("WARNING: using a webcam in feature extraction, forcing visualization of tracking to allow quitting the application (press q)");
			visualizer.vis_track = true;
		}

		cv::Mat captured_image;

		captured_image = sequence_reader.GetNextFrame();

		// For reporting progress
		double reported_completion = 0;

		INFO_STREAM("Starting tracking");
		while (!captured_image.empty())
		{

			// Converting to grayscale
			cv::Mat_<uchar> grayscale_image = sequence_reader.GetGrayFrame();

			// The actual facial landmark detection / tracking
			bool detection_success = LandmarkDetector::DetectLandmarksInVideo(captured_image, face_model, det_parameters, grayscale_image);

			// Do face alignment
			cv::Mat sim_warped_img;
			cv::Mat_<double> hog_descriptor; int num_hog_rows = 0, num_hog_cols = 0;

			pose_estimate = LandmarkDetector::GetPose(face_model, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy);
			pose[0] = -pose_estimate[0]/100;
			pose[1] = pose_estimate[1]/100;
			pose[2] = pose_estimate[2]/100;
			pose[3] = -1 * (-pose_estimate[3]) * 180 / M_PI;
			pose[4] = (-pose_estimate[4]) * 180 / M_PI;
			pose[5] = (-pose_estimate[5]) * 180 / M_PI;
			// Keeping track of FPS
			fps_tracker.AddFrame();

			// Displaying the tracking visualizations
			visualizer.SetImage(captured_image, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy);
			visualizer.SetObservationLandmarks(face_model.detected_landmarks, face_model.detection_certainty, face_model.GetVisibilities());
			visualizer.SetObservationPose(pose_estimate, face_model.detection_certainty);

			landmarks = face_model.GetShape(sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy);
			temp = captured_image.clone();
			vector<cv::Point2f> sequence_uv = LandmarkDetector::CalculateAllLandmarks(face_model);
    		glutDisplayFunc(display);
    		glutReshapeFunc(reshape);
    		glutIdleFunc(display);
    		glutMainLoopEvent();
			glutPostRedisplay();
			cv::Mat drimage;
			drimage = mr_visualizer.Draw(captured_image, sequence_uv, pose_estimate, LandmarkDetector::Calculate3DEyeLandmarks(face_model, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy), sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy);

			visualizer.SetFps(fps_tracker.GetFPS());
			// detect key presses
			char character_press = visualizer.ShowObservation();

			// quit processing the current sequence (useful when in Webcam mode)
			if (character_press == 'q')
			{
				break;
			}

			// Reporting progress
			if (sequence_reader.GetProgress() >= reported_completion / 10.0)
			{
				cout << reported_completion * 10 << "% ";
				if (reported_completion == 10)
				{
					cout << endl;
				}
				reported_completion = reported_completion + 1;
			}

			// Grabbing the next frame in the sequence
			captured_image = sequence_reader.GetNextFrame();

		}
		// end
		end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();
    	std::cout << elapsed <<"ms"<< std::endl;

		// Write output video
		while (true) {
			static int i = 1;
    		std::ostringstream oss;
    		oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
			cv::Mat image = cv::imread( "output/inpainting_openface" + oss.str() + ".png" );
			if( image.empty() ){
            	break;
        	}
			video_writer << image;
		}
		sequence_reader.Close();
		face_model.Reset();
	}
	return 0;
}
