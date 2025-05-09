#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hermod.c>

typedef struct block {
    int index;
    char* content;
    int length;
    int isInContext;
    char* language;
} Block;

void generateExecutable(Block* blocks, int numBlocks, char* targetLanguage) {
    FILE* exeFile = fopen("cache.c", "w"); // file descriptor
    if (!exeFile) {
        Handle_Error("Failed to create executable file");

        return;
    }

    if (strcmp(targetLanguage, "C") == 0) {
        fprintf(exeFile, "#include <stdio.h>\nint main(void){\n");

        for (int i = 0; i < numBlocks; i++) {
            // check if block is in context and if it should get included
            if (blocks[i].isInContext) {
                fprintf(exeFile, "%s\n", blocks[i].content);
            }
            // check if the block should get included no matter, what due to the next one
            else if (i < numBlocks - 1 && blocks[i + 1].isInContext) {
                fprintf(exeFile, "%s\n", blocks[i].content);
            }
        }

        fprintf(exeFile, "return 0;\n}\n");
    }

    fclose(exeFile);
    printf("Executable file created: cash_exe.c\n");
}

void execute(){
   FILE *fp; 
   char* output = (char*)malloc(sizeof(char)*1024);
   system("gcc cache.c -o cache");
   fp = popen("./cache","r");

   if(!fp){
    Handle_Error("Could not execute process");
    exit(-1);
   }

   while (fgets(output, sizeof(output), fp) != NULL) {
    printf("%s", output);
  }

  system("rm cache; rm cache.c");

    pclose(fp);

}

int main() {
    Block blocks[] = {
        {0, "int a = 10;", 12, 0, "C"}, // out of context but included due to next
        {1, "a++; printf(\"%d\", a);", 20, 1, "C"}, // forces block 0 to get into context
        {2, "int b = 5;", 10, 0, "C"}, // Not in context AND not followed by an in-context block (excluded)
        {3, "b *= 2; printf(\"%d\", b);", 25, 1, "C"} // In context, so block 2 gets included
    };

    int numBlocks = sizeof(blocks) / sizeof(blocks[0]);
    generateExecutable(blocks, numBlocks, "C");
    execute();
return 0;
}
