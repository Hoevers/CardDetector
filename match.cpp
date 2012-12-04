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

	if( argc <= 1 )
	{
		fprintf(stderr, "Usage: %s <template> <image>\n", argv[0]);
		exit(-1);
	}

	IplImage *template_src = cvLoadImage( argv[1], 1 );
	CvMat *template_enh = cvCreateMat( template_src->height, template_src->width, CV_8UC1 );
	CvMat *template_edges = cvCreateMat( template_src->height, template_src->width, CV_8UC1 );
	CvMat *template_out = cvCreateMat( template_src->height, template_src->width, CV_8UC3 );

	cvCvtColor( template_src, template_enh, CV_RGB2GRAY );
	cvCopy( template_src, template_out );

	cvSmooth( template_enh, template_enh, CV_GAUSSIAN, 5, 0, 0, 0 );
	cvThreshold( template_enh, template_enh, 200, 255, CV_THRESH_BINARY);
	//cvAdaptiveThreshold( template_enh, template_enh, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 121, 0);
	cvCanny( template_enh, template_edges, 150, 200 );

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

	cvDrawContours( template_out, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 1, 2, 8 ); 

	cvShowImage( "template", template_out );
	cvMoveWindow( "template", 0, 0 );
	
	cvShowImage( "template_enh", template_enh );
	cvMoveWindow( "template_enh", template_enh->width, 0 );

	cvShowImage( "template_edges", template_edges );
	cvMoveWindow( "template_edges", template_edges->width*2, 0 );

	cvWaitKey( 0 );

	return 0;
}
