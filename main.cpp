//
//  main.cpp
//  CV_project1
//
//  Created by Yash Phogat on 8/1/20.
//  Copyright © 2020 Yash Phogat. All rights reserved.
//
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <exception>
#include <cmath>


using namespace std;
using namespace cv;

RNG rng(12345);

int main(int argc, const char * argv[]) {
    // insert code here...
        Mat frame;
          //--- INITIALIZE VIDEOCAPTURE
          VideoCapture cap;
          // open the default camera using default API
          // cap.open(0);
          // OR advance usage: select any API backend
          int deviceID = 0;             // 0 = open default camera
          int apiID = cv::CAP_ANY;      // 0 = autodetect default API
          // open selected camera using selected API
          cap.open(deviceID, apiID);
          // check if we succeeded
          if (!cap.isOpened()) {
              cerr << "ERROR! Unable to open camera\n";
              return -1;
          }
          //--- GRAB AND WRITE LOOP
          cout << "Start grabbing" << endl
              << "Press any key to terminate" << endl;
          for (;;)
          {
              // wait for a new frame from camera and store it into 'frame'
              cap.read(frame);
              // check if we succeeded
              if (frame.empty()) {
                  cerr << "ERROR! blank frame grabbed\n";
                  break;
              }
              
              cv::flip(frame, frame, 1);  //flip frame to work similar to your actions, 1 for flipping across y axix, 0 for flipping across x-axis
              
              Rect region_of_interest(700, 50, 500, 600);   //create a region of interest for execution  - x=700, y=50 (left corner)
                                                            // 500 width (x-length) and 600 height(y-length)
              rectangle(frame, region_of_interest, Scalar(255), 1, 8, 0); // build a rectangle
              
              Mat roi = frame(region_of_interest);
              Mat hsv_rectangle;
              cv::cvtColor(roi, hsv_rectangle, COLOR_BGR2HSV);
              
              Mat hsv_mask;
              cv::inRange(hsv_rectangle, cv::Scalar(0,20,70), cv::Scalar(20,255,255), hsv_mask);
              
              cv::dilate(hsv_mask, hsv_mask, Mat(), Point(-1, -1), 4);
              
              cv::GaussianBlur(hsv_mask, hsv_mask, Size(5,5), 100);
              
              vector<vector<Point> > get_contours;
              vector<Vec4i> hierarchy;
              
              cv::findContours(hsv_mask, get_contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
//              cout<<get_contours[2];
              
              if (get_contours.size()!=0){
                  double largest_area  = 0 ;
                  int largest_countour_index = 0;
                  
                  for (int i  = 0; i<get_contours.size(); i++){
                      double a = cv::contourArea(get_contours.at(i));
    //                  cout<<a<<"\n";
                      if(a>largest_area){
                          largest_area = a;
                          largest_countour_index = i;
    //                      cout<<"New largest area:"<<i<<"--->"<<largest_area<<"\n\n\n\n";
                      }
                      
                  }
                      
                  
//                  Mat max_contour = Mat::zeros( hsv_mask.size(), CV_8UC3 );
//                  Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//                  cv::drawContours(max_contour, get_contours, largest_countour_index, cv::Scalar(100,100,100));
//                  imshow("Contour", max_contour);
                  
                  
                  double epsilon;
                  epsilon = 0.0005*cv::arcLength(get_contours.at(largest_countour_index), true);
                  
                  vector<Point> approx_curve;
                  cv::approxPolyDP(get_contours[largest_countour_index], approx_curve, epsilon, true);
                  
                  
                  vector<Point> convex_hull;
                  cv::convexHull(get_contours[largest_countour_index], convex_hull);
                  
                  double hull_area;
                  hull_area = cv::contourArea(convex_hull);
                  
                  double object_area;
                  object_area = cv::contourArea(get_contours[largest_countour_index]);
                  
                  
                  double area_ratio;
                  area_ratio = (hull_area - object_area)/object_area;
                  
                  vector<int> defects_hull;
                  cv::convexHull(approx_curve, defects_hull, false, false);
                  
                  vector<Vec4i> defects;
                cv:convexityDefects(approx_curve, defects_hull, defects);
                  
                  //                  for (auto def: defects)
                  //                    cout<<def<<"\n";
                  
                  
                  int number_of_defects=0;
                  
                      for(const Vec4i& v : defects)
                      {
                        int startidx = v[0];
//                          cout<<startidx<<"-->"<<approx_curve[startidx]<<"\n";
                        Point ptStart(approx_curve[startidx]);
//                          cout<<ptStart.x<<ptStart.y;
                        int endidx = v[1];
                        Point ptEnd(approx_curve[endidx]);
                        int faridx = v[2];
                        Point ptFar(approx_curve[faridx]);
                        float depth = v[3];
                          
                        //building triangle
                          
                          double ab, bc, ca;
                          ab = sqrt(pow(ptEnd.x - ptStart.x, 2) + pow(ptEnd.y - ptStart.y, 2));
                          bc = sqrt(pow(ptFar.x - ptStart.x, 2) + pow(ptFar.y - ptStart.y, 2));
                          ca = sqrt(pow(ptEnd.x - ptFar.x, 2) + pow(ptEnd.y - ptFar.y, 2));
                          
                          double side, area;
                          side = (ab + bc + ca)/2;
                          area = sqrt(side*(side-ab)*(side-bc)*(side-ca));
                          
                          double angle_between_fingers;
                          angle_between_fingers = acos((pow(bc,2)+pow(ca, 2) - pow(ab, 2))/(2*(bc*ca)));
                          cout<<angle_between_fingers<<"-->";
                          angle_between_fingers = (angle_between_fingers * 180 * 7)/ 22;
                          
                          
                          if (angle_between_fingers<=90 && depth>30){
                              cout<<"Defect Found";
                              number_of_defects++;
                              cv::circle(roi, ptFar, 1, cv::Scalar(255,0,0));
                          }
                          cv::line(roi, ptStart, ptEnd, cv::Scalar(0,255,0));
                          
                      }
                  number_of_defects++;
                  
                  cout<<number_of_defects<<"\n";
                  
                  if (number_of_defects == 1){
                      if(object_area<2000){
                          cv::putText(frame, "Move hand inside the box", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 20, cv::Scalar(0,0,255));
                          
                      }
                      else{
                          if (area_ratio<12){
                            cv::putText(frame, "0", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                          }
                          else if (area_ratio<17.5){
                          cv::putText(frame, "Good Luck", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                          }
                          else{
                           cv::putText(frame, "1", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                          }
                      }
                  }
                  else if (number_of_defects==2){
                      cv::putText(frame, "2", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                  }
                  else if (number_of_defects == 3){
                      if(area_ratio<27){
                          cv::putText(frame, "3", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                          
                      }
                      else{
                          cv::putText(frame, "OK", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                      }
                  }
                  else if (number_of_defects==4){
                      cv::putText(frame, "4", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                  }
                  else if (number_of_defects==5){
                      cv::putText(frame, "5", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                  }
                  else {
                      cv::putText(frame, "Reposition", Point(0,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,0,255));
                  }

                  
              }
              
              
              
              
              
              imshow("Mask", hsv_mask);
              imshow("Live", frame);
              if (waitKey(5) >= 0)
                  break;
          }
          // the camera will be deinitialized automatically in VideoCapture destructor
          return 0;
}


