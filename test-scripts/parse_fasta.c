#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

ssize_t parse_fasta(char *filename, char **string)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    struct stat st;
    fstat(fileno(fp), &st);
    //On some other systems, the st_size return type of off_t might be only 32 bits
    size_t size = (size_t) st.st_size;

    ssize_t line_len;
    size_t len = 0;
    size_t space = size;
    //Enough to hold the file, so more than enough to hold just the sequence   
    char *genome = (char*) calloc(size, sizeof(char));   
    char *line = genome;

    //This is improper because getline expects a pointer that can be reallocated. 
    //But, by definition this is enough space for the whole file, so it should be fine
    while ((line_len = getline(&line, &space, fp)) != -1) {
        if (line_len > 0 && line[0] != '>'){
	    if (line[line_len-1] == '\n'){
                line_len--;
	    }
	    len += line_len;
	    space -= line_len;
	}
	line = genome+len;
    }
    //If the last header is longer than the last sequence, not properly terminated
    genome[len] = '\0';

    fclose(fp);
    *string = genome;
    return len;
}

ssize_t parse_fasta_upper(char *filename, char **string)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    struct stat st;
    fstat(fileno(fp), &st);
    //On some other systems, the st_size return type of off_t might be only 32 bits
    size_t size = (size_t) st.st_size;

    ssize_t line_len;
    size_t len = 0;
    size_t space = 0;
    //Enough to hold the file, so more than enough to hold just the sequence
    char *genome = (char*) calloc(size, sizeof(char));
    char *line = NULL;

    while ((line_len = getline(&line, &space, fp)) != -1) {
	if (line_len > 0 && line[0] != '>'){
            if (line[line_len-1] == '\n'){
                line_len--;
            }
	    for (ssize_t i = 0; i < line_len; i++) {
                genome[len] = toupper(line[i]);
		len ++;
	    }
        }
    }
    //If the last header is longer than the last sequence, not properly terminated
    free(line);

    fclose(fp);
    *string = genome;
    return len;
}

/*
* int main(int argc, char *argv[]){
*     char *string = NULL;
*     ssize_t line_len = parse_fasta_upper(argv[1], &string);
*     printf ("the size of sequence read is  %ld\n", line_len);
*     FILE *fp = fopen(argv[2], "wb");
*     if (fp != NULL)
*     {
*         fputs(string, fp);
*         fclose(fp);
*     }
* }
*/




