#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "relcmd.h"

void cmdReflexive(TRelationItem *rel)
{

}

void cmdSymmetric(TRelationItem *rel)
{

}

void cmdAntisymmetric(TRelationItem *rel)
{

}

void cmdTransitive(TRelationItem *rel)
{

}

/** cmdFunction tiskne true nebo false, jestli je relace rel funkci
 *
 * \param rel je ukazatel na seznam prvku relace
 *
 */
void cmdFunction(TRelationItem *rel)
{
    while(rel != NULL)
    {
        TRelationItem *tmpRel = rel->next;
        while(tmpRel != NULL)
        {
            if(strcmp(rel->name1, tmpRel->name1) == 0)
            {
                printf("false\n");
                return;
            }
            tmpRel = tmpRel->next;
        }
        rel = rel->next;
    }
    printf("true\n");
}

void cmdDomain(TRelationItem *rel, TWordListItem **resSet)
{
    assert(resSet != NULL);

}

/** cmdCodomain tiskne obor hodnot funkce rel
 *
 * \param rel je ukazatel na seznam prvku relace
 * \param resSet je ukazatel na ukazatel, do ktereho je ulozena vysledna mnozina
 *
 */
void cmdCodomain(TRelationItem *rel, TWordListItem **resSet)
{
    assert(resSet != NULL);

    while(rel != NULL)
    {
        addSetItem(resSet, rel->name2);
        rel = rel->next;
    }
    printSet(*resSet);
}

void cmdInjective(TRelationItem *rel)
{

}

void cmdSurjective(TRelationItem *rel)
{

}

void cmdBijective(TRelationItem *rel)
{

}
