#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WINDOWS
#	include <direct.h>
#	define getcwd _getcwd
#else
#	include <unistd.h>
#endif

#include <opencv/cv.h>
#include <opencv/highgui.h>

int
main(
        int argc,
        char **argv )
{
	int i = 0;
	char cwd[FILENAME_MAX+1] = { 0 };

	getcwd( cwd, FILENAME_MAX+1 );
	card_init( cwd );

	CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
	if ( !capture )
	{
		fprintf( stderr, "ERROR: capture is NULL \n" );
		getchar();
		return -1;
	}

	while ( 1 )
	{

		IplImage* template_src = cvQueryFrame( capture );
		if ( !template_src )
		{
			fprintf( stderr, "ERROR: frame is null...\n" );
			getchar();
			break;
		}

		CvMat *template_enh = cvCreateMat( template_src->height, template_src->width, CV_8UC1 );
		CvMat *template_edges = cvCreateMat( template_src->height, template_src->width, CV_8UC1 );
		CvMat *template_out = cvCreateMat( template_src->height, template_src->width, CV_8UC3 );
		CvMat *template_out_rescale = cvCreateMat( template_src->height/4, template_src->width/4, CV_8UC3 );

		cvCvtColor( template_src, template_enh, CV_RGB2GRAY );
		cvCopy( template_src, template_out, NULL );

		cvSmooth( template_enh, template_enh, CV_GAUSSIAN, 3, 0, 0, 0 );
		cvThreshold( template_enh, template_enh, 200, 255, CV_THRESH_BINARY);
		//cvAdaptiveThreshold( template_enh, template_enh, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 121, 0);
		IplConvKernel *kernel = cvCreateStructuringElementEx( 3, 3, 0, 0, CV_SHAPE_RECT, NULL );
		/* cvErode( template_enh, template_enh, kernel, 1 ); */
		cvCanny( template_enh, template_edges, 170, 200, 3 );

		CvMemStorage* storage = cvCreateMemStorage(0);;
		CvSeq *contours = NULL;

		int val = cvFindContours(
		        template_edges,
		        storage,
		        &contours,
		        sizeof(CvContour),
		        CV_RETR_EXTERNAL,
				CV_CHAIN_APPROX_SIMPLE,
				cvPoint(0, 0)
		        );

/*
		cvDrawContours( 
			template_out,
			contours,
			CV_RGB(127,255,127),
			CV_RGB(0,255,0),
			1, 5, 8, cvPoint(0, 0) );
*/

		CvSeq *contour = NULL;
		unsigned int id = 0;
		for( contour = contours; contour != NULL; contour = contour->h_next )
		{
			double area =  cvContourArea( contour, CV_WHOLE_SEQ, 0 );
			if( area > 15000 )
			{ 
				CvRect bounding = cvBoundingRect( contour, 0 );
				printf("[%u] Area: %f\n", id,  area);
				cvRectangle( template_out, cvPoint(bounding.x, bounding.y), cvPoint( bounding.x + bounding.width, bounding.y + bounding.height ), CV_RGB(255, 0, 0), 5, 8, 0);

				printf("bounding width: %d, height: %d\n", bounding.width, bounding.height);

				/* Create card ROI */
				CvMat *card_roi = cvCreateMat( bounding.height, bounding.width, CV_8UC1 );
				CvMat *card_roi_out = cvCreateMat( bounding.height, bounding.width, CV_8UC3 );
				CvMat *card_edges = cvCreateMat( bounding.height, bounding.width, CV_8UC1 );
				card_roi = cvGetSubRect( template_enh, card_roi, bounding );

				cvCvtColor( card_roi, card_roi_out, CV_GRAY2BGR);
				

			CvMemStorage* card_storage = cvCreateMemStorage(0);;
			CvSeq *card_contours = NULL;

			cvCanny( card_roi, card_edges, 170, 200, 3 );
			int val = cvFindContours(
		   	     card_edges,
		   	     card_storage,
		    	    &card_contours,
		        sizeof(CvContour),
		        CV_RETR_EXTERNAL,
				CV_CHAIN_APPROX_SIMPLE,
				cvPoint(0, 0)
		        );

		cvDrawContours( 
			card_roi_out,
			card_contours,
			CV_RGB(127,255,127),
			CV_RGB(0,255,0),
			1, 1, 8, cvPoint(0, 0) );

				/* Show and release */
				cvShowImage( "card_roi_out", card_roi_out );
				cvMoveWindow( "card_roi_out", 0, template_src->width/4 );

				//cvShowImage( "card_edges", card_edges );
				//cvMoveWindow( "card_edges", 100, template_src->width/4 );

				cvReleaseMat( &card_roi );
				cvReleaseMat( &card_roi_out );
				cvReleaseMat( &card_edges );

				cvClearMemStorage(card_storage);
				cvReleaseMemStorage(&card_storage);

				break;
			}
			id++;
		}

		char buffer[256] = { 0 };

		CvFont font;
		cvRectangle(
			template_out,
			cvPoint(0, 0),
			cvPoint(640, 20),
			CV_RGB(127, 0, 0),
			CV_FILLED, 8, 0);

		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, 0.8, 0.2, 1, 4);
		snprintf(buffer, sizeof(buffer), "Found %d contours", val );
		cvPutText(template_out, buffer, cvPoint( 1, 15 ), &font, CV_RGB(255,255,255));

		cvResize( template_out, template_out_rescale, CV_INTER_LINEAR );
		cvShowImage( "template", template_out_rescale );
		cvMoveWindow( "template", 0, 0 );

		//cvShowImage( "template_enh", template_enh );
		//cvMoveWindow( "template_enh", 480, 0 );

		//cvShowImage( "template_edges", template_edges );
		//cvMoveWindow( "template_edges", 480*2, 0 );

		cvReleaseMat( &template_enh );
		cvReleaseMat( &template_edges );
		cvReleaseMat( &template_out );
		cvReleaseMat( &template_out_rescale );



		cvClearMemStorage(storage);
		cvReleaseMemStorage(&storage);

		if ( (cvWaitKey(250) & 255) == 27 )
			break;
	}
	return 0;
}
