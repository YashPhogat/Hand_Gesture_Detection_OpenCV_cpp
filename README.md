# Hand_Gesture_Detection_OpenCV_cpp

Steps involved in the project:

1. Capture Video from webcam (to allow web cam access, you might need the info.plist file)
2. Since you are reading the webcam input frame by frame, initiate an infinite loop to capture the frames and break the loop on some keypress.
3. Now you are continuosly reading your video frame by frame.
4. Flip the frame along y-axis to avoid adjusting your movements in an inverse fashion.
5. Define a region of interest in the frame to avoid processing extra content. For example, you only want to process hand actions but your frame might have your face in it as well. 
6. Convert the BGR(channel) colors to the HSV channels for this region of interest
7. Derive a mask for this HSV region of interest filtering on the hsv ranges for the color of skin.
8. Dilate the mask - Dilation increases the white region in the image or size of foreground object increases
9. Gaussian Blur - Convolving the image by Gaussian kernel. Reduces Noise. It actually removes high frequency content (eg: noise, edges) from the image.
10. Create contours of the mask. Contours are helpful outlining your object.
11. Extract the maximum area contour, usually the object in consideration - hand. This prevents us from working on some other less useful elements in the frame which may have qualified the previous filtering techniques.
12. Approximate the contour borders to smoothen the contour boundaries.
13. Create a Convex hull of this contour. It will be a convex hull of your hand.
14. Find the area of the convex hull, which you will use along with the region of interest area to determine the hand gesture values in tie-breaking situations.
15. Identify the Convexity Defects in the convex hull. Since our fingers form convex angles when usually spread, so we attempt to identify these convex angles in our hull.
16. Use our finger-tips and trench between fingers as three vertices of a triangle, to find the values of triangle edges.
17. Using these edge length in the cosine rule, find the angle between any two fingers.
18. If these angles are less than 90 i.e. Convex, count that as a trench.
19. Based on the final number of trenches at the end, predict the value of fingers as trenches+1 (usually, not always - sometimes we also use the tie breaking ratio of hull area and region of interest area.
20. Display the frame.
