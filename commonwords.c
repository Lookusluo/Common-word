
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>



#define MAX_WORD_COUNT 65536//the max number of words can be found
#define MAX_WORD_LENGTH 50
#define OUT_FILE	"/home/kleistluo/Desktop/BlackBoxTesting/myout.txt"//custom output path

typedef struct WordCount 
{
	char cWord[MAX_WORD_LENGTH];
	int  iCount;
}T_WordCount;

typedef struct  SameWordInfo
{
	int iIndexOne;
	int iIndexTwo;

}T_SameWordInfo;

//translate upper case to lower case word
void LowerText(char *pText)
{
	char *pTmp = pText;
	while (*pTmp != '\0')
	{
		if ((*pTmp >= 'A' && *pTmp <= 'Z'))
		{
			*pTmp += 32 ;
		}
		
		pTmp++;
	}
}

//find common word
//count common words occurance
int ExtractWord(const char *pText, T_WordCount *pWordSet, int *pWordCount)
{
	char cTmp[MAX_WORD_LENGTH] = {0};	
	int  i = 0;
	char *pTmp   = cTmp;
	int  iFlag   = 0;
	
	while (*pText != '\0')
	{		
		if ((*pText >= 'A' && *pText <= 'Z') || (*pText >= 'a' && *pText <= 'z'))
		{	
			*pTmp = *pText;
			pTmp++;
		}
		else if (*pText == '-')
		{
			++pText;
			continue;
		}
		else
		{	
			if (strlen(cTmp) > 0)
			{
				LowerText(cTmp);
				iFlag = 0;
				for (i = 0; i < MAX_WORD_COUNT; ++i)
				{
					if (strlen(pWordSet[i].cWord) > 0)
					{
						if (strcmp(pWordSet[i].cWord, cTmp) == 0)
						{
							iFlag = 1;
							pWordSet[i].iCount++;
							(*pWordCount)++; 
							break;
						}						
					}
					else
					{
						strcpy(pWordSet[i].cWord, cTmp);
						pWordSet[i].iCount = 1;
						iFlag = 1;
						(*pWordCount)++; 
						break;
					}
					
				}
				
				if (!iFlag)
				{
					printf("No more space to save word.\n");
				}
			}
			memset(cTmp, 0, MAX_WORD_LENGTH);
			pTmp = cTmp;
		}		
		++pText;
	}	

	return 0;
}

//Calculate result
//show result on monitor and outfile at the same time
int CalcResult(T_WordCount *pSetOne, T_WordCount *pSetTwo, int *pWordCountOne, int *pWordCountTwo)
{
	int i = 0;
	int j = 0;
	int iCurIndex = 0;
	int iSameCountOne = 0;
	FILE *file = NULL;
	
	T_SameWordInfo tSameWordInfo[MAX_WORD_COUNT];
	memset(&tSameWordInfo, 0, sizeof(tSameWordInfo));
	
	for (i = 1; i < MAX_WORD_COUNT; ++i)
	{
		if (strlen(pSetOne[i].cWord) <= 0)
		{
			break;
		}

		for (j = 0; j < MAX_WORD_COUNT; ++j)
		{       
			if (strlen(pSetTwo[j].cWord) <= 0)
			{
				break;
			}

			if (strcmp(pSetOne[i].cWord, pSetTwo[j].cWord) == 0)
			{
				tSameWordInfo[iCurIndex].iIndexOne = i ;
				++iCurIndex;
			}
		}
	}


	file = fopen(OUT_FILE, "w");
	if (file == NULL)
	{
		printf("Can not open file to write.\n");
		return -1;
	}

	for (i = 0; i < iCurIndex; ++i)
	{
		iSameCountOne += pSetOne[(tSameWordInfo[i].iIndexOne)].iCount;
		printf("%s, %d\n", pSetOne[(tSameWordInfo[i].iIndexOne)].cWord, 
			pSetOne[(tSameWordInfo[i].iIndexOne)].iCount
			);

		fprintf(file, "%s, %d\n", pSetOne[(tSameWordInfo[i].iIndexOne)].cWord, 
			pSetOne[(tSameWordInfo[i].iIndexOne)].iCount
			);
	}


	fclose(file);

	return 0;

}



int main(int argc, char *argv[])
{
	FILE *file = NULL;
	int iWordCountOne = 0;
	int iWordCountTwo = 0;
	char *pTextOne = NULL;
	char *pTextTwo = NULL;
	T_WordCount tWordSetOne[MAX_WORD_COUNT];
	T_WordCount tWordSetTwo[MAX_WORD_COUNT];

	struct stat stStatus; 
	
	//if it isn't have correct input. exit and print input from
	if (argc != 3)
	{
		printf("Usage:app file1 file2\n");
		return -1;
	}

	//get file info
	if (0 != stat(argv[1], &stStatus))
	{
		printf("Can not get file info of [%s],%d\n", argv[1], errno);
		return  -1;
	}
	
	//read only open
	file = fopen(argv[1], "r");
	if (file == NULL)
	{
		printf("Can not open file:%s\n", argv[1]);
		return  -1;
	}
	
	//allocate to a memory bigger than files one more KB
	pTextOne = (char *)malloc(stStatus.st_size + 1);
	if (pTextOne == NULL)
	{
		printf("Can not allocate memory.\n");
		return -1;
	}
	//initial memory
	memset(pTextOne, 0, stStatus.st_size + 1);
	
	//read file in memory
	if(fread(pTextOne, sizeof(char), (size_t)stStatus.st_size, file) <= 0)
	{
		printf("Can not read file.\n");
		return -1;
	}

	//deal with file2 same with above
	if (0 != stat(argv[2], &stStatus))
	{
		printf("Can not get file info of [%s]\n", argv[2]);
		return  -1;
	}
	
	file = fopen(argv[2], "r");
	if (file == NULL)
	{
		printf("Can not open file:%s\n", argv[2]);
		return  -1;
	}
	
	pTextTwo = (char *)malloc(stStatus.st_size + 1);
	if (pTextTwo == NULL)
	{
		printf("Can not allocate memory.\n");
		return -1;
	}
	memset(pTextTwo, 0, stStatus.st_size + 1);
	
	if(fread(pTextTwo, sizeof(char), (size_t)stStatus.st_size, file) <= 0)
	{
		printf("Can not read file.\n");
		return -1;
	}

	//fclose(file);

	//printf("%s\n", pTextOne);
	//printf("%s\n", pTextTwo);


	memset(tWordSetOne, 0, sizeof(tWordSetOne));
	memset(tWordSetTwo, 0, sizeof(tWordSetTwo));
	
	//Extract common words
	ExtractWord(pTextOne, &tWordSetOne, &iWordCountOne);
	ExtractWord(pTextTwo, &tWordSetTwo, &iWordCountTwo);

	//calculate result
	CalcResult(&tWordSetOne, &tWordSetTwo, &iWordCountOne, &iWordCountTwo);

	//free memory
	free(pTextOne);
	free(pTextTwo);

	return 0;

}
