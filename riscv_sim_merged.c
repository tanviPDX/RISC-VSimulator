/* Program Description: Decoding S and B type Instructions
   To Do: Need to implement execution function
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/******** Mask and Shift Amount for loading into Memory *********/
#define BYTE1_MASK          0xFF000000  //Masks
#define BYTE2_MASK          0x00FF0000
#define BYTE3_MASK          0x0000FF00
#define BYTE4_MASK          0x000000FF
#define BYTE1_SHIFT_AMOUNT  24          //Shift Amounts
#define BYTE2_SHIFT_AMOUNT  16
#define BYTE3_SHIFT_AMOUNT  8

/******** Mask for Opcode *********/
#define OPCODE_FIELD    0x0000007f

/******** Mask, Shift Amount, and Function Type for R-type instruction *********/
#define R_DEST_FIELD    0x00000f80
#define R_DEST_SHAMT    7
#define R_FUNCT3_FIELD  0x00007000
#define R_FUNCT3_SHAMT  12
#define R_SRC1_FIELD    0x000f8000
#define R_SRC1_SHAMT    15
#define R_SRC2_FIELD    0x01f00000
#define R_SRC2_SHAMT    20
#define R_FUNCT7_FIELD  0xfe000000
#define R_FUNCT7_SHAMT  25
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

/******** Mask, Shift Amount, and Function Type for S-type instruction *********/
#define S_OFFSET1_FIELD 0x00000f80  //Masks
#define S_OFFSET2_FIELD 0xfe000000
#define S_SRC_FIELD     0x01f00000
#define S_BASE_FIELD    0x000f8000
#define S_WIDTH_FIELD   0x00007000
#define S_OFFSET1_SHAMT 7           //Shift Amounts
#define S_OFFSET2_SHAMT 25
#define S_SRC_SHAMT     20
#define S_BASE_SHAMT    15
#define S_WIDTH_SHAMT   12
#define SB              0x0         //Function Type
#define SH              0x1
#define SW              0x2

/******** Mask, Shift Amount, and Function Type amount B-type instruction *********/
#define B_OFFSET3_FIELD 0x00000080  //Masks
#define B_OFFSET1_FIELD 0x00000f00
#define B_OFFSET2_FIELD 0x7e000000
#define B_OFFSET4_FIELD 0x80000000
#define B_SRC1_FIELD    0x000f8000
#define B_SRC2_FIELD    0x01f00000
#define B_WIDTH_FIELD   0x00007000
#define B_OFFSET3_SHAMT 7           //Shift Amounts
#define B_OFFSET1_SHAMT 8
#define B_OFFSET2_SHAMT 25
#define B_OFFSET4_SHAMT 31
#define B_SRC1_SHAMT    15
#define B_SRC2_SHAMT    20
#define B_WIDTH_SHAMT   12
#define BEQ             0x0         //Function Type
#define BNE             0x1
#define BLT             0x4
#define BGE             0x5
#define BLTU            0x6
#define BGEU            0x7

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

/********* Mask, Shift Amount, and Function Type amount Load instruction *********/
#define L_DEST_FIELD    0x00000f80
#define L_DEST_SHAMT    7
#define L_FUNCT3_FIELD  0x00007000
#define L_FUNCT3_SHAMT  12
#define L_SRC1_FIELD    0x000f8000
#define L_SRC1_SHAMT    15
#define L_IMMI_FIELD    0xfff00000
#define L_IMMI_SHAMT    20
#define LB 0
#define LH 1
#define LW 2
#define LBU 4
#define LHU 5

/************ Global variables *************/
uint8_t memory[65536];
uint32_t current_instruction;

/************ Structure for Instruction fields ***************/
typedef struct
{
    uint32_t opcode;    //opcode
    uint32_t imm;       //offset
    uint32_t rs2;       //src2
    uint32_t rs1;       //src1/base
    uint32_t rd;        //dest
    uint32_t funct3;    //width
    uint32_t funct7;
} InstFields_t;

/************ Loading Program into Memory ***************/
void memory_load(char *file)
{
    FILE *f;
    uint32_t addr,data;
    char str1[10],str2[10];

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
      data = strtoll(str2,0,16);
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
    printf("\nData in the memory:\n");
    for(int i=0; i<4; i++) {
      printf(" Memory[%u] = %x\n",i,memory[i]);
    }
}

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


/************ Print Register Values ***************/
void PrintReg(int32_t* GPR, uint32_t* PC){
    uint32_t i;
    printf("\n****** Register values ******\n");
    for(i = 0; i <= 31; i++){
      printf("x%d : %x\n", i, GPR[i]);
    }
    printf("PC : %x\n", *PC);
}

/************* Extracting R-type Instruction fields *************/
void RTypeInst(uint32_t current_instruction, InstFields_t *RFields)
{
	  RFields->rd = (current_instruction & R_DEST_FIELD) >> R_DEST_SHAMT;
    RFields->funct3 = (current_instruction & R_FUNCT3_FIELD) >> R_FUNCT3_SHAMT;
    RFields->rs1 = (current_instruction & R_SRC1_FIELD) >> R_SRC1_SHAMT;
    RFields->rs2 = (current_instruction & R_SRC2_FIELD) >> R_SRC2_SHAMT;
    RFields->funct7 = (current_instruction & R_FUNCT7_FIELD) >> R_FUNCT7_SHAMT;
   
    printf("rd = %d funct3 = %d rs1 = %d rs2 = %d funct7 = %d\n",RFields->rd,RFields->funct3,RFields->rs1,RFields->rs2,RFields->funct7);
}

/************* Extracting B-type Instruction fields *************/
void BTypeInst(uint32_t current_instruction, InstFields_t *BFields){
    int imm1, imm2, imm3, imm4;

    BFields->imm = 0x0;

    BFields->funct3 = (current_instruction & B_WIDTH_FIELD) >> B_WIDTH_SHAMT;
    BFields->rs1 = (current_instruction & B_SRC1_FIELD) >> B_SRC1_SHAMT;
    BFields->rs2 = (current_instruction & B_SRC2_FIELD) >> B_SRC2_SHAMT;

    imm1 = (current_instruction & B_OFFSET1_FIELD) >> B_OFFSET1_SHAMT;
    imm2 = (current_instruction & B_OFFSET2_FIELD) >> B_OFFSET2_SHAMT;
    imm3 = (current_instruction & B_OFFSET3_FIELD) >> B_OFFSET3_SHAMT;
    imm4 = (current_instruction & B_OFFSET4_FIELD) >> B_OFFSET4_SHAMT;
    BFields->imm = BFields->imm + (imm1 << 1) + (imm2 << 5) + (imm3 << 11) + (imm4 << 12);

    if(BFields->imm & (1<<12)){
        BFields->imm = BFields->imm + 0xffffe000;
    }

    printf("Immediate field: %x \nSource register 1 addr: %x \nSource register 2 addr: %x \nFunction: %x\n", BFields->imm, BFields->rs1, BFields->rs2, BFields->funct3);
}

/************* Extracting S-type Instruction fields *************/
void STypeInst(uint32_t current_instruction, InstFields_t *SFields){
    int imm1, imm2;
    char imm1s[20];
    char imm2s[20];

    SFields->imm = 0x0;

    SFields->funct3 = (current_instruction & S_WIDTH_FIELD) >> S_WIDTH_SHAMT;
    SFields->rs1 = (current_instruction & S_BASE_FIELD) >> S_BASE_SHAMT;
    SFields->rs2 = (current_instruction & S_SRC_FIELD) >> S_SRC_SHAMT;

    imm1 = (current_instruction & S_OFFSET1_FIELD) >> S_OFFSET1_SHAMT;
    imm2 = (current_instruction & S_OFFSET2_FIELD) >> S_OFFSET2_SHAMT;
    SFields->imm = SFields->imm + imm1 + (imm2 << 5);

    if(SFields->imm & (1<<11)){
        SFields->imm = SFields->imm + 0xfffff000;
    }

    printf("Immediate field: %x \nBase register addr: %x \nSource register addr: %x \nFunction: %x\n", SFields->imm, SFields->rs1, SFields->rs2, SFields->funct3);
}

/************* Extraction load instruction fields ****************/
void LoadInst(uint32_t current_instruction, InstFields_t *LFields)
{
    
    LFields->rd     = (current_instruction & L_DEST_FIELD)   >> L_DEST_SHAMT  ;
    LFields->funct3 = (current_instruction & L_FUNCT3_FIELD) >> L_FUNCT3_SHAMT;
    LFields->rs1    = (current_instruction & L_SRC1_FIELD)   >> L_SRC1_SHAMT  ;   
    LFields->imm    = (current_instruction & L_IMMI_FIELD)   >> L_IMMI_SHAMT  ;
    //loadexecute(funct,source_register1,dest_register,imm);
}

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
void JTypeInst(uint32_t current_instruction, InstFields_t *Jfields){

    Jfields->imm = 0x0;
    int imm1, imm2, imm3, imm4;

    Jfields->rd= (current_instruction & J_DEST_FIELD) >> J_DEST_SHAMT;

    imm1 = (current_instruction & J_OFFSET1_FIELD) >> J_OFFSET1_SHAMT;
    imm2 = (current_instruction & J_OFFSET2_FIELD) >> J_OFFSET2_SHAMT;
    imm3 = (current_instruction & J_OFFSET3_FIELD) >> J_OFFSET3_SHAMT;
    imm4 = (current_instruction & J_OFFSET4_FIELD) >> J_OFFSET4_SHAMT;
    printf("Imm1: %x Imm2: %x Imm3: %x Imm4: %x \n", imm1,imm2,imm3,imm4);
    Jfields->imm = Jfields->imm + (imm1 << 1) + (imm2 << 11) + (imm3 << 12) + (imm4 << 20);
    //printf("Imm (before sign extension): %x\n ", JFields->imm);
    Jfields->imm = signExtension_Jtype(Jfields->imm);

    printf("JType Instruction Fields- Immediate field: %x \nDestination register addr: %x \n", Jfields->imm, Jfields->rd);
}

/************* Execute function for R-type Instruction *************/
void RTypeExecute(int32_t* GPR,  InstFields_t* InstFields, uint8_t* memory, uint32_t* PC)
{
	  int function;  
	  function = (InstFields->funct7 << 3) + InstFields->funct3;
    printf("function = %x\n",function);
    
    int rs2_mask=(GPR[InstFields->rs2] & 0x0000001f);
    int rs1_msb,rs1_msb_temp,rd_temp,rd_temp1;
    int rsa_temp=0xffffffff;
	  switch(function)
	  {
  	case ADD : GPR[InstFields->rd] = GPR[InstFields->rs1] + GPR[InstFields->rs2]; 
		    		   break;
		case SUB : GPR[InstFields->rd] = GPR[InstFields->rs1] - GPR[InstFields->rs2];
		      	   break;
		case SLL : GPR[InstFields->rd] = GPR[InstFields->rs1] << rs2_mask;
		    	     break;
		case SRL : GPR[InstFields->rd] =GPR[InstFields->rs1] >> rs2_mask;
		    	     break;
		case SRA : rd_temp1=GPR[InstFields->rs1]>>rs2_mask;
				       rs1_msb_temp=0x80000000;
				       rs1_msb=(rs1_msb_temp & GPR[InstFields->rs1]);
               rs1_msb=rs1_msb>>31;
               if(rs1_msb==1)
				       {
                  rd_temp=rsa_temp<<(32-rs2_mask);
						      GPR[InstFields->rd] =(rd_temp1 |  rd_temp);
					      }
				       else
					      {
						      GPR[InstFields->rd] = GPR[InstFields->rs1] >> rs2_mask;
					      }
			         break;
 	  case SLT : if(GPR[InstFields->rs1]<GPR[InstFields->rs2])
					      {
						      GPR[InstFields->rd] =1;
					      }
               else
					      { 
						      GPR[InstFields->rd] =0;
					      }   
				       break;
		case SLTU: if(GPR[InstFields->rs2]!=0)
					      {
						      GPR[InstFields->rd] =1;
				        }
				       else
					      { 
					        GPR[InstFields->rd] =0;
					      }
				       break;
		case XOR : GPR[InstFields->rd] = (GPR[InstFields->rs1] ^ GPR[InstFields->rs2]);
			         break;
 	  case OR  : GPR[InstFields->rd] = (GPR[InstFields->rs1] | GPR[InstFields->rs2]);
				       break;
		case AND : GPR[InstFields->rd] = (GPR[InstFields->rs1] & GPR[InstFields->rs2]); 
		  	       break;
    default  : printf("not an instruction\n");
               GPR[InstFields->rd] = GPR[InstFields->rd]; GPR[InstFields->rs1]=GPR[InstFields->rs1]; GPR[InstFields->rs2]=GPR[InstFields->rs2];
				       break;
  	}
     
    // printf("rd = %d rs1 = %d rs2 = %d \n",GPR[InstFields->rd],GPR[InstFields->rs1],GPR[InstFields->rs2]);
    // gpr[dest_register]=rd;
    /*for(int i=0;i<32;i++)
    {
    printf("register %d %d\n",i,GPR[i]);
    }*/
    *PC = *PC + 4;
}

void LoadExecute(int32_t* GPR,  InstFields_t* InstFields, uint8_t* memory, uint32_t* PC)
{
     int rd_extend,rd_temp,effective_address,count;
     
	switch(InstFields->funct3)
	{
   case LB: effective_address= GPR[InstFields->rs1] + signExtension(InstFields->imm);
	           GPR[InstFields->rd]=memory[effective_address]; 
	           
               rd_extend = (0x0000000FF & GPR[InstFields->rd]); 
                         
               if(GPR[InstFields->rd] & 0x00000080)
               {
               GPR[InstFields->rd]=(rd_extend | 0xffffff00);
               }
               else 
               {
               GPR[InstFields->rd]=rd_extend;
               }
		break;
		
	 case LH: effective_address= GPR[InstFields->rs1] + signExtension(InstFields->imm);
	             GPR[InstFields->rd]=memory[effective_address];
               rd_temp=(GPR[InstFields->rd]<<8);
               effective_address +=1;
               GPR[InstFields->rd]=memory[effective_address];
               GPR[InstFields->rd] +=rd_temp;
               rd_extend = (0x00000FFFF & GPR[InstFields->rd]); 
               
               if(GPR[InstFields->rd] & 0x00008000)
               {
               GPR[InstFields->rd]=(rd_extend | 0xffff0000);
               }
               else 
               {
               GPR[InstFields->rd]=rd_extend;
               }
		break;
	 case LW: effective_address= GPR[InstFields->rs1] + signExtension(InstFields->imm);
               GPR[InstFields->rd]=memory[effective_address];
               
               count=3;
               while (count!=0)
               {
               rd_temp=(GPR[InstFields->rd]<<8);
               effective_address +=1;
               GPR[InstFields->rd]=memory[effective_address];
               GPR[InstFields->rd] +=rd_temp;
               count -=1;
               }
		break;
	 case LBU: effective_address= GPR[InstFields->rs1] + (InstFields->imm & 0x00000fff);
	             GPR[InstFields->rd]=memory[effective_address];
		break;
	 case LHU: effective_address= GPR[InstFields->rs1] + (InstFields->imm & 0x00000fff);
               GPR[InstFields->rd]=memory[effective_address];
               rd_temp=GPR[InstFields->rd]<<8;
               effective_address +=1;
               GPR[InstFields->rd]=memory[effective_address];
               GPR[InstFields->rd] +=rd_temp;
		break;
	default: printf("Invalid Instruction");
        break;  
	}
    printf("rd = %x rs1 = %x imm = %x effective_address = %x\n", InstFields->rd, InstFields->rs1, InstFields->imm, effective_address);
}

/************* Fetch an Instruction *************/
uint32_t instruction_fetch(uint32_t pc){
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

/************* Decode the Current Instruction ************/
void Decode(uint32_t current_instruction, InstFields_t *InstFields){
    uint32_t Opcode;
    printf("\nStarted Decoding\n");

    //identify the opcode and call the required instruction type
    Opcode = current_instruction & OPCODE_FIELD;
    InstFields->opcode = Opcode;
    printf("\nOpcode for Current Instruction %x: %x",current_instruction, Opcode);

    switch(Opcode){
        case 0x33:  printf("\n*****R-type Instruction*****\n");
                    RTypeInst(current_instruction, InstFields);//function call to decode R-type instruction
                    break;
        case 0x23:  printf("\n*****S-type Instruction*****\n");
                    STypeInst(current_instruction, InstFields);//function call to decode S-type instruction
                    break;
        case 0x63:  printf("\n*****B-type Instruction*****\n");
                    BTypeInst(current_instruction, InstFields);//function call to decode B-type instruction
                    break;
        case 0x6f:  printf("\n******J-type Instruction*****\n");
                    JTypeInst(current_instruction, InstFields);//function call to decode J-type instruction
                    break;
        case 0x03:  printf("\n******Load Instruction*******\n");
                    LoadInst(current_instruction, InstFields);//function call to decode load instruction
                    break;
        default:    printf("\n*****Other Instruction*****\n");
    }

}

/************* Execute the Current Instruction ************/
void ExecuteInstruction(int32_t* GPR,  InstFields_t* InstFields, uint8_t* memory, uint32_t* PC){
    switch(InstFields->opcode){
        case 0x33:  printf("\nExecution of R-Type Instruction\n");
                    RTypeExecute(GPR, InstFields, memory, PC); 
                    break;
        case 0x03:  printf("\nExecution of Load Instruction\n");
                    LoadExecute(GPR, InstFields, memory, PC);
                    break;
        case 0x23:  printf("\nExecution of S-type Instruction\n");
                    switch(InstFields->funct3){
                        case SW: printf("SW Instruction\n");
                                 printf("Base Register: %x, Base Address: %x, Offset: %x, Byte Address: %x, Whole Data: %x\n", InstFields->rs1, GPR[InstFields->rs1], InstFields->imm, ((InstFields->imm)+(GPR[InstFields->rs1])), GPR[InstFields->rs2]);
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1])] = (GPR[InstFields->rs2] & BYTE1_MASK) >> BYTE1_SHIFT_AMOUNT;
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1]) + 1] = (GPR[InstFields->rs2] & BYTE2_MASK) >> BYTE2_SHIFT_AMOUNT;
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1]) + 2] = (GPR[InstFields->rs2] & BYTE3_MASK) >> BYTE3_SHIFT_AMOUNT;
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1]) + 3] = GPR[InstFields->rs2] & BYTE4_MASK;
                                 for(int i=(InstFields->imm)+(GPR[InstFields->rs1]); i<(InstFields->imm)+(GPR[InstFields->rs1])+4; i++) {
                                    printf("Memory[%d] = %x\n",i,memory[i]);
                                 }
                                 *PC = *PC + 4;
                                 break;
                        case SB: printf("SB Instruction\n");
                                 printf("Base Register: %x, Base Address: %x, Offset: %x, Byte Address: %x, Whole Data: %x\n", InstFields->rs1, GPR[InstFields->rs1], InstFields->imm, ((InstFields->imm)+(GPR[InstFields->rs1])), GPR[InstFields->rs2]);
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1])] = GPR[InstFields->rs2] & BYTE4_MASK;
                                 printf("Memory[%d] = %x\n", (InstFields->imm)+(GPR[InstFields->rs1]), memory[(InstFields->imm)+(GPR[InstFields->rs1])]);
                                 *PC = *PC + 4;
                                 break;
                        case SH: printf("SH Instruction\n");
                                 printf("Base Register: %x, Base Address: %x, Offset: %x, Byte Address: %x, Whole Data: %x\n", InstFields->rs1, GPR[InstFields->rs1], InstFields->imm, ((InstFields->imm)+(GPR[InstFields->rs1])), GPR[InstFields->rs2]);
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1])] = (GPR[InstFields->rs2] & BYTE3_MASK) >> BYTE3_SHIFT_AMOUNT;
                                 memory[(InstFields->imm)+(GPR[InstFields->rs1]) + 1] = GPR[InstFields->rs2] & BYTE4_MASK;
                                 printf("Memory[%d] = %x\n", (InstFields->imm)+(GPR[InstFields->rs1]), memory[(InstFields->imm)+(GPR[InstFields->rs1])]);
                                 *PC = *PC + 4;
                                 break;
                        default: printf("Error: Cannot find the instruction based on opcode and func3\n");
                    }
                    break;
        case 0x63:  printf("\nExecution of B-type Instruction\n");
                    switch(InstFields->funct3){
                        case BEQ : printf("BEQ Instruction\n");
                                   if(GPR[InstFields->rs1] == GPR[InstFields->rs2])
                                    *PC = *PC + InstFields->imm;
                                   else
                                    *PC = *PC + 4;
                                   break;
                        case BNE : printf("BNE Instruction\n");
                                   if(GPR[InstFields->rs1] != GPR[InstFields->rs2])
                                    *PC = *PC + InstFields->imm;
                                   else
                                    *PC = *PC + 4;
                                   break;
                        case BLT : printf("BLT Instruction\n");
                                   if(GPR[InstFields->rs1] < GPR[InstFields->rs2])
                                    *PC = *PC + InstFields->imm;
                                   else
                                    *PC = *PC + 4;
                                   break;
                        case BGE : printf("BGE Instruction\n");
                                   if(GPR[InstFields->rs1] >= GPR[InstFields->rs2])
                                    *PC = *PC + InstFields->imm;
                                   else
                                    *PC = *PC + 4;
                                   break;
                        case BLTU: printf("BLTU Instruction\n"); //unsigned comparison
                                   if((unsigned int)GPR[InstFields->rs1] < (unsigned int)GPR[InstFields->rs2])
                                    *PC = *PC + InstFields->imm;
                                   else
                                    *PC = *PC + 4;
                                   break;
                        case BGEU: printf("BGEU Instruction\n"); //unsigned comparison
                                   if((unsigned int)GPR[InstFields->rs1] >= (unsigned int)GPR[InstFields->rs2])
                                    *PC = *PC + InstFields->imm;
                                   else
                                    *PC = *PC + 4;
                                   break;
                        default  : printf("Error: Cannot find the instruction based on opcode and func3\n");
                    }
                    break;
        default:    printf("\nExecution of Other Instruction\n");
    }
}

/************** Main function ***************/
int main(int argc, char *argv[])
{
    //Declaration of variables
    InstFields_t InstFields; //Instruction Fields structure
    unsigned int default_pc = 0, default_sp = 65535;
    char default_file_name[] = "branch.mem";
    uint32_t pc, sp, ra = 0;
    char *file_name, *sp_arg, *pc_arg;
    uint8_t flag, i; //local variables
    int32_t GPR[32]; //General Purpose Register

    //Initialization of variables: file, pc, and sp
    if(argc == 1)
    {
        file_name = default_file_name;
        pc = default_pc;
        GPR[2] = default_sp;
    }
    else
    {
        file_name = argv[3];
        sp_arg = argv[2];
        pc_arg = argv[1];
        pc = atoi(pc_arg);
        GPR[2] = atoi(sp_arg);
    }
    printf("\nProgram counter: %u",pc);
    printf("\nStack pointer: %u",sp);
    printf("\nFile name: %s\n",file_name);

    flag = 1;
    i = 1;

    //Function call to load program into memory
    memory_load(file_name);

    //Loop for fetch, decode and execute
    while(flag){
        current_instruction = instruction_fetch(pc);//function call to fetch instruction
        printf("\nCompleted fetching\n");
        Decode(current_instruction, &InstFields); //function call to decode instruction
        printf("\nCompleted decoding\n");
        ExecuteInstruction(GPR, &InstFields, memory, &pc);//function call to execute instruction
        printf("\nCompleted execution\n");
        //pc = pc + 4;
        printf("PC: %x", pc);
        if(i == 1) flag = 0; //condition to exit loop
        i++;
        printf("\nConditions --> i: %d, flag: %d\n", i, flag);
    }

    //Function call to print all the register values
    printf("\nRegister values");
    PrintReg(GPR, &pc);

    printf("******************* END *****************\n");
    return 0;
}

/********************** End of Program ***********************/
