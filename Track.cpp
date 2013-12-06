#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
using namespace std;
using namespace cv;

#define VIDEO_DEVICE_NO 1

class Tracker {
public:
    Tracker() {
        capture = NULL;
    }
    ~Tracker() {
        if (capture != NULL)
            capture->release();
    }
    bool InitSkinModel() {
        this->skin_model = Mat::zeros(Size(256, 256), CV_8UC1);
        ellipse(this->skin_model, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
        return true;
    }
    bool StartCamera() {
        if (capture != NULL)
            return false;
        capture = new VideoCapture(VIDEO_DEVICE_NO);
        if (capture->isOpened())
            return true;
        return false;
    }
    bool StopCamera() {
        if (capture != NULL) {
            capture->release();
            waitKey(1);
        }
        return true;
    }
    bool GetNextFrame() {
        if (!capture->read(this->src_img))
            return false;

        src_img.convertTo(src_img, CV_32FC3);
        normalize(src_img, src_img, 1.0, 0.0, CV_MINMAX);
        return true;
    }
    void Display() {
        imshow("source", this->src_img);
        imshow("mask", this->mask);
    }

    bool GenerateBackground() {
        if (GetNextFrame() == false)
            return false;
        src_img.copyTo(background);
        src_img.copyTo(pre_frame);
        return true;
    }

    void SkinExtract() {
        src_img.convertTo(src_img, CV_8UC3, 255);
        mask = Mat::zeros(src_img.size(), CV_8UC1);

        Mat yuv;
        cvtColor(src_img, yuv, CV_BGR2YCrCb);
        for (int i = 0; i < src_img.cols; ++i) {
            for (int j = 0; j < src_img.rows; ++j) {
                Vec3b ycrcb = yuv.at<Vec3b>(j, i);
                if (skin_model.at<uchar>(ycrcb[1], ycrcb[2]) > 0)
                    mask.at<uchar>(j, i) = 255;
            }
        }
        src_img.copyTo(src_img, mask);
    }

    void Run() {
        if (InitSkinModel() == false)
            return;
        if (StartCamera() == false)
            return;
        if (GenerateBackground() == false)
            return;
        while (GetNextFrame() == true) {
            SkinExtract();
            Display();
            char key = (char)waitKey(1);
            if (key == 'q' || key == 'Q' || key == 27)
                break;
        }
        StopCamera();
        return;
    }
private:
    VideoCapture *capture;
    Mat src_img;

    //ellipse-skin-model
    Mat skin_model, mask;

    //background frame
    Mat background, pre_frame;
};




int main() {
    Tracker tracker;
    tracker.Run();
    return 0;
}
