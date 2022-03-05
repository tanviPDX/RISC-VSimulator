
#include <stdio.h>

#define dest_register_shift 7
#define funct_shift 12
#define source_register1_shift 15
#define imm_shift 20
#define shamt_shift 20
#define funct_7_shift 25
#define ADDI 0
#define SLLI 1
#define SLTI 2
#define SLTIU 3
#define XORI 4
#define SRLI 5
#define ORI  6
#define ANDI 7
#define SRAI 261

int signExtension(int imm)  
{
    int imm_extend = (0x000000FFF & imm);
    int mask = 0x00000800;

    if (mask & imm_extend) 
    {
        imm_extend += 0xFFFFF000;
    }
    return imm_extend;
}

void itypeexecute1(int funct)
{
    int rd;
	int rs1;
	int imm, imm_extend;

    printf("enter the value of register 1\n");
    scanf("%d",&rs1);
    printf("enter the value of Immediate\n");
    scanf("%d",&imm);
    
	switch(funct)
	{
		case ADDI : rd=rs1+signExtension(imm); 
	        break;
		case SLTI : if(rs1<signExtension(imm))
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
		case XORI : rd= rs1^signExtension(imm);
		    break;
		case ORI  : rd= rs1 | signExtension(imm);
	        break;
		case ANDI : rd= rs1 & signExtension(imm); 
		    break;
		default: printf("Invalid Instruction");
            break;
	}

 printf("rd = %d rs1 = %d imm = %d \n", rd, rs1, imm);

}

void itypeexecute2(int function, int shamt)
{
    int rd;
	int rs1;
	int imm;
    int rs1_msb,rs1_msb_temp,rd_temp,rd_temp1;
    int rsa_temp=0xffffffff;
    
    printf("enter the value of register 1\n");
    scanf("%d",&rs1);
     
    switch(function)
	{
		case SLLI : rd=rs1<<shamt; 
	        break;
	    case SRLI : rd=rs1>>shamt;
	        break;
	    case SRAI : rd_temp1=rs1>>shamt;
               rs1_msb_temp=0x80000000;
               rs1_msb=(rs1_msb_temp & rs1);
               rs1_msb=rs1_msb>>31;
               if(rs1_msb==1)
               {
               rd_temp=rsa_temp<<(32-shamt);
               rd=(rd_temp1 |  rd_temp);
               }
               else
               {
               rd=rs1>>shamt;
               }
	        break;
	    default:   printf("Invalid Instruction");
            break;
     
    }
     
    printf("rd = %d rs1 = %d \n", rd, rs1);
}

void itype(int instruction)
{
    unsigned int opcode,dest_register,funct,source_register1,shamt,funct_7;
    int imm;
    unsigned int function;
    
    dest_register=(instruction & 0x00000f80);  
    dest_register=dest_register>>dest_register_shift;

    funct=(instruction & 0x00007000);
    funct=funct>>funct_shift;
    
    source_register1=(instruction & 0x000f8000);
    source_register1=source_register1>>source_register1_shift;
    
    imm=(instruction & 0xfff00000);
    imm=imm>>imm_shift;
    
    if ((funct == 1) || (funct == 5))
    {
        shamt = (instruction & 0x01f00000);
        shamt = shamt>>shamt_shift;
        funct_7 = (instruction & 0xfe000000);
        funct_7 = funct_7>>funct_7_shift;
        function = funct_7;
        function = function<<3;
        function = function+funct;
        printf("function = %x, rd= %x shamt=%x \n",function,dest_register,shamt);
        itypeexecute2(function,shamt);
    }
    else
    {
        printf("rd = %x funct = %x \n",dest_register,funct);
        itypeexecute1(funct);
    }
}

int main()
{
    unsigned int instruction,opcode;
    printf("Enter the instruction\n");
    scanf("%x",&instruction);
    opcode=(instruction & 0x0000007f);
    printf("Opcode=%x\n",opcode);
    if(opcode==0x00000013)
    {
        itype(instruction);
    }
    return 0;
}
