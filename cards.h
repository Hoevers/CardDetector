#ifndef CARDS_H__
#define CARDS_H__

#include <opencv/cv.h>

enum
card_type
{
	CARD_JOKER = 0,
	CARD_HEARTS,
	CARD_SPADES,
	CARD_CLUBS,
	CARD_ACE,
	CARD_TWO,
	CARD_THREE,
	CARD_FOUR,
	CARD_FIVE,
	CARD_SIX,
	CARD_SEVEN,
	CARD_NINE,
	CARD_TEN,
	CARD_JACK,
	CARD_QUEEN,
	CARD_KING,
	CARD_TYPE_END
};

struct
card_t
{
	enum card_type type;
	CvSeq *contour;
};

int card_init(const char *directory);

#endif /* CARDS_H__ */
