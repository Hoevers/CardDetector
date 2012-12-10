#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

int
main(
	int argc,
	char **argv )
{
	int i;

	/*if( argc <= 1 )
	{
		fprintf(stderr, "Usage: %s <template> <image>\n", argv[0]);
		exit(-1);
	}*/

	CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
	if ( !capture ) {
		fprintf( stderr, "ERROR: capture is NULL \n" );
		getchar();
		return -1;
	}	
	
	
	  while ( 1 ) {

     IplImage* template_src = cvQueryFrame( capture );
     if ( !template_src ) {
       fprintf( stderr, "ERROR: frame is null...\n" );
       getchar();
       break;
     }
   
	CvMat *template_enh = cvCreateMat( template_src->height, template_src->width, CV_8UC1 );
	CvMat *template_edges = cvCreateMat( template_src->height, template_src->width, CV_8UC1 );
	CvMat *template_out = cvCreateMat( template_src->height, template_src->width, CV_8UC3 );

	cvCvtColor( template_src, template_enh, CV_RGB2GRAY );
	cvCopy( template_src, template_out );

	cvSmooth( template_enh, template_enh, CV_GAUSSIAN, 5, 0, 0, 0 );
	//cvThreshold( template_enh, template_enh, 200, 255, CV_THRESH_BINARY);
	cvAdaptiveThreshold( template_enh, template_enh, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 121, 0);
	IplConvKernel *kernel = cvCreateStructuringElementEx( 3, 3, 0, 0, CV_SHAPE_RECT, NULL );
//	cvErode( template_enh, template_enh, kernel, 1 );
	cvCanny( template_enh, template_edges, 170, 200 );

	CvMemStorage* storage = cvCreateMemStorage();;
	CvSeq *contours = NULL;

	int val = cvFindContours(
		template_edges,
		storage,
		&contours,
		sizeof(CvContour),
		//CV_CHAIN_APPROX_SIMPLE
		CV_RETR_EXTERNAL
	);

	cvDrawContours( template_out, contours, CV_RGB(127,255,127), CV_RGB(0,255,0), 1, 1, 8 ); 

	CvSeq *contour = NULL;
	unsigned int id = 0;
	for( contour = contours; contour != NULL; contour = contour->h_next )
	{
		printf("[%u] Area: %f\n", id, cvContourArea( contour, CV_WHOLE_SEQ ) );
		CvRect bounding = cvBoundingRect( contour, 0 );

		cvRectangle( template_out, cvPoint(bounding.x, bounding.y), cvPoint( bounding.x + bounding.width, bounding.y + bounding.height ), CV_RGB(255, 0, 0), 2, 8, 0);

		id++;
	}

	char buffer[256] = { 0 };

	CvFont font;
	cvRectangle( template_out, cvPoint(0, 0), cvPoint( 640 , 20 ), CV_RGB(127, 0, 0), CV_FILLED);
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, 0.8, 0.2, 1, 4);
	snprintf(buffer, sizeof(buffer), "Found %d contours", val );
	cvPutText(template_out, buffer, cvPoint( 1, 15 ), &font, CV_RGB(255,255,255));

	cvShowImage( "template", template_out );
	cvMoveWindow( "template", 0, 0 );
	
	cvShowImage( "template_enh", template_enh );
	cvMoveWindow( "template_enh", template_enh->width, 0 );

	cvShowImage( "template_edges", template_edges );
	cvMoveWindow( "template_edges", template_edges->width*2, 0 );
	
	cvReleaseMat( &template_enh );
	//cvReleaseImageMat( &template_enh );
	cvReleaseMat( &template_edges );
	//cvReleaseMatHeader( &template_edges );
	cvReleaseMat( &template_out );
	//cvReleaseImageHeader( &template_out );
	
	cvClearMemStorage(storage);
	cvReleaseMemStorage(&storage);

	if ( (cvWaitKey(10) & 255) == 27 ) break;
	}
	return 0;
}
