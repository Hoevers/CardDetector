#include "cards.h"
#include <stdio.h>

#include <opencv/cv.h>

/** Card strings for all types */
const char *
card_type_string[] =
{
	"joker",
	"hearts",
	"spades",
	"clubs",
	"diamonds",
	"ace",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"nine",
	"ten",
	"jack",
	"queen",
	"king"
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
	enum card_type current = CARD_JOKER;
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
//			exit(-1);
		}
		else
			printf("file exist :-)!\n");

		fclose(file);

		/* TODO */
		/*
			Load image with opencv
			Enhance image
			Canny edge
			Contour
			Save contour to array
		*/
	}

	return 0;
}
