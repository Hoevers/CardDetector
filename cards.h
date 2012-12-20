#ifndef CARDS_H__
#define CARDS_H__

#include <opencv/cv.h>

enum
card_type
{
	CARD_UNKNOWN = -1,
	CARD_JOKER = 0,
	CARD_HEARTS,
	CARD_SPADES,
	CARD_CLUBS,
	CARD_DIAMONDS,
	CARD_JACK,
	CARD_QUEEN,
	CARD_KING,
	CARD_TYPE_END,
};

int  card_init(const char *directory);
enum card_type card_detect(CvMat *card);
void card_cleanup(void);
void card_process(CvMat *image, enum card_type current);

#endif /* CARDS_H__ */
