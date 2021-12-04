#include "setcal_common.h"

TRelationItem *findRelXY(TRelationItem *rel, char *x, char *y);
int cmdReflexive(TRelationItem *rel, TWordListItem *universum);
int cmdSymmetric(TRelationItem *rel);
int cmdAntisymmetric(TRelationItem *rel);
int cmdTransitive(TRelationItem *rel);
int cmdFunction(TRelationItem *rel);
void cmdDomain(TRelationItem *rel, TWordListItem **resSet);
void cmdCodomain(TRelationItem *rel, TWordListItem **resSet);
int cmdInjective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2);
int cmdSurjective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2);
int cmdBijective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2);
void cmdClosureRef(TRelationItem *rel, TWordListItem *universum, TRelationItem **resRel);
void cmdClosureSym(TRelationItem *rel, TRelationItem **resRel);
void cmdClosureTrans(TRelationItem *rel, TRelationItem **resRel);
