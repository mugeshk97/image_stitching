#include <opencv2/opencv.hpp>
#include <opencv2/stitching.hpp>
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;


int getMaxAreaContourId(vector <vector<cv::Point>> contours) {
    double maxArea = 0;
    int maxAreaContourId = -1;
    for (int j = 0; j < contours.size(); j++) {
        double newArea = contourArea(contours.at(j));
        if (newArea > maxArea) {
            maxArea = newArea;
            maxAreaContourId = j;
        } 
    }
    return maxAreaContourId;
}

int main(int argc, char* argv[]) {

    vector<Mat> images;

    for (int i = 1; i < argc; ++i){
        Mat img = imread(samples::findFile(argv[i]));

        if (img.empty()){
            cout << "Can't read image '" << argv[i] << "'\n";
            return EXIT_FAILURE;
        }
        else
            images.push_back(img);
    }

    chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    Mat panorama;

    Stitcher::Mode mode = Stitcher::PANORAMA;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(images, panorama);

    if (status != Stitcher::OK)
    {
        cout << "Can't stitch images, error code = " << int(status) << endl;
        return EXIT_FAILURE;
    }
    
    
    Mat src;
    Mat gray;
    Mat threshold_image;
    Mat mask;


    copyMakeBorder(panorama, src,  10, 10, 10, 10, BORDER_CONSTANT, (0,0,0));
    cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    threshold(gray, threshold_image, 0, 255, THRESH_BINARY);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(threshold_image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    int counid;
    counid = getMaxAreaContourId(contours);

    mask = Mat::zeros(threshold_image.size(), threshold_image.type());

    Rect mask_rec;

    mask_rec = boundingRect(contours[counid]);

    rectangle(mask,  Point(mask_rec.x, mask_rec.y), Point(mask_rec.x + mask_rec.width, mask_rec.y + mask_rec.height), 255, -1);

    Mat minRectangle = mask.clone();
    Mat sub = mask.clone();
    

    while(countNonZero(sub) > 0 ){
        erode(minRectangle, minRectangle, Mat());
        subtract(minRectangle, threshold_image, sub);
    };

    vector<vector<Point>> contours1;
    vector<Vec4i> hierarchy1;
    findContours(minRectangle.clone(), contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    

    int counid1;
    counid1 = getMaxAreaContourId(contours1);

    Rect mask_rec1;
    mask_rec1 = boundingRect(contours1[counid1]);


    Mat croppped = src(Range(mask_rec1.y, mask_rec1.y + mask_rec1.height), Range(mask_rec1.x, mask_rec1.x + mask_rec1.width));

    chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "[µs]" << endl;

    imshow("result",  croppped);
    waitKey(0);
    destroyAllWindows();

}