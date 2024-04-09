#include <opencv2\opencv.hpp> 
#include <iostream>

using namespace std;
using namespace cv;

cv::Mat mask_of_image(cv::Mat image)
{
    int height = image.rows;
    int width = image.cols;
    cv::Mat mask(image.size(), CV_8UC1, cv::Scalar(0));

    vector<Point>pts;
    Point ptA((width / 8) * 2, (height / 20) * 19);
    Point ptB((width / 8) * 2, (height / 8) * 7);
    Point ptC((width / 10) * 4, (height / 5) * 3);
    Point ptD((width / 10) * 5, (height / 5) * 3);
    Point ptE((width / 8) * 7, (height / 8) * 7);
    Point ptF((width / 8) * 7, (height / 20) * 19);
    pts = { ptA ,ptB,ptC,ptD,ptE, ptF };

    cv::fillPoly(mask, pts, cv::Scalar(255));

    cv::Mat mask_image;

    cv::bitwise_and(image, image, mask_image, mask);

    return mask_image;
}

cv::Mat DetectRoadLine(Mat srcImg)
{

    //添加蒙版
    cv::Mat gray_img;
    gray_img = mask_of_image(srcImg);

    //cv::imshow("img", srcImg);
    //转灰度图
    cvtColor(gray_img, gray_img, COLOR_BGR2GRAY);
    ////二值化
    cv::Mat threshold_img;
    cv::threshold(gray_img, threshold_img, 180, 255, cv::THRESH_BINARY);
    ////高斯模糊
    cv::Mat gauss_img;
    cv::GaussianBlur(threshold_img, gauss_img, Size(3, 3), 3);
    ////边缘检测
    cv::Mat canny_img;
    cv::Canny(gauss_img, canny_img, 180, 255);
    cv::imshow("canny_img", canny_img);

    ////霍夫直线检测
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(canny_img, lines, 1., CV_PI / 180, 30);

    ////画线
    for (int i = 0; i < lines.size(); ++i) {
        cv::Vec4i line_ = lines[i];
        cv::line(srcImg, cv::Point(line_[0], line_[1]), cv::Point(line_[2], line_[3]), cv::Scalar(0, 255, 0), 2, LINE_AA);
    }
    ////画车道
    vector<Point>left_line;
    vector<Point>right_line;

    //左车道线
    for (int i = 0; i < canny_img.cols / 2; i++)
    {
        for (int j = canny_img.rows / 2; j < canny_img.rows; j++)
        {
            if (canny_img.at<uchar>(j, i) == 255)
            {
                left_line.push_back(Point(i, j));
            }
        }
    }
    //右车道线
    for (int i = canny_img.cols / 2; i < threshold_img.cols; i++)
    {
        for (int j = canny_img.rows / 2; j < canny_img.rows; j++)
        {
            if (canny_img.at<uchar>(j, i) == 255)
            {
                right_line.push_back(Point(i, j));
            }
        }
    }

    //车道绘制
    if (left_line.size() > 0 && right_line.size() > 0)
    {
        Point B_L = (left_line[0]);
        Point T_L = (left_line[left_line.size() - 1]);
        Point T_R = (right_line[0]);
        Point B_R = (right_line[right_line.size() - 1]);

        circle(srcImg, B_L, 10, Scalar(0, 0, 255), -1);
        circle(srcImg, T_L, 10, Scalar(0, 255, 0), -1);
        circle(srcImg, T_R, 10, Scalar(255, 0, 0), -1);
        circle(srcImg, B_R, 10, Scalar(0, 255, 255), -1);

        line(srcImg, Point(B_L), Point(T_L), Scalar(0, 255, 0), 10);
        line(srcImg, Point(T_R), Point(B_R), Scalar(0, 255, 0), 10);

        vector<Point>pts;
        pts = { B_L ,T_L ,T_R ,B_R };
        vector<vector<Point>>ppts;
        ppts.push_back(pts);
        fillPoly(srcImg, ppts, Scalar(133, 230, 238));
    }


    //cv::imshow("img", srcImg);
    return srcImg;
}


int main()
{
    // 读取图像文件
    //std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    //// 读取图片
    //Mat srcImg = imread("D:\\test\\chedao.jpg");//自己的图片位置信息
    //if (srcImg.empty()) {
    //    cout << "could not load image..." << endl;
    //    return -1;
    //}


    //waitKey(0);

    //return 0;

    VideoCapture capture;
    capture.open("D:\\test\\project_video.mp4");

    if (!capture.isOpened())
    {
        cout << "Can not open video file!" << endl;
        system("pause");
        return -1;
    }

    Mat frame, image;
    while (capture.read(frame))
    {
        char key = waitKey(10);
        if (key == 27)
        {
            break;
        }

        DetectRoadLine(frame);

        imshow("frame", frame);
    }

    capture.release();
    destroyAllWindows();
    system("pause");
    return 0;


}

