#include "setcal_common.h"

int cmdReflexive(TRelationItem *rel);
int cmdSymmetric(TRelationItem *rel);
int cmdAntisymmetric(TRelationItem *rel);
int cmdTransitive(TRelationItem *rel);
int cmdFunction(TRelationItem *rel);
void cmdDomain(TRelationItem *rel, TWordListItem **resSet);
void cmdCodomain(TRelationItem *rel, TWordListItem **resSet);
int cmdInjective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2);
int cmdSurjective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2);
int cmdBijective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2);
