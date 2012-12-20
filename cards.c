#include "cards.h"

#include <stdio.h>
#include <stdbool.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

/** Card strings for all types */
const char *
card_type_string[] =
{
	"joker",
	"hearts",
	"spades",
	"clubs",
	"diamonds",
	"jack",
	"queen",
	"king",
	NULL
};

/** Card templates contours */
CvSeq *card_contours[ sizeof(card_type_string) ];

/**
 * Initialise the card template engine
 * <directory>/cards/<card_type_string>.png
 * @param directory The directory where to search the templates
 */
int
card_init(const char *directory)
{
	enum card_type current  = CARD_JOKER;
	CvMat *current_image = NULL;
	char filename[256] = { 0 };
	FILE *file = NULL;

	/* Process every card template */
	for(current = CARD_JOKER; current < CARD_TYPE_END; current++)
	{
		snprintf(filename, sizeof(filename), "%s/cards/%s.png", directory, card_type_string[current]);
		printf("%s, ", filename);

		file = fopen(filename, "r");
		if( file == NULL )
		{
			printf("doesn't exist!\n");
			fclose(file);
			break;
			//exit(-1);
		}
		else
			printf("file exist :-)!\n");

		fclose(file);

		current_image = cvLoadImageM( filename, CV_LOAD_IMAGE_GRAYSCALE );
		if( current_image != NULL )
		{
			card_process( current_image, current );
			cvReleaseMat( &current_image );
		}
	}


	return 0;
}

void
card_cleanup(void)
{
	enum card_type current = CARD_JOKER;

	for(current = CARD_JOKER; current < CARD_TYPE_END; current++)
	{
		if( card_contours[current] != NULL )
		{
			cvClearSeq( card_contours[current] );
		}
	}
}

/**
 * Process the CvMat image as a card
 * @param image   The current image data pointer
 * @param current Current card type
 */
void
card_process(CvMat *image, enum card_type current)
{
	int contour_count     = 0;

	CvMat *edges          = NULL;
	CvSeq *contours       = NULL;
	CvMemStorage* storage = NULL;

	/* Check for valid current card */
	if( (image != NULL) && (current >= CARD_JOKER) && (current < CARD_TYPE_END) )
	{
		storage = cvCreateMemStorage(0); 
		edges   = cvCreateMat( image->rows, image->cols, image->type );

		cvSmooth( image, image, CV_GAUSSIAN, 3, 0, 0, 0 );
		cvThreshold( image, image, 200, 255, CV_THRESH_BINARY);

		/* Show card */
#if DEBUG
		cvShowImage( card_type_string[current], image );
		cvMoveWindow( card_type_string[current], 0, 0 );
#endif /* DEBUG */

		cvCanny( image, edges, 170, 200, 3 );

		/* Show edges */
#if DEBUG
		cvShowImage( "edges", edges );
		cvMoveWindow( "edges", edges->cols, 0 );
		while( cvWaitKey(0) != 32 )
			;
#endif /* DEBUG */

		/* Find contours */
		contour_count = cvFindContours(
				edges,
				storage,
				&contours,
				sizeof(CvContour),
				CV_RETR_EXTERNAL,
				CV_CHAIN_APPROX_SIMPLE,
				cvPoint(0, 0)
		);

		/* Save contours in card list */
		card_contours[ current ] = contours;

		/* Cleanup */
		cvReleaseMat( &edges );
		cvClearMemStorage(storage);
		cvReleaseMemStorage(&storage);
	}
}

/**
 * Try to detect a card from the learned list
 * @param card The binary enhanced image
 */
enum card_type
card_detect(CvMat *card_roi)
{

	int contour_count     = 0;
	bool card_found       = false;
	enum card_type card   = CARD_UNKNOWN;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq *contour        = NULL;
	CvSeq *contours       = NULL;
	CvMat *edges          = NULL;

	/* Detect edges and find */
	edges = cvCreateMat( card_roi->rows, card_roi->cols, card_roi->type );
	cvCanny( card_roi, edges, 170, 200, 3 );
	contour_count = cvFindContours(
        edges,
        storage,
        &contours,
        sizeof(CvContour),
        CV_RETR_LIST,
		CV_CHAIN_APPROX_SIMPLE,
		cvPoint(0, 0)
	);


	/* Compare contours */
	if( contour_count > 0)
	{
		for( contour = contours; contour != NULL; contour = contour->h_next )
		{
			cvDrawContours( 
				edges,
				contour,
				CV_RGB(255,255,255),
				CV_RGB(255,255,255),
				0,
				2,
				CV_FILLED, cvPoint(0, 0) );
#if DEBUG
				cvShowImage( "card_edges", edges );
				cvMoveWindow( "card_edges", 0, 400 );
				while( cvWaitKey(3000) != 32 )
				;
#endif
		}
	}

	/* Cleanup */
	cvClearSeq( contours );
	cvReleaseMat( &edges );
	cvClearMemStorage(storage);
	cvReleaseMemStorage(&storage);

	return card;
}
