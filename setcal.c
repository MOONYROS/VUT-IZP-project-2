/**
 * @file setcal.cz
 * @author  Ondrej Lukasek <ondrej@lukasek.cz>
 * @version 0.1
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Set calculator reads file, that describes sets and relations and calculates math operations and displays result.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "setcal_common.h"
#include "setcmd.h"
#include "relcmd.h"

#define MAX_ITEM_LEN 30
#define MAX_LINES 1000
#define LINES_ALLOC_SIZE 3
#define UNIVERSUM_LINE 1

#define ERR_OK 0
#define ERR_NR_ARG 1
#define ERR_FOPEN 2
#define ERR_FCLOSE 3
//#define ERR_WRONG_CHAR 4
#define ERR_LAST_LINE_CHARS 5
#define ERR_MALLOC_LINES 6
//#define ERR_PROCESS_LINE 7
#define ERR_EMPTY_LINE 8
#define ERR_UKNOWN_COMMAND 9

#define PL_ERR_OK 0
#define PL_ERR_INVALID_CMD 1
#define PL_ERR_INVALID_CHAR 2
#define PL_ERR_TOO_LONG_ITEM 3
#define PL_ERR_SET_AFTER_COMMAND 4
#define PL_ERR_REL_AFTER_COMMAND 5
#define PL_ERR_UNIVERSE_NOT_LINE1 6
#define PL_ERR_BAD_CMD_PARAM 7
#define PL_ERR_MALLOC_CMD 8
#define PL_ERR_ITEM_NOT_IN_UNIVERSE 9
#define PL_ERR_ITEM_IN_COMMANDS 10
#define PL_ERR_ITEM_IN_SET_ALREADY 11
#define PL_ERR_REL_ITEM_MISSING 12
#define PL_ERR_MISSING_PARENTHESIS 13
#define PL_ERR_ITEMS_IN_RELATION_ALREADY 14

#define ERR_CMD_LINE_NOEX 1
#define ERR_CMD_NOEX 2


/** addRelationItem prida prvky do relace
 *
 * \param prel je ukazatel na ukazatel na relaci, nesmi byt NULL
 * \param name1 a name 2 jsou retezce predstavujici prvky relace
 *
 */
void addRelationItem(TRelationItem **prel, char *name1, char *name2)
{
    assert(prel != NULL);

    TRelationItem *next = *prel;
    *prel = malloc(sizeof(TRelationItem));
    (*prel)->name1 = malloc(strlen(name1)+1);
    strcpy((*prel)->name1, name1);
    (*prel)->name2 = malloc(strlen(name2)+1);
    strcpy((*prel)->name2, name2);
    (*prel)->next = next;
}

/** printRelation vytiskne vechny prvky relace na stdout
 *
 * \param rel je ukazatel na relaci
 *
 */
void printRelation(TRelationItem *rel)
{
    printf("R");
    while(rel != NULL)
    {
        printf(" (%s %s)", rel->name1, rel->name2);
        rel = rel->next;
    }
    printf("\n");
}

/** strInRelation otestuje, jestli je dvojice retezcu str1 a str2 prvkem relace rel
 *
 * \param rel je ukazatel na relaci
 * \param str1 je prvni prvek relace, ktery hledame
 * \param str2 je druhy prvek relace, ktery hledame
 * \return vraci 1, kdyz je dvojice v relaci nalezena, jinak 0
 *
 */
int strInRelation(TRelationItem *rel, char *str1, char *str2)
{
    while(rel != NULL)
    {
        if(strcmp(rel->name1, str1) == 0 && strcmp(rel->name2, str2) == 0 )
        {
            return 1;
        }
        rel = rel->next;
    }
    return 0;
}

/** freeWordList uvolni pamet z celeho seznamu retezcu, vcetne retezcu o kterych predpoklada, ze jsou alokovane dynamicky
 *
 * \param item je ukazatel na prvni prvek seznamu
 *
 */
void freeWordList(TWordListItem *item)
{
    TWordListItem *tmpItem;

    while(item != NULL)
    {
        tmpItem = item;
        item = item->next;
        free(tmpItem->name);
        free(tmpItem);
    }
}

/** hasOnlyEnLetters zkontorluje, jestli retezec obsahuje pouze mala a velka pismena anglicke abecedy
 *
 * \param str je retezec, ktery se ma zkontrolovat
 * \return vraci 1 pokud retezec obsahuje jen mala a velka pismena anglicke abecedy, jinak 0
 *
 */
int hasOnlyEnLetters(char *str)
{
    for(size_t i=0; i<strlen(str); i++)
    {
        if(str[i] < 'A' || (str[i] > 'Z' && str[i] < 'a') || str[i] > 'z')
        {
            return 0;
        }
    }
    return 1;
}

/** strInSetCmds1 zjisti, jesli se retezec str vyskytouje mezi mnozinovymi prikazy s 1 operandem
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInSetCmds1(char *str)
{
    char *setCmds1[] = {"empty", "card", "complement"};

    for(size_t i=0; i<sizeof(setCmds1)/sizeof(char*); i++)
    {
        //printf("%d %s\n", i, *(setCmds1+i));
        if(strcmp(*(setCmds1+i), str) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/** strInSetCmds2 zjisti, jesli se retezec str vyskytouje mezi mnozinovymi prikazy se 2 operandy
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInSetCmds2(char *str)
{
    char *setCmds2[] = {"union", "intersect", "minus", "subseteq", "subset", "equals"};

    for(size_t i=0; i<sizeof(setCmds2)/sizeof(char*); i++)
    {
        //printf("%d %s\n", i, *(setCmds2+i));
        if(strcmp(*(setCmds2+i), str) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/** strInRelCmds zjisti, jesli se retezec str vyskytouje mezi relacnimi prikazy
 *
 * \param str je hledany retezec
 * \return vraci 1, kdyz je retezec mezi prikazy nalezen, jinak 0
 *
 */
int strInRelCmds(char *str)
{
    char *relCmds[] = {"reflexive", "symmetric", "antisymmetric", "transitive", "function",
                       "domain", "codomain", "injective", "surjective", "bijective"};

    for(size_t i=0; i<sizeof(relCmds)/sizeof(char*); i++)
    {
        //printf("%d %s\n", i, *(relCmds+i));
        if(strcmp(*(relCmds+i), str) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/** printAllLines vytiskne vsechny seznamy ze vsech radku
 *
 * \param line je ukazatel na pole radku
 * \param lineNr je pocet platnych radku v poli
 *
 */
void printAllLines(TLine *line, int lineNr)
{
    for(int i=0; i<lineNr; i++)
    {
        if(line[i].set != NULL)
        {
            if(i == 0)
            {
                printf("%4d: ", i+1);
                printUniversum(line[i].set);
            }
            else
            {
                printf("%4d: ", i+1);
                printSet(line[i].set);

            }
        }
        if(line[i].relation != NULL)
        {
            printf("%4d: ", i+1);
            printRelation(line[i].relation);
        }
        if(line[i].command != NULL)
        {
            printf("%4d: ", i+1);
            if(strInSetCmds1(line[i].command->name) == 1 || strInRelCmds(line[i].command->name) == 1)
            {
                printf("C %s %d\n", line[i].command->name, line[i].command->op1);
            }
            else
            {
                printf("C %s %d %d\n", line[i].command->name, line[i].command->op1, line[i].command->op2);
            }
        }
        if(line[i].set == NULL && line[i].relation == NULL && line[i].command == NULL)
        {
            printf("%4d: []\n", i+1);
        }

    }
}

/** processLine zpracuje tokeny z nacteneho radku
 *  unviversum, mnoziny a relace ulozi do prislusnych radku v poli line[]
 *  prikazy jsou pouze ulozeny, ale neprovedeny, protoze nemame zatim nactene vsechny radky a nevime tedy, zda existuji
 *
 * \param token je ukazatel na seznam tokenu (obsah vstupniho radku), nesmi byt NULL
 * \param lineNr je cislo radku ke kteremu vstupni tokeny priradit
 * \param wasCommand je ukazatel na promennou indikujici, ze jiz byl nacten nejaky prikaz 'C'
 * \param line je pole radku
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int processLine(TWordListItem *token, int lineNr, int *wasCommand, TLine *line)
{
    assert(token != NULL);
    if(strlen(token->name)!=1)
    {
        fprintf(stderr, "ERROR: Prilis dlouha definice typu radku!\n");
        return PL_ERR_INVALID_CMD;
    }
    switch(*(token->name))
    {
        case 'U':
            if(lineNr != UNIVERSUM_LINE)
            {
                fprintf(stderr, "ERROR: Univerzum neni na radku 1!\n");
                return PL_ERR_UNIVERSE_NOT_LINE1;
            }
            token=token->next;
            while(token != NULL)
            {
                if(hasOnlyEnLetters(token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Vadny znak v prvku. Prvky musi obsahova jen mala a velka pismena anglicke abecedy!\n");
                    return PL_ERR_INVALID_CHAR;
                }
                if(strInSetCmds1(token->name) == 1 || strInSetCmds2(token->name) == 1 || strInRelCmds(token->name) == 1 || strcmp("true", token->name) == 0 || strcmp("false", token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Prvek je obsazeny v prikazech!\n");
                    return PL_ERR_ITEM_IN_COMMANDS;
                }
                if(strInSet(line[UNIVERSUM_LINE - 1].set, token->name) == 1)
                {
                    fprintf(stderr, "ERROR: Prvek je uz v mnozine obsazen!\n");
                    return PL_ERR_ITEM_IN_SET_ALREADY;
                }
                addSetItem(&line[UNIVERSUM_LINE - 1].set, token->name);
                token = token->next;
            }
            printUniversum(line[0].set);
            break;
        case 'S':
            // printf("SetCMD\n");
            if(line[0].set == NULL)
            {
                fprintf(stderr, "ERROR: Univerzum nebylo definovano a mame mnozinu!\n");
                return PL_ERR_UNIVERSE_NOT_LINE1;
            }
            if(*wasCommand == 1)
            {
                fprintf(stderr, "ERROR: Mnozina po prikazu Command!\n");
                return PL_ERR_SET_AFTER_COMMAND;
            }
            token=token->next;
            while(token != NULL)
            {
                /* pismena anglicke abecedy se nemusi kontrolovat u mnoziny a relace, protoze by jinak stejne nemohlo byt v univerzu, kde se to kontroluje
                if(hasOnlyEnLetters(token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Vadny znak v prvku. Prvky musi obsahovat jen mala a velka pismena anglicke abecedy!\n");
                    return PL_ERR_INVALID_CHAR;
                } */
                if(strInSet(line[0].set, token->name) == 0)
                {
                    fprintf(stderr, "ERROR: Prvek neni obsazeny v Univerzu!\n");
                    return PL_ERR_ITEM_NOT_IN_UNIVERSE;
                }
                if(strInSet(line[lineNr - 1].set, token->name) == 1)
                {
                    fprintf(stderr, "ERROR: Prvek je uz v mnozine obsazen!\n");
                    return PL_ERR_ITEM_IN_SET_ALREADY;
                }
                addSetItem(&line[lineNr - 1].set, token->name);
                token = token->next;
            }
            printSet(line[lineNr - 1].set);
            break;
        case 'R':
            if(line[0].set == NULL)
            {
                fprintf(stderr, "ERROR: Univerzum nebylo definovano a mame relaci!\n");
                return PL_ERR_UNIVERSE_NOT_LINE1;
            }
            if(*wasCommand == 1)
            {
                fprintf(stderr, "ERROR: Relace po prikazu Command!\n");
                return PL_ERR_REL_AFTER_COMMAND;
            }

            char name1[MAX_ITEM_LEN + 1];
            char name2[MAX_ITEM_LEN + 1];
            token = token->next;
            while(token != NULL)
            {
                strcpy(name1, token->name);
                token = token->next;
                if(token == NULL)
                {
                    fprintf(stderr, "ERROR: Chybi parovy prvek relace!\n");
                    return PL_ERR_REL_ITEM_MISSING;
                }
                strcpy(name2, token->name);
                token = token->next;
                if(name1[0] == '(')
                {
                    memmove(name1, name1 + 1, strlen(name1));
                }
                else
                {
                    fprintf(stderr, "ERROR: Chybi prvni zavorka u prvniho prvku relace!\n");
                    return PL_ERR_MISSING_PARENTHESIS;
                }
                if(name2[strlen(name2) - 1] == ')')
                {
                    name2[strlen(name2) - 1] = '\0';
                }
                else
                {
                    fprintf(stderr, "ERROR: Chybi druha zavorka u druheho prvku relace!\n");
                    return PL_ERR_MISSING_PARENTHESIS;
                }

                if(strInSet(line[0].set, name1) == 0 || strInSet(line[0].set, name2) == 0)
                {
                    fprintf(stderr, "ERROR: Prvek relace neni obsazen v univerzu!\n");
                    return PL_ERR_ITEM_NOT_IN_UNIVERSE;
                }
                if(strInRelation(line[lineNr - 1].relation, name1, name2) == 1)
                {
                    fprintf(stderr, "ERROR: Prvky jsou uz v relaci obsazeny!\n");
                    return PL_ERR_ITEMS_IN_RELATION_ALREADY;
                }
                addRelationItem(&line[lineNr - 1].relation, name1, name2);
            }
            printRelation(line[lineNr - 1].relation);
            break;
        case 'C':
            *wasCommand = 1;
            token=token->next;
            int param = 0;
            int line1;
            int line2;
            char command[MAX_ITEM_LEN + 1];

            while(token != NULL)
            {
                switch(param)
                {
                    case 0:
                        strcpy(command, token->name);
                        break;
                    case 1:
                        if(sscanf(token->name, "%d", &line1) != 1)
                        {
                            fprintf(stderr, "ERROR: Parametr prikazu neni cislo!\n");
                            return PL_ERR_BAD_CMD_PARAM;
                        }
                        if(line1<1 || line1>MAX_LINES)
                        {
                            fprintf(stderr, "ERROR: Parametr prikazu {radek) je mimo rozsah 1-1000!\n");
                            return PL_ERR_BAD_CMD_PARAM;
                        }
                        break;
                    case 2:
                        if(sscanf(token->name, "%d", &line2) != 1)
                        {
                            fprintf(stderr, "ERROR: Parametr prikazu neni cislo!\n");
                            return PL_ERR_BAD_CMD_PARAM;
                        }
                        if(line2<1 || line2>MAX_LINES)
                        {
                            fprintf(stderr, "ERROR: Parametr prikazu {radek) je mimo rozsah 1-1000!\n");
                            return PL_ERR_BAD_CMD_PARAM;
                        }
                        break;
                    default:
                        fprintf(stderr, "ERROR: Prikaz ma vice nez 2 parametry!\n");
                        return PL_ERR_INVALID_CMD;
                }
                param++;
                token=token->next;
            }
            line[lineNr-1].command = malloc(sizeof(TCommand));
            if(line[lineNr-1].command ==  NULL)
            {
                fprintf(stderr, "ERROR: Nelze alokovat pamet pro ulozeni prikazu!\n");
                return PL_ERR_MALLOC_CMD;
            }
            line[lineNr-1].command->name = malloc(strlen(command)+1);
            if(line[lineNr-1].command->name ==  NULL)
            {
                fprintf(stderr, "ERROR: Nelze alokovat pamet pro ulozeni jmena prikazu!\n");
                return PL_ERR_MALLOC_CMD;
            }
            strcpy(line[lineNr-1].command->name, command);
            line[lineNr-1].command->op1 = line1;
            line[lineNr-1].command->op2 = line2;
            //printf("Command: %s %d %d\n", line[lineNr-1].command->name, line[lineNr-1].command->op1, line[lineNr-1].command->op2);
            break;
        default:
            fprintf(stderr, "ERROR: Spatny prikaz! Povolene prikazy jsou pouze U, S, R, C!\n");
            return PL_ERR_INVALID_CMD;
    }
    return 0;
}

/** skipWhitesFromFile preskoci bile znaky pri nacitani soubori
 *
 * \param fp je vstupni soubor
 * \param ch je ukazatel na posledni znak nacteny ze souboru (musi byt nacteny pred volanim)
 * \return funkce nic nevraci, ale v ch bude pri navratu prvni nebily znak na ktery narazi a tento znak bude uz nacteny ze souboru
 *
 */
void skipWhitesFromFile(FILE *fp, char *ch)
{
    while(*ch == ' ' || *ch == '\t')
    {
        *ch = fgetc(fp);
    }
}

/** getLineFromFile precte radek ze vstupniho souboru a vrati ho jako seznam tokenu
 *
 * \param fp je vstupni soubor
 * \return vracit seznam tokenu jako struct _TWordListItem
 *
 */
TWordListItem * getLineFromFile(FILE *fp)
{
    TWordListItem *tokenList = NULL;
    TWordListItem *tmpTokenList;
    int i = 0;
    char token[MAX_ITEM_LEN];

    char ch = fgetc(fp);
    skipWhitesFromFile(fp, &ch);

    while(ch != EOF)
    {
        if(ch == ' ' || ch == '\t' || ch == '\n')
        {
            if(i == 0)
            {
                // mame konec radku, tak prazdny retezec uz pridavat nebudeme
                //fprintf(stderr, "ERROR: Prazdny token, mezery na konci radku!\n");
                return tokenList;
            }
            // novy token - ukoncit a dat na konec seznamu
            token[i] = '\0';
            // jestli je zatim prazdny seznam tokenu, musime ho zalozit jinak pridat na konec
            // musime drzet poradi na radku kvuli prikazum
            if(tokenList==NULL)
            {
                tokenList = malloc(sizeof(TWordListItem));
                tmpTokenList = tokenList;
            }
            else
            {
                tmpTokenList = tokenList;
                while(tmpTokenList->next != NULL)
                {
                    tmpTokenList = tmpTokenList->next;
                }
                tmpTokenList->next = malloc(sizeof(TWordListItem));
                tmpTokenList = tmpTokenList->next;
            }
            // kontrola jestli se povedl zalozit novy token
            if( tmpTokenList==NULL )
            {
                fprintf(stderr, "ERROR: Nepodarilo se alokovat pamet pro novy token!\n");
                return NULL;
            }
            // zalozit retezec, opet zkontrolovat a naplnit daty
            tmpTokenList->name = malloc(strlen(token)+1);
            if( tmpTokenList==NULL )
            {
                fprintf(stderr, "ERROR: Nepodarilo se alokovat pamet pro retezec v novem tokenu!\n");
                return NULL;
            }
            strcpy(tmpTokenList->name, token);
            tmpTokenList->next = NULL;
            // jestli duvod konce tokenu byl konec radku, tak koncime, jinak preskocit mezery a vynulovat pocitadlo znaku, bude asi dalsi token
            if(ch == '\n')
            {
                return tokenList;
            }
            else
            {
                skipWhitesFromFile(fp, &ch);
                i = 0;
            }
        }
        else
        {
            // jen dalsi znak do aktualne nacitaneho tokenu
            token[i++] = ch;
            if(i > 30)
            {
                fprintf(stderr, "ERROR: Prilis dlouhy prvek na radku!\n");
                return NULL;
            }
            ch = fgetc(fp);
        }
    }

    if(tokenList != NULL)
    {
        fprintf(stderr, "ERROR: Neocekavany konec souboru (radek neni ukonceny \\n)!\n");
    }

    return tokenList;
}

/** processCommands zpracuje prikazy na radcich
 *
 * \param fileName je jmeno vstupniho souboru
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int processCommands(TLine *line, int lineNr)
{
    char command[MAX_ITEM_LEN];

    for(int ln = 0; ln < lineNr; ln++)
    {
        if(line[ln].command != NULL)
        {
            strcpy(command, line[ln].command->name);
            int operand1 = line[ln].command->op1;
            if(operand1 > lineNr)
            {
                fprintf(stderr, "ERROR: Prikaz pro neexistujici radek!\n");
                return ERR_CMD_LINE_NOEX;
            }
            int operand2 = line[ln].command->op2;
            if(operand2 > lineNr)
            {
                if(strInSetCmds2(command) == 1) // druhy operand nas zajima jen pokud mame prikaz pro 2 operandy
                {
                    fprintf(stderr, "ERROR: Prikaz pro neexistujici radek!\n");
                    return ERR_CMD_LINE_NOEX;
                }
            }
            // Set commands
            if(strcmp(command, "empty") == 0)
            {
                cmdEmpty(line[operand1-1].set);
                continue;
            }
            if(strcmp(command, "card") == 0)
            {
                cmdCard(line[operand1-1].set);
                continue;
            }
            if(strcmp(command, "complement") == 0)
            {
                cmdComplement(line[operand1-1].set, &line[ln].set);
                continue;
            }
            if(strcmp(command, "union") == 0)
            {
                cmdUnion(line[operand1-1].set, line[operand2-1].set, &line[ln].set);
                continue;
            }
            if(strcmp(command, "intersect") == 0)
            {
                cmdIntersect(line[operand1-1].set, line[operand2-1].set, &line[ln].set);
                continue;
            }
            if(strcmp(command, "minus") == 0)
            {
                cmdMinus(line[operand1-1].set, line[operand2-1].set, &line[ln].set);
                continue;
            }
            if(strcmp(command, "subseteq") == 0)
            {
                cmdSubseteq(line[operand1-1].set, line[operand2-1].set);
                continue;
            }
            if(strcmp(command, "subset") == 0)
            {
                cmdSubset(line[operand1-1].set, line[operand2-1].set);
                continue;
            }
            if(strcmp(command, "equals") == 0)
            {
                cmdEquals(line[operand1-1].set, line[operand2-1].set);
                continue;
            }

            // Relation commands
            if(strcmp(command, "reflexive") == 0)
            {
                cmdReflexive(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "symmetric") == 0)
            {
                cmdSymmetric(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "antisymmetric") == 0)
            {
                cmdAntisymmetric(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "transitive") == 0)
            {
                cmdTransitive(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "function") == 0)
            {
                cmdFunction(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "domain") == 0)
            {
                cmdDomain(line[operand1-1].relation, &line[ln].set);
                continue;
            }
            if(strcmp(command, "codomain") == 0)
            {
                cmdCodomain(line[operand1-1].relation, &line[ln].set);
                continue;
            }
            if(strcmp(command, "injective") == 0)
            {
                cmdInjective(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "surjective") == 0)
            {
                cmdSurjective(line[operand1-1].relation);
                continue;
            }
            if(strcmp(command, "bijective") == 0)
            {
                cmdBijective(line[operand1-1].relation);
                continue;
            }
            fprintf(stderr, "ERROR: Neznamy prikaz!\n");
            return ERR_UKNOWN_COMMAND;
        }
    }
    return ERR_OK;
}

/** processFile zpracuje vstupni soubor podle zadani
 *
 * \param fileName je jmeno vstupniho souboru
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int processFile(char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "r");

    if(fp != NULL)
    {
        int lineNr = 0;
        int allocatedLines = LINES_ALLOC_SIZE;
        int err;
        int wasCommand = 0;

        // definujeme a vytvorime vynulovane pole s informacemi pro max 1000 (MAX_LINES) radku vstupniho souboru
        TLine *line;
        line = malloc(sizeof(TLine) * allocatedLines);
        memset(line, 0, sizeof(TLine) * allocatedLines);
        if(line == NULL)
        {
            fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek pameti pro pole radku!\n");
            return ERR_MALLOC_LINES;
        }

        // prochazi cely soubor
        do
        {
            TWordListItem *token = getLineFromFile(fp);
            if(token != NULL)
            {
                if(feof(fp))
                {
                    // neco se nacetlo, ale jsme na konci souboru, takze radek nebyl ukonceny \n a to je chyba
                    return ERR_LAST_LINE_CHARS;
                }
                lineNr++;
                if(lineNr>allocatedLines)
                {
                    allocatedLines += LINES_ALLOC_SIZE;
                    line = realloc(line, sizeof(TLine) * allocatedLines);
                    if(line == NULL)
                    {
                        fprintf(stderr, "ERROR: Nepodarilo se alokovat dostatek pameti pro pole radku!\n");
                        return ERR_MALLOC_LINES;
                    }
                    memset(line + allocatedLines - LINES_ALLOC_SIZE, 0, sizeof(TLine) * LINES_ALLOC_SIZE);
                }
                //printf("Nacteny radek = "); printSet(token);
                err = processLine(token, lineNr, &wasCommand, line);
                freeWordList(token);
                if( err != 0 )
                {
                    return err;
                }
            }
            else
            {
                if(feof(fp))
                {
                    // nic se nenactelo, ale jsme uz na konci souboru, tak to je OK
                    break;
                }
                fprintf(stderr, "Nepodarilo se nacist vstupni radek nebo je prazdny!");
                return ERR_EMPTY_LINE;
            }
        } while (!feof(fp));

        if(fclose(fp) != 0)
        {
            fprintf(stderr, "Nepodarilo se zavrit vstupni soubor!");
            return ERR_FCLOSE;
        }

        processCommands(line, lineNr);

        printf("\n... a to je konec programu\ntohle mame v pameti radku:\n\n");
        printAllLines(line, lineNr);
        // aaa tady se musi smazat vsechny naalokovane struktury
    }
    else
    {
        fprintf(stderr, "Nepodarilo se otevrit vstupni soubor! Chyba %s\n", strerror(errno));
        return ERR_FOPEN;
    }

    return ERR_OK;
}

/** main nacte argument (jmeno souboru) z prikazoveho radku
 *  a zavola processFile() na zpracovani souboru
 *
 * \param argc je pocet parametru na prikazove radce (vc. vlastniho spustitelneho souboru)
 * \param argv[] jsou parametry prikazove radky
 * \return vraci 0 pri uspesnem zpracovani, jinak kod chyby v pripade neuspechu
 *
 */
int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        int result = processFile(argv[1]);
        if(result != 0)
        {
            //fprintf(stderr, "Chyba pri zpracovani vstupniho souboru!");
            return result;
        }
    }
    else
    {
        fprintf(stderr, "Spatny pocet vstupnich argumentu. Spustte program jako ./setcal FILE!");
        return ERR_NR_ARG;
    }

    return ERR_OK;
}
