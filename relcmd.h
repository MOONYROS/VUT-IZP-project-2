#include "setcal_common.h"

void cmdReflexive(TRelationItem *rel);
void cmdSymmetric(TRelationItem *rel);
void cmdAntisymmetric(TRelationItem *rel);
void cmdTransitive(TRelationItem *rel);
void cmdFunction(TRelationItem *rel);
void cmdDomain(TRelationItem *rel, TWordListItem **resSet);
void cmdCodomain(TRelationItem *rel, TWordListItem **resSet);
void cmdInjective(TRelationItem *rel);
void cmdSurjective(TRelationItem *rel);
void cmdBijective(TRelationItem *rel);
