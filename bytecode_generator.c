#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NAME 20
#define MAX_TABLE 1000


typedef struct {
    char name[MAX_NAME];
    int value;
}code;


code hash_table[MAX_TABLE];
int global_hash_counter = 0;

unsigned int hash_func(char* name) {
    int length = strnlen(name, MAX_NAME);
    unsigned int hash = 0;
    for (int i = 0; i < length; i++)
    {
        hash += name[i];
        hash = (hash * name[i]) % MAX_TABLE;
    }
    return hash;

}

void inithashtable() {
    for (int i = 0; i < MAX_TABLE; i++)
    {
        for (int y = 0; y < MAX_NAME; y++)
        {
            hash_table[i].name[y] = 0;
        }
        hash_table[i].value = 0;
    }
}

int hashtableget(char* name) {

    int index = hash_func(name);
    for (int i = 0; i < MAX_TABLE; i++)
    {
        int try = ( index+i) % MAX_TABLE;
        if (hash_table[try].name[0] != 0 && strncmp(hash_table[try].name, name,
            MAX_TABLE) == 0) {
            return hash_table[try].value;
        }
    }

    return MAX_TABLE + 1;
}

int hashtableinsert(code cd) {

    if (cd.name[0] == 0) return 3;
    int index = hash_func(cd.name);

    for (int i = 0; i < MAX_TABLE; i++)
    {
        int try = (i + index) % MAX_TABLE;
        if (hash_table[try].name[0] == 0)
        {
            hash_table[try] = cd;
            global_hash_counter++;
            return 0;
        }
    }

    return 3;

}


int fillcodes(char* opcodes, long size) {
    code cd = { NULL,0 };
    int x = 0;
    int i;
    char number[10] = {0};

    for (i = 0; i < size; i++)
    {
        if (opcodes[i] == '=')
        {
            x = 0;

            while(i < size)
            {
                if (x > 10) return 3;
                i++;
                if (opcodes[i] == '\n' || opcodes[i] == ',' || opcodes[i] == '\r')
                {
                    if (opcodes[i + 1] == '\n' || opcodes[i + 1] == ',' || opcodes[i + 1] == '\r') {
                        continue;
                    }

                    i++;

                    cd.value = atoi(number);
                    for (int y = 0; y < 10; y++)
                    {
                        number[y] = 0;

                    }

                    hashtableinsert(cd);
                    for (int y = 0; y < MAX_NAME; y++)
                    {
                        cd.name[y] = 0;

                    }
                    cd.value = 0;
                    x = 0;
                    break;
                }
                if (i == size)
                {
                    return 0;
                }
                if (!isdigit(opcodes[i]))
                {
                    continue;
                }

                number[x] = opcodes[i];
                x++;
                                
            }

        }

        if (x > MAX_NAME) return 3;
        cd.name[x] = opcodes[i];
        x++;
    }


    return 0;
}

int gencodes(char *codes,long size) {
    FILE* out; fopen_s(&out, "output", "wb");
    if (out == NULL) return 3;

    char *ptr_array = (char*)malloc(sizeof(char *) * global_hash_counter);
    int x = 0;
    char tempbuff[MAX_NAME] = {0};

    for (int i = 0; i < size; i++)
    {
        if (codes[i] == '\n' || codes[i] == ',' || codes[i] == '\r')
        {
            if (codes[i + 1] == '\n' || codes[i + 1] == ',' || codes[i + 1] == '\r') {
                continue;
            }
            int opcode = hashtableget(tempbuff);
            printf("%d\n", opcode);
            fwrite(&opcode, sizeof(int), 1, out);
            x = 0;
            for (int c = 0; c < MAX_NAME; c++)
            {
                tempbuff[c] = 0;
            }
        }
        else {
            tempbuff[x] += codes[i];
            x++;
        }


    }

    free(ptr_array);
    fclose(out);
}

int main(int argc, char *argv[])
{
   if( argc <= 2 ) {
      printf("Usage: bytecode_gen.exe <opcodes.txt> <codes.txt> %s\n", argv[1]);
       return 2;
   }

    
    inithashtable();
    char* codesbuffer;
    char* opcodesbuffer;
    long size;
    FILE* opcodes;
    FILE* codes;


    fopen_s(&opcodes,argv[1], "rb");
    fopen_s(&codes,argv[2], "rb");


    if(opcodes == NULL || codes == NULL) return 3;

    fseek(opcodes, 0L, SEEK_END);
    size = ftell(opcodes);
    if (size <= 0) return 3;
    fseek(opcodes, 0L, SEEK_SET);
    opcodesbuffer = (char*)malloc(size * sizeof(char));
    fread(opcodesbuffer, sizeof(char), size, opcodes);

    if (fillcodes(opcodesbuffer, size) == 3) {
        //error
        free(opcodesbuffer);
        fclose(opcodes);
        fclose(codes);
        return 3;
    }

    size = 0;

    fseek(codes, 0L, SEEK_END);
    size = ftell(codes);
    if (size <= 0) return 3;
    fseek(codes, 0L, SEEK_SET);
    codesbuffer = (char*)malloc(size * sizeof(char));
    fread(codesbuffer, sizeof(char), size, codes);

    if (codesbuffer == NULL || opcodesbuffer == NULL) return 3;
    

    free(opcodesbuffer);
    fclose(opcodes);
    fclose(codes);

    gencodes(codesbuffer, size);
    free(codesbuffer);
    return 0;
}
