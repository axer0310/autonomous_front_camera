#include <stdio.h>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
int main( int argc, char** argv )
{
//    cout << "Hello World"<<endl;
    
    cv::VideoCapture vidcap = cv::VideoCapture("/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/0122/Original Image/0122-Right1.mp4");
    if(!vidcap.isOpened())
    {
        cout << " cant open video" << endl;
        return -1;
    }
    Mat frame;
    vidcap >> frame;
    while( frame.data != NULL)
    {
        Mat image;
//        image = imread("/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/0122/Original Image/0122-1.png", IMREAD_COLOR); // 讀取圖片
        //    image = imread("/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/barrier.jpg", IMREAD_COLOR); // 讀取圖片
        image = frame;
        if( image.empty() )   // Check for invalid input
        {
            std::cout << "Could not open or find the image" << std::endl;
            return -1;
        }
        
        cv::Mat hsv_image;
        cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);
        
        // Threshold the HSV image, keep only the red pixels
        cv::Mat lower_red_hue_range;
        cv::Mat upper_red_hue_range;
        cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
        cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);
        
        cv::Mat lower_white_hue_range;
        cv::Mat upper_white_hue_range;
        cv::inRange(hsv_image, cv::Scalar(0, 0, 180), cv::Scalar(180, 150, 255), lower_white_hue_range);
        //    cv::inRange(hsv_image, cv::Scalar(0, 255, 255), cv::Scalar(180, 255, 255), upper_white_hue_range);
        
        
        
        cv::Mat red_hue_image;
        cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
        
        cv::Mat combine;
        cv::addWeighted(red_hue_image, 1.0, lower_white_hue_range, 1.0, 0.0, combine);
        cv::Mat combine_Blur;
        cv::GaussianBlur(combine, combine_Blur, cv::Size(9, 9), 2, 2);
        
        
        
        cv::Point2f src_points[] = {
            cv::Point2f((combine.cols/5)*1, combine.rows),
            cv::Point2f((combine.cols/5)*4, combine.rows),
            cv::Point2f((combine.cols/6)*2, (combine.rows/2)),
            cv::Point2f((combine.cols/6)*4, (combine.rows/2))
            
            
        };
        
        
        cv::Point2f dst_points[] = {
            cv::Point2f((combine.cols/5)*1, combine.rows),
            cv::Point2f((combine.cols/5)*4, combine.rows),
            cv::Point2f((combine.cols/5)*1, 0),
            cv::Point2f((combine.cols/5)*4, 0)
            
        };
        
        cv::Mat M = cv::getPerspectiveTransform(src_points, dst_points);
        
        cv::Mat perspective;
        cv::warpPerspective(combine_Blur, perspective, M, cv::Size(combine.cols, combine.rows), cv::INTER_LINEAR);
        
        
        
        cv::Mat maskImg;
        cv::Mat mask = cv::Mat::zeros(combine.size(), combine.type());
        //    cv::Point wholePoints[4] ={
        //
        //        cv::Point(0, (combine.rows/2) ),
        //        //        cv::Point((combine.cols/5)*1, (combine.rows/2)*1),
        //        cv::Point(combine.cols, (combine.rows/2)),
        //        cv::Point((combine.cols), (combine.rows)),
        //        cv::Point(0, (combine.rows))
        //    };
        //
        
        cv::Point wholePoints[4] ={
            
            cv::Point(0, 0),
            //        cv::Point((combine.cols/5)*1, (combine.rows/2)*1),
            cv::Point(combine.cols, 0),
            cv::Point((combine.cols), (combine.rows)),
            cv::Point(0, (combine.rows))
        };
        
        
        cv::Point leftPts[4] = {
            cv::Point((combine.cols/6)*0, (combine.rows/3)*1 ),
            //        cv::Point((combine.cols/5)*1, (combine.rows/2)*1),
            cv::Point((combine.cols/6)*1, (combine.rows/3)*1),
            cv::Point((combine.cols/6)*1, (combine.rows/2)*2),
            cv::Point((combine.cols/6)*0, (combine.rows/2)*2)
        };
        
        cv::Point rightPts[4] = {
            cv::Point((combine.cols/6)*5, (combine.rows/3)*1 ),
            //        cv::Point((combine.cols/5)*1, (combine.rows/2)*1),
            cv::Point((combine.cols/6)*6, (combine.rows/3)*1),
            cv::Point((combine.cols/6)*6, (combine.rows/2)*2),
            cv::Point((combine.cols/6)*5, (combine.rows/2)*2)
        };
        
        
//        std::cout << combine.cols << std::endl;
//        std::cout << combine.rows << std::endl;
        // Create a binary polygon ma4sk
        //    cv::fillConvexPoly(mask, leftPts, 4, cv::Scalar(255, 0, 0));
        //    cv::fillConvexPoly(mask, rightPts, 4, cv::Scalar(255, 0, 0));
        
        cv::fillConvexPoly(mask, wholePoints, 4, cv::Scalar(255, 0, 0));
        
        
        // Multiply the edges image and the mask to get the output
        
        cv::bitwise_and(perspective, mask, maskImg);
        
        //    cv::Mat green(combine.rows, combine.cols, combine.channels(), Scalar(0,100, 0));
        
        //    cv::addWeighted(combine, 1.0, green, 1.0, 0.0, greenImg);
        
        std::vector<cv::Vec4i> lines;
        std::vector<int> selectedLines;
        std::vector<int> leftLines;
        std::vector<int> rightLines;
        
        
//        cv::GaussianBlur(maskImg, maskImg, cv::Size(9, 9), 2, 2);
        
        HoughLinesP(maskImg, lines, 1, CV_PI/180, 20, 20, 30);
        for(int i = 0 ; i< lines.size();i++)
        {
            //lines[i][0] = x coordinate of the init point of the line
            //lines[i][1] = y coordinate of the init point of the line
            //lines[i][2] = x coordinate of the last point of the line
            //lines[i][3] = y coordinate of the last point of the line
            
            cv::Point init = Point(lines[i][0], lines[i][1]);
            cv::Point last = Point(lines[i][2], lines[i][3]);
            double slope = (last.y - init.y)/(last.x - init.x + 0.00001);
            double dist = hypot(last.x - init.x, last.y - init.y);
            if(std::abs(slope) > 0.1 && dist > 20)
            {
                if(slope < 0)
                {
                    leftLines.push_back(i);
                }
                else if(slope > 0 )
                {
                    rightLines.push_back(i);
                }
                selectedLines.push_back(i);
            }
            
        }
//        std::cout << "lines size: " << lines.size() << std::endl;
//        std::cout << "selected lines size: " << selectedLines.size() << std::endl;
//        std::cout << "left lines size: " << leftLines.size() << std::endl;
//        std::cout << "right lines size: " << rightLines.size() << std::endl;
        
        
        
        Mat lineImg = cv::Mat::zeros(maskImg.size(), maskImg.type());
        for(int i =0;i< selectedLines.size();i++)
        {
            int count = selectedLines[i];
            
            
            
            
            
            cv::line(lineImg, Point(lines[count][0], lines[count][1]), Point(lines[count][2], lines[count][3]), Scalar(255,0,0), 3);
        }
        
        
        
//        namedWindow( "original window", WINDOW_NORMAL ); //建立顯示窗口
//        imshow( "original window", image); // 顯示圖片數據
    
        
        //
        //    namedWindow( "lower window", WINDOW_NORMAL ); //建立顯示窗口
        //    imshow( "lower window", lower_red_hue_range); // 顯示圖片數據
        //    resizeWindow("lower window", 600,600);
        //    namedWindow( "Higher window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "Higher window", upper_red_hue_range); // 顯示圖片數據
        //
        //    namedWindow( "Red window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "Red window", red_hue_image); // 顯示圖片數據
        //
        //    namedWindow( "lower white window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "lower white window", lower_white_hue_range); // 顯示圖片數據
        
//        namedWindow( "Combine window", WINDOW_AUTOSIZE ); //建立顯示窗口
//        imshow( "Combine window", combine); // 顯示圖片數據
//        imwrite( "/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/0122/0122-16_combine.png", combine );
//
//
//        namedWindow( "perspective window", WINDOW_AUTOSIZE ); //建立顯示窗口
//        imshow( "perspective window", perspective); // 顯示圖片數據
//
//            namedWindow( "Combine Blur window", WINDOW_NORMAL ); //建立顯示窗口
//            imshow( "Combine Blur window", combine_Blur); // 顯示圖片數據
//        //    imwrite( "/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/0122-1_combine_Blur.png", combine_Blur );
//        //
//        namedWindow( "Mask  window", WINDOW_AUTOSIZE ); //建立顯示窗口
//        imshow( "Mask window", maskImg); // 顯示圖片數據
//        //    imwrite( "/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/0122-1_Mask.png", maskImg );
//        //
//        namedWindow( "Line  window", WINDOW_AUTOSIZE ); //建立顯示窗口
//        imshow( "Line window", lineImg); // 顯示圖片數據
        //    imwrite( "/Users/arthur/Documents/UnderGrad/2018 Fall/GPEV/opencvTest/opencvTest/0122-1_line.png", lineImg );
        //
        //    namedWindow( "Higher white window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "Higher white window", upper_white_hue_range); // 顯示圖片數據
        
        
        
        
        //    Mat grayImg;
        //    Mat blurImg;
        //    Mat cannyImg;
        //    cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);
        //    blur(grayImg, blurImg, Size(3,3));
        //    cv::Canny(blurImg, cannyImg, 180, 255);
        //    std::vector<cv::Vec4i> lines;
        //    std::vector<int> selectedLines;
        //    std::vector<int> leftLines;
        //    std::vector<int> rightLines;
        //    // rho and theta are selected by trial and error
        //
        //
        //    cv::Mat maskImg;
        //    cv::Mat mask = cv::Mat::zeros(cannyImg.size(), cannyImg.type());
        //    cv::Point pts[4] = {
        //        cv::Point(0, (cannyImg.rows/3)*2),
        //        cv::Point(cannyImg.cols, (cannyImg.rows/3)*2),
        //        cv::Point(cannyImg.cols, (cannyImg.rows/3)*3),
        //        cv::Point(0, (cannyImg.rows/3)*3)
        //    };
        //
        //    std::cout << cannyImg.cols << std::endl;
        //    std::cout << cannyImg.rows << std::endl;
        //    // Create a binary polygon ma4sk
        //    cv::fillConvexPoly(mask, pts, 4, cv::Scalar(255, 0, 0));
        //    // Multiply the edges image and the mask to get the output
        //    cv::bitwise_and(cannyImg, mask, maskImg);
        //
        //
        //    HoughLinesP(maskImg, lines, 1, CV_PI/180, 20, 20, 30);
        //    for(int i = 0 ; i< lines.size();i++)
        //    {
        //        //lines[i][0] = x coordinate of the init point of the line
        //        //lines[i][1] = y coordinate of the init point of the line
        //        //lines[i][2] = x coordinate of the last point of the line
        //        //lines[i][3] = y coordinate of the last point of the line
        //
        //        cv::Point init = Point(lines[i][0], lines[i][1]);
        //        cv::Point last = Point(lines[i][2], lines[i][3]);
        //        double slope = (last.y - init.y)/(last.x - init.x + 0.00001);
        //        double dist = hypot(last.x - init.x, last.y - init.y);
        //        if(std::abs(slope) > 0.1 && dist > 20)
        //        {
        //            if(slope < 0)
        //            {
        //                leftLines.push_back(i);
        //            }
        //            else if(slope > 0 )
        //            {
        //                rightLines.push_back(i);
        //            }
        //            selectedLines.push_back(i);
        //        }
        //
        //    }
        //    std::cout << "lines size: " << lines.size() << std::endl;
        //    std::cout << "selected lines size: " << selectedLines.size() << std::endl;
        //    std::cout << "left lines size: " << leftLines.size() << std::endl;
        //    std::cout << "right lines size: " << rightLines.size() << std::endl;
        //
        //
        //
        //    Mat lineImg = cv::Mat::zeros(cannyImg.size(), cannyImg.type());
        //    for(int i =0;i< selectedLines.size();i++)
        //    {
        //        int count = selectedLines[i];
        //        cv::line(lineImg, Point(lines[count][0], lines[count][1]), Point(lines[count][2], lines[count][3]), Scalar(255,0,0), 3);
        //    }
        int result = -2;
        if(leftLines.size() > rightLines.size())
        {
            result = 1;
        }
        else if(leftLines.size() == rightLines.size())
        {
            result = 0;
        }
        else if(leftLines.size() < rightLines.size())
        {
            result = -1;
        }
        if(result == -2)
        {
            cout << "Result: can't determined" << endl;
        }
        else
        {
            switch (result)
            {
                case -1:
                    cout << "Result: "<< "Left turn" << endl;
                    break;
                case 0:
                    cout << "Result: "<< "stright" << endl;
                    break;
                case 1:
                    cout << "Result: "<< "right turn" << endl;
                    break;
                default:
                    cout << "Result: Error" << endl;
            }
            
        }
        
        //
        //    namedWindow( "Canny window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "Canny window", cannyImg); // 顯示圖片數據
        //    namedWindow( "line window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "line window", lineImg); // 顯示圖片數據
        //
        //    namedWindow( "Image window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "Image window", image); // 顯示圖片數據
        //
        //
        //    namedWindow( "Mask window", WINDOW_AUTOSIZE ); //建立顯示窗口
        //    imshow( "mask window", maskImg); // 顯示圖片數據

        
        vidcap >> frame;
//        waitKey(0); // 等待按鍵，當你按按鍵才會繼續下段程式碼
    }
    
    
    
    
    
    
    
    
    return 0;
}
