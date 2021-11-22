#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "setcmd.h"

/** cmdEmpty tiskne true nebo false podle toho, jestli je mnozina definovana na radku A prazdna nebo neprazdna
 *
 * \param set1 je ukazatel na mnozinu
 *
 */
void cmdEmpty(TWordListItem *set1)
{
    if(set1 == NULL)
    {
        printf("true\n");
    }
    else
    {
        printf("false\n");
    }
}

void cmdCard(TWordListItem *set1) /// Ondra
{

}

void cmdComplement(TWordListItem *set1, TWordListItem **resSet) /// Petana
{
    assert(resSet != NULL);

}

void cmdUnion(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet) /// mikki
{
    assert(resSet != NULL);

}

/** cmdIntersect tiskne prunik mnozin set1 a set2
 *
 * \param set1 je ukazatel na prvni mnozinu
 * \param set2 je ukazatel na druhou mnozinu
 * \param resSet je ukazatel na ukazatel, do ktereho je ulozena vysledna mnozina
 *
 */
void cmdIntersect(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet)
{
    assert(resSet != NULL);

    TWordListItem *setTmp = set2;
    while(set1 != NULL)
    {
        while(setTmp != NULL)
        {
            if(strcmp(set1->name, setTmp->name) == 0)
            {
                if(resSet != NULL)
                {
                    addSetItem(resSet, set1->name);
                }
            }
            setTmp = setTmp->next;
        }
        set1 = set1->next;
        setTmp = set2;
    }
    printSet(*resSet);
}

/** cmdMinus tiskne rozdil mnozin set1 a set2
 *
 * \param set1 je ukazatel na prvni mnozinu
 * \param set2 je ukazatel na druhou mnozinu
 * \param resSet je ukazatel na ukazatel, do ktereho je ulozena vysledna mnozina
 *
 */
void cmdMinus(TWordListItem *set1, TWordListItem *set2, TWordListItem **resSet)
{
    assert(resSet != NULL);

    TWordListItem *setTmp = set2;
    while(set1 != NULL)
    {
        int isThere = 0;
        while(setTmp != NULL)
        {
            if(strcmp(set1->name, setTmp->name) == 0)
            {
                isThere = 1;
            }
            setTmp = setTmp->next;
        }
        if(isThere == 0)
        {
            if(resSet != NULL)
            {
                addSetItem(resSet, set1->name);
            }
        }
        set1 = set1->next;
        setTmp = set2;
    }
    printSet(*resSet);
}

void cmdSubseteq(TWordListItem *set1, TWordListItem *set2) /// Ondra
{

}

void cmdSubset(TWordListItem *set1, TWordListItem *set2)    /// Petana
{

}

void cmdEquals(TWordListItem *set1, TWordListItem *set2)    /// mikki
{

}
