#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "relcmd.h"

int cmdReflexive(TRelationItem *rel) /// Ondra
{
    int elementNum = 0;
    int sameElementRelationNum = 0;

    while (rel != NULL)
    {
        elementNum += 2;
        if (strcmp(rel->name1, rel->name2) == 0)
        {
            sameElementRelationNum++;
        }
        rel = rel->next;
    }

    if (elementNum / 2 == sameElementRelationNum)
    {
        return true;
    }
    else 
    {
        return false;
    }
}


/** cmdSymmetric tiskne true pokud je relace symetricka, false pokud neni
 *
 * \param rel je ukazatel na danou relaci
 * \return true nebo false
 *
 */
int cmdSymmetric(TRelationItem *rel) /// Petana
{
    while(rel != NULL){
    if(strcmp(rel->name1, rel->name2)!=0){
      TRelationItem *secRel = rel->next;

      while(secRel != NULL){
        if((strcmp(rel->name1, secRel->name2)==0) && (strcmp(rel->name2, secRel->name1)==0)){
          printf("True\n");
          return true;
        }else{
          printf("False\n");
          return false;
        }
        secRel = secRel->next;
      }
      rel= rel->next;
    }
  }
    
}

/** cmdAntisymmetric tiskne true nebo false, jestli je relace antisymetricka
 *
 * \param rel je ukazatel na seznam prvku relace
 *
 */
int cmdAntisymmetric(TRelationItem *rel) /// mikki
{
    while(rel != NULL)
    {
        if(strcmp(rel->name1,rel->name2)==0)
        {
            printf("true\n");
            return true;
        }
        TRelationItem *tmpRel = rel->next;
        while(tmpRel != NULL)
        {
            if(strcmp(rel->name1, tmpRel->name2)==0)
            {
                if(strcmp(rel->name2, tmpRel->name1)==0)
                {
                    printf("false\n");
                    return false;
                }
            }
            tmpRel=tmpRel->next;
        }
        rel=rel->next;
    }
    printf("true\n");
    return true;
}

int cmdTransitive(TRelationItem *rel)  /// Ondra
{
    return false;
}

/** cmdFunction tiskne true nebo false, jestli je relace rel funkci
 *
 * \param rel je ukazatel na seznam prvku relace
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

    while(rel != NULL){
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

/** cmdSurjective tiskne true nebo false, jestli je funkce rel surjektivni
 *
 * \param rel je ukazatel na seznam prvku relace
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
