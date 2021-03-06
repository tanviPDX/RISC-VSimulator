#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t memory[65536];
uint32_t current_instruction;
int gpr[32];
//uint32_t pc;


#define BYTE1_MASK 0xFF000000
#define BYTE2_MASK 0x00FF0000
#define BYTE3_MASK 0x0000FF00
#define BYTE4_MASK 0x000000FF

#define BYTE1_SHIFT_AMOUNT 24
#define BYTE2_SHIFT_AMOUNT 16
#define BYTE3_SHIFT_AMOUNT 8

#define dest_register_shift 7
#define funct_low3_shift 12
#define source_register1_shift 15
#define source_register2_shift 20
#define funct_upper7_shift 25
#define ADD  0
#define SUB  256
#define SLL  1
#define SLT  2
#define SLTU 3
#define XOR  4
#define SRL  5
#define SRA  261
#define OR   6
#define AND  7



void rtypeexecute(int function1,unsigned int source_register1,unsigned int source_register2,unsigned int dest_register)
{
    int rd;
    int rs1;
	  int rs2;
	/* printf("enter the value of register 1\n");
     scanf("%u",&rs1);
     printf("enter the value of register 2\n");
     scanf("%u",&rs2);*/
 
    gpr[source_register1]=source_register1;     //taking the address laoding the addres into the register as value 
    gpr[source_register2]=source_register2;
    
 /* gpr[source_register1]=2147483647;       //case for maximum add and sutract
    gpr[source_register2]=2147483647;*/
    
//  gpr[source_register1]=4294967295;       // for SRA instruction to check the MSB
//  gpr[source_register2]=0;
    rs1 = gpr[source_register1];
    rs2 = gpr[source_register2];
    
    for(int i=0;i<32;i++)
    {
    printf("register %d %d\n",i,gpr[i]);
    }
    
    int rs2_mask=(rs2 & 0x0000001f);
    int rs1_msb,rs1_msb_temp,rd_temp,rd_temp1;
    int rsa_temp=0xffffffff;
	  switch(function1)
	  {
	  	case ADD : rd=rs1+rs2; 
		  	   break;
		  case SUB : rd=rs1-rs2;
		  	   break;
		  case SLL : rd=rs1<<rs2_mask;
		  	   break;
		  case SRL : rd=rs1>>rs2_mask;
			     break;
      case SRA : rd_temp1=rs1>>rs2_mask;
               rs1_msb_temp=0x80000000;
               rs1_msb=(rs1_msb_temp & rs1);
               rs1_msb=rs1_msb>>31;
               if(rs1_msb==1)
               {
               rd_temp=rsa_temp<<(32-rs2_mask);
               rd=(rd_temp1 |  rd_temp);
               }
               else
               {
               rd=rs1>>rs2_mask;
               }
			       break;
	  	case SLT : if(rs1<rs2)
               {
               rd=1;
               }
               else
               { 
               rd=0;
               }   
			     break;
		  case SLTU: if(rs2!=0)
               {
               rd=1;
               }
               else
               { 
               rd=0;
               }
			     break;
		  case XOR : rd= (rs1 ^ rs2);
			     break;
	  	case OR  : rd= (rs1 | rs2);
		  	   break;
		  case AND : rd= (rs1 & rs2); 
		  	   break;
	    default: 
	            printf("not an instruction\n");
	            rd=rd; rs1=rs1; rs2=rs2;
	         break;
	}
     
     printf("rd = %d rs1 = %d rs2 = %d \n",rd,rs1,rs2);
     gpr[dest_register]=rd;
     for(int i=0;i<32;i++)
    {
    printf("register %d %d\n",i,gpr[i]);
    }
}


void rtype(int instruction)
{
    unsigned int opcode,dest_register,funct_low3,source_register1,source_register2,funct_upper7;
    unsigned int function1;
    
    dest_register=(instruction & 0x00000f80);  
    dest_register=dest_register>>dest_register_shift;

    funct_low3=(instruction & 0x00007000);
    funct_low3=funct_low3>>funct_low3_shift;
    
    source_register1=(instruction & 0x000f8000);
    source_register1=source_register1>>source_register1_shift;
    
    source_register2=(instruction & 0x01f00000);
    source_register2=source_register2>>source_register2_shift;
    
    funct_upper7=(instruction & 0xfe000000);
    funct_upper7=funct_upper7>>funct_upper7_shift;
    
    
    printf("rd = %x funct3 = %x rs1 = %x rs2 = %x funct7 = %x\n",dest_register,funct_low3,source_register1,source_register2,funct_upper7);

    function1=funct_upper7;
    function1=function1<<3;
    function1=function1+funct_low3;
    printf("function1 = %x\n",function1);
    
    
    rtypeexecute(function1,source_register1,source_register2,dest_register);
}

void memory_load(char *file)
{
    FILE *f;
    uint32_t addr,data;
    char str1[10],str2[10];
   // char *delimited_str;
    
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
    
    unsigned int instruction,opcode;
   /* printf("enter the instruction\n");
    scanf("%d",&instruction);*/
    opcode=(current_instruction & 0x0000007f);
    printf("opcode=%x\n",opcode);
    if(opcode==0x00000033)
    {
     rtype(current_instruction);
    }
    
    return 0;
}
