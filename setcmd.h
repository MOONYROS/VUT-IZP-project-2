#include "setcal_common.h"

int cmdEmpty(TWordListItem *set1);
void cmdCard(TWordListItem *set1);
void cmdComplement(TWordListItem *set1, TWordListItem **resSet);
void cmdUnion(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet);
void cmdIntersect(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet);
void cmdMinus(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet);
int cmdSubseteq(TWordListItem *set1, TWordListItem *set2);
int cmdSubset(TWordListItem *set1, TWordListItem *set2);
int cmdEquals(TWordListItem *set1, TWordListItem *set2);
