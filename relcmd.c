#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "relcmd.h"

TRelationItem *findRelX(TRelationItem *rel, char *x)
{
    while(rel != NULL)
    {
        if(strcmp(rel->name1, x)==0)
        {
            return rel;
        }
        rel = rel->next;
    }
    return NULL;
}

TRelationItem *findRelY(TRelationItem *rel, char *y)
{
    while(rel != NULL)
    {
        if(strcmp(rel->name2, y)==0)
        {
            return rel;
        }
        rel = rel->next;
    }
    return NULL;
}

TRelationItem *findRelXY(TRelationItem *rel, char *x, char *y)
{
    while(rel != NULL)
    {
        if((strcmp(rel->name1, x)==0) && (strcmp(rel->name2, y)==0))
        {
            return rel;
        }
        rel = rel->next;
    }
    return NULL;
}

int countRelX(TRelationItem *rel, char *x)
{
    int cnt = 0;
    while(rel != NULL)
    {
        if(strcmp(rel->name1, x)==0)
        {
            cnt++;
        }
        rel = rel->next;
    }
    return cnt;
}

int countRelY(TRelationItem *rel, char *y)
{
    int cnt = 0;
    while(rel != NULL)
    {
        if(strcmp(rel->name2, y)==0)
        {
            cnt++;
        }
        rel = rel->next;
    }
    return cnt;
}

/** cmdReflexive tiskne a vraci true nebo false podle toho, jestli je relace reflexivni
 *
 * \param rel je ukazatel na seznam prvku relace
 * \return vraci true (1), pokud je relace reflexivni, jinak false (0)
 *
 */
int cmdReflexive(TRelationItem *rel)
{
    while(rel != NULL)
    {
        if (strcmp(rel->name1, rel->name2) != 0)
        {
            printf ("false\n");
            return false;
        }
        rel = rel->next;
    }
    printf ("true\n");
    return true;
}


/** cmdSymmetric tiskne a vraci true pokud je relace symetricka, false pokud neni
 *
 * \param rel je ukazatel na danou relaci
 * \return vraci true (1), pokud je relace symetricka, jinak false (0)
 *
 */
int cmdSymmetric(TRelationItem *rel) /// Petana
{
    TRelationItem *tmpRel = rel;
    while(tmpRel != NULL)
    {
        if(strcmp(tmpRel->name1, tmpRel->name2) != 0)
        {
            TRelationItem *secRel = rel;
            while(secRel != NULL)
            {
                if((strcmp(tmpRel->name1, secRel->name2) == 0) && (strcmp(tmpRel->name2, secRel->name1) == 0))
                {
                    break;
                }
                secRel = secRel->next;
            }
            if(secRel == NULL)
            {
                printf("false\n");
                return false;
            }
        }
        tmpRel = tmpRel->next;
    }
    printf("true\n");
    return true;
}

/** cmdAntisymmetric tiskne true nebo false, jestli je relace antisymetricka
 *
 * \param rel je ukazatel na seznam prvku relace
 * \return vraci true (1), pokud je relace antisymetricka, jinak false (0)
 *
 */
int cmdAntisymmetric(TRelationItem *rel) /// mikki
{
    TRelationItem *origRel = rel;
    while(rel != NULL)
    {
        if(strcmp(rel->name1, rel->name2) != 0)
        {
            TRelationItem *tmpRel = origRel;
            while(tmpRel != NULL)
            {
                if((strcmp(rel->name1, tmpRel->name2) == 0) && (strcmp(rel->name2, tmpRel->name1) == 0))
                {
                    printf("false\n");
                    return false;
                }
                tmpRel = tmpRel->next;
            }
        }
        rel = rel->next;
    }
    printf("true\n");
    return true;
}

/** cmdTransitive tiskne a vraci true nebo false, jestli je relace rel tranzitivni
 *
 * \param rel je ukazatel na seznam prvku relace
 * \return vraci true (1), pokud je relace tranzitivni, jinak false (0)
 *
 */
int cmdTransitive(TRelationItem *rel)
{
    TRelationItem *firstRel = rel;
    TRelationItem *secondRel;
    while(firstRel != NULL)
    {
        secondRel = rel;
        while(secondRel != NULL)
        {
            if(firstRel != secondRel && strcmp(firstRel->name2, secondRel->name1) == 0)
            {
                if(findRelXY(rel, firstRel->name1, secondRel->name2) == NULL)
                {
                    printf("false\n");
                    return false;
                }
            }
            secondRel = secondRel->next;
        }
        firstRel = firstRel->next;
    }
    printf("true\n");
    return true;
}

/** cmdFunction tiskne a vraci true nebo false, jestli je relace rel funkci
 *
 * \param rel je ukazatel na seznam prvku relace
 * \return vraci true (1), pokud je relace funkci, jinak false (0)
 *
 */
int cmdFunction(TRelationItem *rel)
{
    while(rel != NULL)
    {
        TRelationItem *tmpRel = rel->next;
        while(tmpRel != NULL)
        {
            if(strcmp(rel->name1, tmpRel->name1) == 0)
            {
                printf("false\n");
                return false;
            }
            tmpRel = tmpRel->next;
        }
        rel = rel->next;
    }
    printf("true\n");
    return true;
}

/** cmdDomain tiskne definicni obor relace
 *
 * \param rel je ukazatel na danou relaci
 * \param resSet je ukazatel na ukazatel na vysledny definicni obor
 *
 */
void cmdDomain(TRelationItem *rel, TWordListItem **resSet) /// Petana
{
    assert(resSet != NULL);

    while(rel != NULL)
    {
        addSetItem(resSet, rel->name1);
        rel = rel->next;
    }
    printSet(*resSet);
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

/** cmdInjective tiskne a vraci true nebo false, jestli je relace rel injektivni
 *
 * \param rel je ukazatel na seznam prvku relace
 * \param set1 je ukazatel na mnozinu A
 * \param set2 je ukazatel na mnozinu B
 * \return vraci true (1), pokud je relace injektivni, jinak false (0)
 *
 */
int cmdInjective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2)   /// Petana
{
    TRelationItem *tmpRel;
    while(set2 != NULL)
    {
        if(countRelY(rel, set2->name) >= 2)
        {
            printf("false\n");
            return false;
        }
        tmpRel = findRelY(rel, set2->name);
        if(tmpRel != NULL)
        {
            if(!strInSet(set1, tmpRel->name1))
            {
                printf("false\n");
                return false;
            }
        }
        set2 = set2->next;
    }
    printf("true\n");
    return true;
}

/** cmdSurjective tiskne a vraci true nebo false, jestli je relace rel surjektivni
 *
 * \param rel je ukazatel na seznam prvku relace
 * \param set1 je ukazatel na mnozinu A
 * \param set2 je ukazatel na mnozinu B
 * \return vraci true (1), pokud je relace surjektivni, jinak false (0)
 *
 */
int cmdSurjective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2)  /// mikki
{
    TRelationItem *tmpRel;
    while(set2 != NULL)
    {
        tmpRel = findRelY(rel, set2->name);
        if(tmpRel == NULL)
        {
            printf("false\n");
            return false;
        }
        if(!strInSet(set1, tmpRel->name1))
        {
            printf("false\n");
            return false;
        }
        set2 = set2->next;
    }
    printf("true\n");
    return true;
}

/** cmdBijective tiskne a vraci true nebo false, jestli je relace rel bijektivni
 *
 * \param rel je ukazatel na seznam prvku relace
 * \param set1 je ukazatel na mnozinu A
 * \param set2 je ukazatel na mnozinu B
 * \return vraci true (1), pokud je relace bijektivni, jinak false (0)
 *
 */
int cmdBijective(TRelationItem *rel, TWordListItem *set1, TWordListItem *set2) /// Ondra
{
    TRelationItem *tmpRel;
    TWordListItem *tmpSet;
    tmpSet = set1;
    while(tmpSet != NULL)
    {
        if(countRelX(rel, tmpSet->name) != 1)
        {
            printf("false\n");
            return false;
        }
        tmpRel = findRelX(rel, tmpSet->name);
        if(tmpRel != NULL)
        {
            if(!strInSet(set2, tmpRel->name2))
            {
                printf("false\n");
                return false;
            }
        }
        else
        {   // toto by teoreticky nemelo nastat, protoze jsme si predtim spocitali, ze je prave 1
            printf("false\n");
            return false;
        }
        tmpSet = tmpSet->next;
    }
    tmpSet = set2;
    while(tmpSet != NULL)
    {
        if(countRelY(rel, tmpSet->name) != 1)
        {
            printf("false\n");
            return false;
        }
        tmpRel = findRelY(rel, tmpSet->name);
        if(tmpRel != NULL)
        {
            if(!strInSet(set1, tmpRel->name1))
            {
                printf("false\n");
                return false;
            }
        }
        else
        {   // toto by teoreticky nemelo nastat, protoze jsme si predtim spocitali, ze je prave 1
            printf("false\n");
            return false;
        }
        tmpSet = tmpSet->next;
    }
    printf("true\n");
    return true;
}
