#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <iostream>

using namespace cv;

int main()
{
    // Open the video camera.
    std::string pipeline = "libcamerasrc"
        " ! video/x-raw, width=800, height=600" // camera needs to capture at a higher resolution
        " ! videoconvert"
        " ! videoscale"
        " ! video/x-raw, width=400, height=300" // can downsample the image after capturing
        " ! videoflip method=rotate-180" // remove this line if the image is upside-down
        " ! appsink drop=true max_buffers=2";
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    if(!cap.isOpened()) {
        printf("Could not open camera.\n");
        return 1;
    }

    // Convert to HSV colour space
	cvtColor(display_img, hsv_img, COLOR_BGR2HSV);

	// Create a control window
	namedWindow("Control", WINDOW_AUTOSIZE);
	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	// Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	// Create the display windows
	namedWindow("Display", WINDOW_AUTOSIZE);
	imshow("Display", bgr_img); //show the original image
	namedWindow("Thresholded", WINDOW_AUTOSIZE);
    namedWindow("Sobel_filter", WINDOW_AUTOSIZE);
    namedWindow("Sharpening", WINDOW_AUTOSIZE);
    
    // Moments in OpenCV
    Moments moments(InputArray array, bool binaryImage);

    Moments m = moments(thresh_img, true);
    if (m.m00 > 0) {
        double x = m.m10 / m.m00;
    }
    // Create the OpenCV window
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;
    cv::Mat thresh_img;
	cv::Mat display_img = bgr_img.clone(); // deep copy because we will modify it below
    
    // Measure the frame rate - initialise variables
    int frame_id = 0;
    timeval start, end;
    gettimeofday(&start, NULL);

    for(;;) {
        if (!cap.read(frame)) {
            printf("Could not read a frame.\n");
            break;
        }

        //show frame
        cv::imshow("Camera", frame);
        cv::waitKey(1);

		// Threshold the image
		inRange(hsv_img, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), thresh_img);

		// Show the thresholded image
		imshow("Thresholded", thresh_img);

		// Draw the final image
		imshow("Display", display_img);

		// Allow openCV to process GUI events
		waitKey(100);

        // Measure the frame rate
        frame_id++;
        if (frame_id >= 30) {
            gettimeofday(&end, NULL);
            double diff = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
            printf("30 frames in %f seconds = %f FPS\n", diff, 30/diff);
            frame_id = 0;
            gettimeofday(&start, NULL);
        }
    }


	// Display the result of the current calibration settings



    // Free the camera 
    cap.release();
    return 0;
}

