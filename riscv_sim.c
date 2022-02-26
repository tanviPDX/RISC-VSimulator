#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t memory[65536];
uint32_t current_instruction;


#define BYTE1_MASK 0xFF000000
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0x0000FF00
#define BYTE4_MASK 0x000000FF

#define BYTE1_SHIFT_AMOUNT 24
#define BYTE2_SHIFT_AMOUNT 16
#define BYTE3_SHIFT_AMOUNT 8

void memory_load(char *file)
{
    FILE *f;
    uint32_t addr,data;
    char str1[10],str2[10];
    char *delimited_str;
    
    uint8_t byte1, byte2, byte3, byte4;
    
    f=fopen(file, "r");
    if(f==NULL)
      printf("\nFile open failed ");
    else 
      printf("\nFile opened successfully");
      
    do {
      fscanf(f, "%s %s", str1,str2);
      //delimited_str = strtok(s,":");
      addr = strtol(str1,0,16);
      data = strtol(str2,0,16);
      //printf("\n%s %s %u %u", str1,str2,addr,data);
      byte1 = (data & BYTE1_MASK) >> BYTE1_SHIFT_AMOUNT;
      byte2 = (data & BYTE2_MASK) >> BYTE2_SHIFT_AMOUNT;
      byte3 = (data & BYTE3_MASK) >> BYTE3_SHIFT_AMOUNT;
      byte4 = (data & BYTE4_MASK);
      //printf("\n%u %u %u %u", byte1, byte2, byte3, byte4);
      memory[addr] = byte1;
      memory[addr+1] = byte2;
      memory[addr+2] = byte3;
      memory[addr+3] = byte4;     
    }while(!feof(f));
    
    fclose(f);
    /*for(int i=0; i<65536; i++) {
      printf("Memory[%u] = %u",i,memory[i]);
    }*/
}

uint32_t instruction_fetch(uint32_t pc)
{
  uint32_t fetched_instruction;
  uint8_t instruction_1, instruction_2, instruction_3, instruction_4;
  instruction_1 = memory[pc];
  instruction_2 = memory[pc+1];
  instruction_3 = memory[pc+2];
  instruction_4 = memory[pc+3];
  fetched_instruction = (instruction_1 << BYTE1_SHIFT_AMOUNT) + (instruction_2 << BYTE2_SHIFT_AMOUNT) + (instruction_3 << BYTE3_SHIFT_AMOUNT) + instruction_4;
  printf("\nFetched Instruction: %x", fetched_instruction);
  return(fetched_instruction);
}

int main(int argc, char *argv[])
{
    unsigned int default_pc = 0, default_sp = 65535;
    char default_file_name[] = "program.mem";
    uint32_t pc, sp, ra = 0;
    char *file_name, *sp_arg, *pc_arg;
    /*FILE *f;
    uint32_t addr,data;
    char s[10],t[10];
    char *p;
    //char *s,*t;
    uint8_t memory[65536];
    uint8_t byte1, byte2, byte3, byte4;*/
    
    if(argc == 1)
    {
        file_name = default_file_name;
        pc = default_pc;
        sp = default_sp;
    }
    else
    {
        file_name = argv[3];
        sp_arg = argv[2];
        pc_arg = argv[1];
        pc = atoi(pc_arg);
        sp = atoi(sp_arg);
    }
    printf("\nProgram counter: %u",pc);
    printf("\nStack pointer: %u",sp);
    printf("\nFile name: %s",file_name);
    
    memory_load(file_name);
    current_instruction = instruction_fetch(pc);
    
    printf("\n\n");
    
    return 0;
}
