//////////////////////////////////////////////utype - (auipc and lui) /////////////////////////////////////////////////////////



#include <stdio.h>

#define dest_register_shift 7
#define imm_shift 12


void luiexecute(int imm)
{
    int rd;
    rd = imm<< 12;
    printf("rd = %x\n", rd);
}

void auipcexecute(int imm)
{
    int rd, auipc_address;
    printf("enter the auipc address\n");
    scanf("%x",&auipc_address);
    rd = imm << 12;
    rd = auipc_address + rd; 
    printf("rd = %x, auipc_address = %x\n", rd, auipc_address);
}

void utype(int instruction, int opcode)
{
    int dest_register; 
    int imm;
    
    dest_register=(instruction & 0x00000f80);  
    dest_register=dest_register>>dest_register_shift;
    
    imm=(instruction & 0xfffff000);
    imm=imm>>imm_shift;
    
    printf("imm= %x, rd= %x \n",imm, dest_register);
    if (opcode == 0x00000037)
    {
        luiexecute(imm);
    }
    else if (opcode == 0x00000027)
    {
        auipcexecute(imm);
    }
}                                                                                               

int main()
{
    unsigned int instruction,opcode;
    
    printf("Enter the instruction\n");
    scanf("%x",&instruction);
    
    opcode=(instruction & 0x0000007f);
    printf("Opcode=%x\n",opcode);
    
    if((opcode==0x00000037) || (opcode == 0x00000027))
    {
        utype(instruction, opcode);
    }
    else 
    {
        printf("invalid instruction");
    }
    return 0;
}