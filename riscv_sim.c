#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t memory[65536];
uint32_t current_instruction,opcode;
uint32_t pc, sp, ra = 0;
int32_t gpr[32];

#define BYTE1_MASK 0xFF000000
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0x0000FF00
#define BYTE4_MASK 0x000000FF

#define BYTE1_SHIFT_AMOUNT 24
#define BYTE2_SHIFT_AMOUNT 16
#define BYTE3_SHIFT_AMOUNT 8

/************ Structure for Instruction fields ***************/
typedef struct
{
    int32_t imm; //offset
    uint32_t rs2; //src2
    uint32_t rs1; //src1/base
    uint32_t funct; //width
    uint32_t rd; //destination
} InstFields_t;

//***********************sign extension function***********************//
int signExtension(int imm) 
{
    int imm_extend = (0x000000FFF & imm);
    int mask = 0x00000800;
    
    if (mask & imm_extend) 
    {
        imm_extend |= 0xFFFFF000;
    }
    return imm_extend;
}


/************JTYPE Functions********************/

/******** Mask, Shift Amount, and Function Type amount J-type instruction *********/
#define J_OFFSET3_FIELD 0x000ff000
#define J_OFFSET1_FIELD 0x7fe00000
#define J_OFFSET2_FIELD 0x00100000
#define J_OFFSET4_FIELD 0x80000000
#define J_DEST_FIELD    0x00000f80
#define J_OFFSET3_SHAMT 12
#define J_OFFSET1_SHAMT 21
#define J_OFFSET2_SHAMT 25
#define J_OFFSET4_SHAMT 31
#define J_DEST_SHAMT    7

/************ Sign extension for J type ***********/
int32_t signExtension_Jtype(int32_t imm) 
{
    int32_t imm_extend = (0x001FFFFF & imm);
    int32_t mask = 0x00100000;
    
    if (mask & imm_extend) 
    {
    
        imm_extend |= 0xFFE00000;
    }
    return imm_extend;
}

/************* Extracting JType Instruction field *************/
void JTypeInst(uint32_t current_instruction, InstFields_t *Inst_fields){

    Inst_fields->imm = 0x0;
    int imm1, imm2, imm3, imm4;

    Inst_fields->rd= (current_instruction & J_DEST_FIELD) >> J_DEST_SHAMT;

    imm1 = (current_instruction & J_OFFSET1_FIELD) >> J_OFFSET1_SHAMT;
    imm2 = (current_instruction & J_OFFSET2_FIELD) >> J_OFFSET2_SHAMT;
    imm3 = (current_instruction & J_OFFSET3_FIELD) >> J_OFFSET3_SHAMT;
    imm4 = (current_instruction & J_OFFSET4_FIELD) >> J_OFFSET4_SHAMT;
    printf("Imm1: %x Imm2: %x Imm3: %x Imm4: %x \n", imm1,imm2,imm3,imm4);
    Inst_fields->imm = Inst_fields->imm + (imm1 << 1) + (imm2 << 11) + (imm3 << 12) + (imm4 << 20);
    //printf("Imm (before sign extension): %x\n ", JFields->imm);
    Inst_fields->imm = signExtension_Jtype(Inst_fields->imm);

    printf("JType Instruction Fields- Immediate field: %x \nDestination register addr: %x \n", Inst_fields->imm, Inst_fields->rd);
}

/************ Executing JType Instruction*******************/
void JTypeExecute(int32_t* GPR,  InstFields_t* InstFields, uint8_t* memory, uint32_t* PC) {
    int32_t target; 
    target = *PC + Inst_fields->imm;
    gpr[Inst_fields->rd] = *PC+4;
    *PC += target; 
    
    printf("JType Execute- Target = %x \n", target);
}
    
/******************* END JTYPE ******************************/    

/****************** JAL Instruction *************************/

/******** Mask, Shift Amount, and Function Type amount JAL instruction *********/
#define JAL_IMM_FIELD    0xfff00000
#define JAL_DEST_FIELD   0x00000f80
#define JAL_RS1_FIELD   0x000f8000
#define JAL_FUNC_FIELD   0x00007000 
#define JAL_IMM_SHAMT    20
#define JAL_DEST_SHAMT   7
#define JAL_SRC1_SHAMT   15
#define JAL_FUNC_SHAMT   12

/******** Extracting JAL Instruction field **************/
void JALInst(uint32_t current_instruction, InstFields_t *JALfields){

    //JALfields->imm = 0x0;
    JALfields->rd= (current_instruction & JAL_DEST_FIELD) >> JAL_DEST_SHAMT;
    JALfields->rs1= (current_instruction & JAL_RS1_FIELD) >> JAL_SRC1_SHAMT;
    JALfields->funct= (current_instruction & JAL_FUNC_FIELD) >> JAL_FUNC_SHAMT;
    JALFields->imm = (current_instruction & JAL_IMM_FIELD) >> JAL_IMM_SHAMT;
    JALFields->imm = signExtension(JALFields->imm);

    printf("JAL Instruction Fields- Immediate field: %x Destination register addr: %x Source register addr: %x Func: %x \n", JALFields->imm, JALFields->rd, JALFields->rs1, JALFields->funct);
}


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
      addr = strtoll(str1,0,16);
      data = strtoll(str2,0,16);
      printf("\n%s %s %x %x", str1,str2,addr,data);
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
      printf(" Memory[%u] = %x",i,memory[i]);
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
    //uint32_t pc, sp, ra = 0;
    char *file_name, *sp_arg, *pc_arg;
    InstFields_t Inst_Fields;
    int flag = 1;
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
    
    opcode=(current_instruction & 0x0000007f);
    printf("opcode=%x\n",opcode);
    
    if(opcode==0x0000006f)
    {
    JTypeInst(current_instruction, &Inst_Fields);
    JTypeExecute(&Inst_Fields, &flag);
    }
    else if(opcode==0x00000067)
    {
    //current_inst = JALInst(current_instruction);
    //JALExecute(current_inst);
    }
    
    printf("\n\n");
    
    return 0;
}
