
#include <stdio.h>

#define dest_register_shift 7
#define funct_shift 12
#define source_register1_shift 15
#define imm_shift 20
#define ADDI  0
#define SLTI 2
#define SLTIU 3
#define XORI  4
#define ORI  6
#define ANDI 7



void itypeexecute(int funct)
{
    
     int rd;
	 int rs1;
	 int imm;
	 printf("enter the value of register 1\n");
     scanf("%d",&rs1);
     printf("enter the value of Immediate\n");
     scanf("%d",&imm);
 
 
	switch(funct)
	{
		case ADDI : rd=rs1+imm; 
	         break;
		case SLTI : if(rs1<imm)
		            {
		                rd =1;
		            }
		            else
		            {
                        rd =0;
		            }
			 break;
		case SLTIU : if (rs1 != 0)
		            {
		                rd =1;
		            }
		            else
		            {
                        rd =0;
		            }
			 break;
		case XORI : rd= rs1^imm;
			 break;
		case ORI  : rd= rs1 | imm;
			 break;
		case ANDI : rd= rs1 & imm; 
			 break;
		default: printf("Invalid Instruction");
             break;
	}
    
 printf("rd = %d rs1 = %d imm = %d \n", rd, rs1, imm);
}


void itype(int instruction)
{
    unsigned int opcode,dest_register,funct,source_register1, imm;
    
    dest_register=(instruction & 0x00000f80);  
    dest_register=dest_register>>dest_register_shift;

    funct=(instruction & 0x00007000);
    funct=funct>>funct_shift;
    
    source_register1=(instruction & 0x000f8000);
    source_register1=source_register1>>source_register1_shift;
    
    imm=(instruction & 0xfff00000);
    imm=imm>>imm_shift;
    
    
   printf("rd = %x funct = %x\n",dest_register,funct);
   itypeexecute(funct);
   
}


int main()
{
unsigned int instruction,opcode;
printf("enter the instruction\n");
scanf("%x",&instruction);
opcode=(instruction & 0x0000007f);
printf("opcode=%x\n",opcode);
if(opcode==0x00000013)
{
  itype(instruction);
}
}
