#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "BlobExtract.h"

int
main(
	int argc,
	char **argv )
{
	int i;
	BlobExtract extract;
	std::vector<BlobExtract::Blob> blobs;

	if( argc <= 1 )
	{
		fprintf(stderr, "Usage: %s <image>\n", argv[0]);
		exit(-1);
	}

	IplImage *img = cvLoadImage( argv[1], 1 );
	IplImage *img_out = cvCreateImage( cvGetSize( img ), 8, 3 );

	CvMat *out = cvCreateMat( img->height, img->width, CV_8UC1 );
	CvMat *out_edge = cvCreateMat( img->height, img->width, CV_8UC1 );
	CvMat *binary = cvCreateMat( img->height, img->width, CV_8UC1 );
	CvMat *color = cvCreateMat( img->height, img->width, CV_8UC3 );
	cvCvtColor( img, out, CV_RGB2GRAY );

	cvSmooth( out, out, CV_GAUSSIAN, 5, 0, 0, 0 );
	cvCanny( out, out_edge, 150, 200 );

	CvMemStorage* storage = cvCreateMemStorage();;
	CvSeq *contours = NULL;

	int Nc = cvFindContours(
		out_edge,
		storage,
		&contours,
		sizeof(CvContour),
		CV_CHAIN_APPROX_SIMPLE
		//CV_RETR_EXTERNAL // Try all four values and see what happens
	);

	cvCvtColor( out, color, CV_GRAY2RGB );

int n=0;
printf( "Total Contours Detected: %d\n", Nc );
for( CvSeq* c=contours; c!=NULL; c=c->h_next ) {
cvDrawContours(
color,
c,
cvScalar(0,0,255),
cvScalar(0),
0, // Try different values of max_level, and see what happens 2,
CV_FILLED );
printf("Contour #%d\n", n );
printf(" %d elements:\n", c->total );
for( int i=0; i<c->total; ++i ) {
CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
printf(" (%d,%d)\n", p->x, p->y ); }
n++; }

	while( cvWaitKey( 10 ) != 27 )
	{
		//cvCopy( img, out );
		//cvCircle( out, cvPoint(50,50), 20, cvScalar(255, 0, 0), 5, 8 );

		//extract.process( out, &blobs );

		for( i=0; i<(int)blobs.size(); ++i )
		{
			cvRectangle( color, blobs[i].point1(), blobs[i].point2(), CV_RGB(255,0,0) );
			cvCircle( color, blobs[i].com(), 2, CV_RGB(0,255,0), 5, 8 );
		}


//		cvShowImage( "out", out );
		cvShowImage( "color", color );
		cvMoveWindow( "color", 0, 0 );
		//cvShowImage( "binary", binary );
	}

	// cleanup
	cvDestroyAllWindows();

	cvReleaseImage( &img );
	cvReleaseImage( &img_out );

	return 0;
}
