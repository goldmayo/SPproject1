#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include "common.h"

//  해당인자가 존재하는지 확인하는 함수
int check_arg(char *av[], const char *opt)
{
	int count = 0;

	// 모든 인자값을 확인한다.
	while(*av != '\0')
	{
		// opt 인자가 존재하는가?
		if(!strcmp(av[count], opt))
		{
			return TRUE;
		}

		av++;
	}

	return FALSE;
}

// cd
void my_cd(int ac, char *av[])
{
	char *path;

	// 인자가 있을 경우 path를 설정
	if(ac > 1)
	{
		path = av[1];
	}
	
	// 인자가 없을 경우 HOME디렉토리를 설정
	else if((path = (char*)getenv("HOME")) == NULL)
	{
		// 환경 변수가 없을 경우 현재 디렉토리로 설정
		path = ".";
	}

	// 디렉토리를 변경한다.
	if(chdir(path) == ERROR)
	{
		fprintf(stderr, "%s: bad directory.\n", path);
	}
}

// exit
void my_exit()
{
	exit(1);
}
// ls
void my_ls(int ac, char *av[])
{
	DIR *dp;
	struct dirent *entry;
	char *path;
	int count;
	int opt_a;
	int opt_l;

	// 인자가 없을 경우 자기 자신의 디렉토리로 설정한다.
	if(ac < 2)
	{
		path = ".";
	}
	
	// 인자가 있을 경우 설정
	else
	{
		path = av[1];
	}

	if((dp = opendir(path)) == NULL)
	{
		fprintf(stderr, "Can't open directory: %s\n", av[1]);
		return;
	}

	// 다음의 인자들이 존재하는지 확인
	opt_a = check_arg(av, "-a");
	opt_l = check_arg(av, "-l");

	count = 0;

	// 파일이나 디렉토리를 읽어들인다.
	while((entry = readdir(dp)) != NULL)
	{
		// -a 옵션이 없을 경우 숨김 파일은 표시하지 않는다.
		if(!opt_a)
		{
			if(entry->d_name[0] == '.')
			{
				continue;
			}
		}
		
		// 출력
		printf("%s\t", entry->d_name);

		// -l 옵션이 설정되어있을 경우 줄마다 한원소씩을 출력한다.
		if(opt_l)
		{
			printf("\n");
		}
		
		// 한줄에 3개씩 출력한다.
		else
		{
			if(count > 3)
			{
				printf("\n");
				count = 0;
			}
			else
			{
				count++;
			}
		}
	}

	closedir(dp);
	printf("\n");
}

// cp
void my_cp(int ac, char *av[])
{
	FILE *src;
	FILE *dst;
	char ch;

	// 인자가 2개 이하일 경우 에러
	if(ac < 3)
	{
		fprintf(stderr, "Not enough arguments.\n");
		return;
	}

	if((src = fopen(av[1], "r")) == NULL)
	{
		fprintf(stderr, "%s: Can't open file.\n", av[1]);
		return;
	}

	if((dst = fopen(av[2], "w")) == NULL)
	{
		fprintf(stderr, "%s: Can't open file.\n", av[2]);
		return;
	}

	// 복사
	while(!feof(src))
	{
		ch = (char) fgetc(src);

		if(ch != EOF)
		{
			fputc((int)ch, dst);
		}
	}

	// -v 옵션이 있을 경우 내용 출력
	if(check_arg(av, "-v"))
	{
		printf("cp %s %s\n", av[1], av[2]);
	}

	fclose(src);
	fclose(dst);
}

// rm
void my_rm(int ac, char *av[])
{
	// 인자가 없을 경우 에러
	if(ac < 2)
	{
		fprintf(stderr, "Not enough arguments.\n");
		return;
	}

	// 파일 삭제
	unlink(av[1]);

	// -v 옵션이 있을 경우 결과 출력
	if(check_arg(av, "-v"))
	{
		printf("rm %s\n", av[1]);
	}
}

// mv
void my_mv(int ac, char *av[])
{
	FILE *src;
	FILE *dst;
	char ch;

	// 인자가 2개 이하일 경우 에러
	if(ac < 3)
	{
		fprintf(stderr, "Not enough arguments.\n");
		return;
	}

	// 복사할 파일을 연다.
	if((src = fopen(av[1], "r")) == NULL)
	{
		fprintf(stderr, "%s: Can't open file.\n", av[1]);
		return;
	}

	// 쓰기할 파일을 연다.
	if((dst = fopen(av[2], "w")) == NULL)
	{
		fprintf(stderr, "%s: Can't open file.\n", av[2]);
		return;
	}

	// 복사
	while(!feof(src))
	{
		ch = (char) fgetc(src);

		if(ch != EOF)
		{
			fputc((int)ch, dst);
		}
	}

	fclose(src);
	fclose(dst);

	// 기존 파일 삭제
	unlink(av[1]);

	// -v 옵션을 사용하면  결과를 출력한다.
	if(check_arg(av, "-v"))
	{
		printf("mv %s %s\n", av[1], av[2]);
	}
}

// mkdir
void my_mkdir(int ac, char *av[])
{
	// 인자가 없을 경우 에러
	if(ac < 2)
	{
		fprintf(stderr, "Not enough arguments.\n");
		return;
	}

	if(mkdir(av[1], 0755))
	{
		fprintf(stderr, "Make directory failed.\n");
	}
}

// rmdir
void my_rmdir(int ac, char *av[])
{
	// 인자가 없을 경우 에러
	if(ac < 2)
	{
		fprintf(stderr, "Not enough arguments.\n");
		return;
	}

	if(rmdir(av[1]))
	{
		fprintf(stderr, "Remove directory failed.\n");
	}
}

// cat
void my_cat(int ac, char *av[])
{
	int ch;
	FILE *fp;

	// 인자가 없을 경우 에러처리
	if(ac < 2)
	{
		fprintf(stderr, "Not enough arguments\n");
		return;
	}

	if((fp = fopen(av[1], "r")) == NULL)
	{
		fprintf(stderr, "No such file on directory.\n");
		return;
	}

	while((ch = getc(fp)) != EOF)
	{
		putchar(ch);
	}

	fclose(fp);
}

