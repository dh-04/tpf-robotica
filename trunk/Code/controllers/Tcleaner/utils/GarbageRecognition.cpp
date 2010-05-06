#include "GarbageRecognition.h"

#include <highgui.h>
#include <cv.h>
#include <vector>
#include <stdio.h>
#include <string>
#include "Contours.h"
#include "Histogram.h"
#include "Garbage.h"
#include "MinimalBoundingRectangle.h"

// image preprocessing values
#define THRESHOLD_VALUE 240
#define MORPH_KERNEL_SIZE 1
#define MORPH_DILATE_ITER 2
#define MORPH_ERODE_ITER 1

#define _RED  cvScalar (0, 0, 255, 0)
#define _GREEN cvScalar (0, 255, 0, 0)

//contour filters constants
#define MINCONTOUR_AREA 1000
#define MAXCONTOUR_AREA 10000
#define BOXFILTER_TOLERANCE 0.55
#define MINCONTOUR_PERIMETER 80
#define MAXCONTOUR_PERIMETER 1000
#define CONTOUR_RECTANGULAR_MIN_RATIO 1.2
#define CONTOUR_RECTANGULAR_MAX_RATIO 3.0
#define HIST_S_BINS 8
#define HIST_H_BINS 8
#define HIST_MIN 0.7
#define TIME_THRESHOLD 15 //seconds
/*
int main(void)
{
	utils::GarbageRecognition * gr= new utils::GarbageRecognition();
	
	IplImage * src = cvLoadImage("./colilla-scene.png",1);
	gr->getGarbageList(src);
	
}
*/
namespace utils {



std::list<utils::Garbage*> garbages;

std::list<Garbage*> 
GarbageRecognition::getGarbageList(IplImage * src)
{
	    IplImage * model = cvLoadImage("./colilla-sinBlanco.png",1);
		garbages = this->garbageList(src,model);
	
		cvReleaseImage(&model);
	return garbages;
}

std::list<Garbage*> 
GarbageRecognition::garbageList(IplImage * src, IplImage * model){

                    
    
	std::list<Garbage*> garbageList;

	//cvNamedWindow("output",CV_WINDOW_AUTOSIZE);
	//object model

	//image for the histogram-based filter
	//could be a parameter
	
	cvNamedWindow("andImage",CV_WINDOW_AUTOSIZE);

	utils::Histogram * h = new Histogram(HIST_H_BINS,HIST_S_BINS);
	CvHistogram * testImageHistogram = h->getHShistogramFromRGB(model);

	//~ int frameWidth=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH);
	//~ int frameHeight=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT);



	//gets a frame for setting  image size
	//CvSize srcSize = cvSize(frameWidth,frameHeight);
	CvSize srcSize = cvGetSize(src);

	//images for HSV conversion
	IplImage* hsv = cvCreateImage( srcSize, 8, 3 );
	IplImage* h_plane = cvCreateImage( srcSize, 8, 1 );
	IplImage* s_plane = cvCreateImage( srcSize, 8, 1 );
	IplImage* v_plane = cvCreateImage( srcSize, 8, 1 );


	//Image for thresholding
	IplImage * andImage=cvCreateImage(srcSize,8,1);

	//Image for thresholding
	IplImage * threshImage=cvCreateImage(srcSize,8,1);

	//image for equalization
	IplImage * equalizedImage=cvCreateImage(srcSize,8,1);

	//image for Morphing operations(Dilate-erode)
	IplImage * morphImage=cvCreateImage(srcSize,8,1);

	//image for image smoothing
	IplImage * smoothImage=cvCreateImage(srcSize,8,1);
	
	//image for contour-finding operations
	IplImage * contourImage=cvCreateImage(srcSize,8,3);
	

	int frameCounter=1;
	int cont_index=0;

	//convolution kernel for morph operations
	IplConvKernel* element;

	CvRect boundingRect;

	//contours
	CvSeq * contours;

	//Main loop


	//convert image to hsv
	cvCvtColor( src, hsv, CV_BGR2HSV );
	cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
	
	
	/*I(x,y)blue ~ ((uchar*)(img->imageData + img->widthStep*y))[x*3]
	I(x,y)green ~ ((uchar*)(img->imageData + img->widthStep*y))[x*3+1]
	I(x,y)red ~ ((uchar*)(img->imageData + img->widthStep*y))[x*3+2]*/
	
	for(int x=0;x<640;x++){
		for(int y=0;y<480;y++){
			uchar * hue=&((uchar*) (h_plane->imageData+h_plane->widthStep*y))[x];
			uchar * sat=&((uchar*) (s_plane->imageData+s_plane->widthStep*y))[x];
			uchar * val=&((uchar*) (v_plane->imageData+v_plane->widthStep*y))[x];
			if(*hue>20 && *hue<40)// && *sat<180 && *val<180 )
				*hue=150;
			else
				*hue=0;
			
		}
	}
	cvAnd(h_plane, v_plane, andImage);
	cvShowImage("andImage",andImage);
	
	//apply smooth gaussian-filter
	//~ cvSmooth(andImage,smoothImage,CV_GAUSSIAN,3,0,0,0);
	//~ cvNamedWindow("smoothImage",CV_WINDOW_AUTOSIZE);
	//~ cvShowImage("smoothImage",smoothImage);
	
	
	//cvEqualizeHist(threshImage,equalizedImage);
	cvThreshold(andImage,threshImage,140,255,CV_THRESH_BINARY);
	cvNamedWindow("threshImage",CV_WINDOW_AUTOSIZE);
	cvShowImage("threshImage",threshImage);
	
	//apply morphologic operations
	element = cvCreateStructuringElementEx( MORPH_KERNEL_SIZE*2+1,
		MORPH_KERNEL_SIZE*2+1, MORPH_KERNEL_SIZE, MORPH_KERNEL_SIZE,
		CV_SHAPE_RECT, NULL);

	
	cvDilate(threshImage,morphImage,element,MORPH_DILATE_ITER);
	cvErode(morphImage,morphImage,element,MORPH_ERODE_ITER);
	
	cvNamedWindow("morphImage",CV_WINDOW_AUTOSIZE);
	cvShowImage("morphImage",morphImage);
	
	
	
	
	//get all contours
	contours=myFindContours(morphImage);


	cont_index=0;
	
	cvCopy(src,contourImage,0);
	


	while(contours!=NULL){

		CvSeq * aContour=getPolygon(contours);
		utils::Contours * ct = new Contours(aContour);

	


		//apply filters

    
		if( ct->perimeterFilter(MINCONTOUR_PERIMETER,MAXCONTOUR_PERIMETER) && 
			ct->areaFilter(MINCONTOUR_AREA,MAXCONTOUR_AREA) &&
			//ct->rectangularAspectFilter(CONTOUR_RECTANGULAR_MIN_RATIO, CONTOUR_RECTANGULAR_MAX_RATIO) && 
			ct->boxAreaFilter(BOXFILTER_TOLERANCE) && 	
			1){//ct->histogramMatchingFilter(src,testImageHistogram, HIST_H_BINS,HIST_S_BINS,HIST_MIN)){
				
				//if passed filters
				ct->printContour(3,cvScalar(127,127,0,0),
					contourImage);
				
				//get contour bounding box
				boundingRect=cvBoundingRect(ct->getContour(),0);
				cvRectangle(contourImage,cvPoint(boundingRect.x,boundingRect.y),
						cvPoint(boundingRect.x+boundingRect.width,
						boundingRect.y+boundingRect.height),
						_GREEN,1,8,0);
				//build garbage List
			
				//printf(" c %d,%d\n",boundingRect.x,boundingRect.y);

				utils::MinimalBoundingRectangle * r = new utils::MinimalBoundingRectangle(boundingRect.x,
					boundingRect.y,boundingRect.width,boundingRect.height);

				utils::Garbage * aGarbage = new utils::Garbage(r);

				garbageList.push_back(aGarbage);


			}

		delete ct;
		cvReleaseMemStorage( &aContour->storage );
		contours=contours->h_next;
		cont_index++;
	}

   // cvShowImage("output",contourImage);
   // cvWaitKey(0);
	delete h;
	
	
	cvReleaseHist(&testImageHistogram);
	//Image for thresholding
	//cvReleaseMemStorage( &contours->storage );
	cvReleaseImage(&threshImage);
	cvReleaseImage(&equalizedImage);
	cvReleaseImage(&morphImage);
	cvReleaseImage(&smoothImage);
	cvReleaseImage(&contourImage);
	
	cvReleaseImage(&hsv);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvReleaseImage(&andImage);

	return garbageList;
}

} /* End of namespace utils */
