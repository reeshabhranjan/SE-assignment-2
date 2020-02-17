#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include "acl.h"
#include<string.h>


int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Enter a directory name.\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Enter only a directory name with no spaces (or escaped).\n");
        exit(1);
    }

    int n = strlen(argv[1]);

    char* dir_name = argv[1];
    
    if (argv[1][n - 1] != '/')
    {
        dir_name = (char*) malloc(n + 2);
        strcpy(dir_name, argv[1]);
        strcat(dir_name, "/");
    }

    int slash_position = strfind(dir_name, '/');

    char* parent_directory = NULL;
    n = strlen(dir_name); // BEWARE: re-assigning n
    if (slash_position != n - 1)
    {
        // this means there are more slashes in the path
        // need to extract the parent folder where the new directory is being created
        int prev_position = slash_position;
        int next_position = prev_position + 1 + strfind(dir_name + prev_position + 1, '/');

        while (next_position < n - 1)
        {
            prev_position = next_position;
            next_position = prev_position + 1 + strfind(dir_name + prev_position + 1, '/');
        }
        parent_directory = substring(dir_name, 0, prev_position + 1);   
    }
    else
    {
        parent_directory = ".";
    }


    int caller_uid = getuid();
    struct passwd* pwd = getpwuid(caller_uid);
    char* caller_name = pwd -> pw_name;

    if (!validate(caller_name, parent_directory, 10))
    {
        perror("You do not have sufficient permissions!");
        exit(1);
    }

    // TODO set permissions inherited from the parent directory
    int default_permissions = 777;
    int status = mkdir(dir_name, default_permissions);
    if (status == -1)
    {
        perror("Cannot create directory.");
        exit(1);
    }
}