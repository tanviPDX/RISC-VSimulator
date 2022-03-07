//U-type LUI

#include <stdio.h>

#define dest_register_shift 7
#define imm_shift 12

void utypeexecute()
{
    int rd;
	int imm,imm1; 

    printf("enter the value of Immediate\n");
    scanf("%d",&imm);

    imm1 = imm << 12;
    rd = rd | imm1; 

    printf("rd = %x imm = %d \n", rd, imm);

}

void utype(int instruction)
{
    int dest_register; 
    int imm;
    
    dest_register=(instruction & 0x00000f80);  
    dest_register=dest_register>>dest_register_shift;
    
    imm=(instruction & 0xfffff000);
    imm=imm>>imm_shift;
    
    printf("imm= %x, rd= %x \n",imm, dest_register);
    utypeexecute(); 
     
}                                                                                               

int main()
{
    unsigned int instruction,opcode;
    printf("Enter the instruction\n");
    scanf("%x",&instruction);

    opcode=(instruction & 0x0000007f);

    printf("Opcode=%x\n",opcode);
    if(opcode==0x00000037)
    {
        utype(instruction);
    }
    
    return 0;
}





