#include "setcal_common.h"

void cmdEmpty(TWordListItem *set1);
void cmdCard(TWordListItem *set1);
void cmdComplement(TWordListItem *set1, TWordListItem **resSet);
void cmdUnion(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet);
void cmdIntersect(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet);
void cmdMinus(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet);
void cmdSubseteq(TWordListItem *set1, TWordListItem *set2);
void cmdSubset(TWordListItem *set1, TWordListItem *set2);
void cmdEquals(TWordListItem *set1, TWordListItem *set2);
