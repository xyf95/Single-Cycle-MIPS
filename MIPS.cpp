#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

bitset<6> getSixBits(bitset<32> Ins, int start, int end) {
	int i, j =5;
	bitset<6> temp;
	for (i = start; i >= end; i--)
	{
		temp[j] = Ins[i];      
		j--;
	}
	return temp;
}

bitset<5> getFiveBits(bitset<32> Ins, int start, int end) {
	int i, j = 4;
	bitset<5> temp;
	for (i = start; i >= end; i--)
	{
		temp[j] = Ins[i];         
		j--;
	}
	return temp;
}

bitset<32> signExtend(bitset<16> Imm){
	int j;
	bitset<32> temp;
	for (j = 0; j < 16; j++){
		temp[j] = Imm[j];
		temp[j+16] = Imm[15];
	}
	return temp;
}

	
class RF
{
    public:
        bitset<32> ReadData1, ReadData2;   //32 bit 
     	RF()
    	{ 
          Registers.resize(32);                      //in this program, every register's data is 0 at beginning  Registers[i]=0
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
            // implement the funciton by you.  
		ReadData1 = Registers[RdReg1.to_ulong()];             //read data
		ReadData2 = Registers[RdReg2.to_ulong()];             //let RdReg1,2 from binary bits to an integer.
		if (WrtEnable.to_ulong()==1) 
		Registers[WrtReg.to_ulong()] = WrtData;              //if writeenable, write data to register
         }
		 
	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);      //appand at the end
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;                     //all 32 register result output into RFresult
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, std::bitset<32> oprand1, std::bitset<32> oprand2)
             {   
                 // implement the ALU operations by you. 
                if(ALUOP.to_ulong() == ADDU)        //addition
                ALUresult = oprand1.to_ulong() + oprand2.to_ulong();

                else if(ALUOP.to_ulong() == SUBU)     //subtraction
                ALUresult = oprand1.to_ulong() - oprand2.to_ulong();

                else if(ALUOP.to_ulong() == AND)    //and
                ALUresult = oprand1 & oprand2;           

                else if (ALUOP.to_ulong() == OR)//or
                ALUresult = oprand1|oprand2;           

                else if (ALUOP.to_ulong() == NOR)    //nor
                ALUresult = ~(oprand1|oprand2);             
                 return ALUresult;
               }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                        IMem[i] = bitset<8>(line);         //store all line in imem
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
		Instruction = 0;
		for (int i = 0; i < 4; i++) {
			Instruction <<= 8;                    //a instruction have 32 bit
			Instruction =Instruction.to_ulong() + IMem[ReadAddress.to_ulong() + i].to_ulong();        //read 32 bit insturction in imen.txt 
		}
				return Instruction;  
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem()
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line);      //get all line in deme
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
	{  
		// implement by you.
		int x=31;
		if (readmem.to_ulong()==1) {
			readdata = 0;
			for (int i = 0; i < 4; ++i) {
				readdata <<= 8;
				readdata = readdata.to_ulong() + DMem[Address.to_ulong() + i].to_ulong();     //get data in M[R[rs]+SignExtImm]
			}
		}
		if (writemem.to_ulong()==1) {
			for (int i = 0; i < 4; i++) 
                        	for (int j = 7; j >= 0; j--)
                        	{
							DMem[Address.to_ulong() + i][j] = WriteData[x];
							x--;
							} 


		}
         
		return readdata;
	}
                     
          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt",std::ios_base::app);
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;              //output the lastest result into demeresult.txt
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  


   
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

	//int pc = 0, nextAddr = 4;
	bool isEqual = false;
	int i;
	bitset<16> imm;
	bitset<32> pc=0;//32 bit register
	bitset<32> tempPC;
	bitset<32> curInstruction;
	bitset<32> jAddr;
	bitset<32> BranchAddr;
    bitset<32> SignExtImm;	
	bitset<32> result;
	bitset<32> offset;
	bitset<32> readData;
	bitset<6> opcode /*bits 31-26*/, funct /*bits 5-0*/;
	bitset<5> rs /*bits 25-21*/, rt /*bits 20-16*/, rd /*bits 15-11*/, shtamt /*bits 10-6*/;
	bitset<3> ALUop;
    while (1)
	{
        // Fetch
        	curInstruction = myInsMem.ReadMemory(pc.to_ulong());          //find the instruction in 4i
		cout << "CurIns: " << curInstruction << endl;
		// If current insturciton is "11111111111111111111111111111111", then break;
        	if (curInstruction == 0xFFFFFFFF)
        	{
			cout<<"halt instruction"<<endl;
			cout<<"PC: "<<pc.to_ulong()<<endl;
        	break;
			}


		opcode = getSixBits(curInstruction, 31, 26);
		cout << "opcode: " << opcode << endl;

	// decode(Read RF)
		if (opcode == 0x00) {
			rs = getFiveBits(curInstruction, 25, 21);
			rt = getFiveBits(curInstruction, 20, 16);          //seperate rs, rt and rd
			rd = getFiveBits(curInstruction, 15, 11);
			funct = getSixBits(curInstruction, 5, 0);         //get function
			cout<<"function code: "<<funct<<endl;
			for (i = 0; i < 3; i++) {
				ALUop[i] = funct[i];               //the last 3 digit of function can divide 5 operation
			}
            if(ALUop==1)
            cout<<"addu instruction"<<endl;
            else if(ALUop==3)
            cout<<"subu instruction"<<endl;             //output certain function 
            else if(ALUop==4)
            cout<<"and instruction"<<endl;
            else if(ALUop==5)
            cout<<"or instruction"<<endl;
            else
            cout<<"nor instruction"<<endl;
            

			myRF.ReadWrite(rs, rt, rd, 0, 0);         //only read no write
			cout << "R[rs]=R["<<rs.to_ulong()<<"]= " << myRF.ReadData1 << endl; //rs
			cout << "R[rt]=R["<<rt.to_ulong()<<"]= " << myRF.ReadData2 << endl; //rt


		} else if (opcode == 0x02) { //J-Type instructions   jump
		    cout<<"jump instruction  ";
			tempPC = pc.to_ulong()+4;
			for (i = 2; i < 28; i++) { //getting the 26 bit address from the instruction
				jAddr[i] = curInstruction[i-2];
			}
			for (i = 28; i < 32; i++) { //getting the first four bits from PC
				jAddr[i] = tempPC[i];
			}
			for (i = 0; i < 2; i++) { //shift left 2 position
				jAddr[i] = 0;
			}
         cout<<"PC jump to: "<<jAddr.to_ulong()<<endl;
			
		} else {//I-Type Instrcutions
			rs = getFiveBits(curInstruction, 25, 21);
			rt = getFiveBits(curInstruction, 20, 16);
			for (i = 0; i < 16; i++) {
				imm[i] = curInstruction[i];           //immediate
			}
			offset = signExtend(imm);                    //SignExtImm


			myRF.ReadWrite(rs, rt, rt, 0, 0);           //only read no write
		}



		// Execute
		if (opcode == 0x00) {
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
			cout << "result=     " << result << endl ;
		} 
		else if(opcode!=0x02){
			if (opcode == 0x04)           //beq
			{
			cout<<"beq instruction"<<endl;	
			ALUop = 3;
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
			if (result == 0)           //beq judgement
			{
			isEqual = true;
			cout<<"R[rs]=R[rt]"<<endl;	
			}
			else
			cout<<"R[rs]!=R[rt]"<<endl;				
			}
			else
			{
			ALUop = 1;            //lw,sw,addiu
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, offset);
			}



		}

		// Read/Write Mem
		if (opcode == 0x23)           //lw   update register data
		{cout<<"lw instruction"<<endl;
		readData = myDataMem.MemoryAccess(result, myRF.ReadData2, 1, 0);
		cout<<"M[R[rs]+SignExtImm]=";	
		cout<<"M[R["<<rs.to_ulong()<<"]+SignExtImm]="<<endl;
		cout<<"M["<<myRF.ReadData1;
		cout<<" + " <<offset<<"]"<<endl;
		cout<<"= M["<<result.to_ulong()<<"]"<<endl;				
		cout<<"= "<<readData<<endl;		
		cout<<"Load in R["<<rt.to_ulong()<<"] "<<endl;	
		}

		else if (opcode == 0x2b)       //sw, update data stored in memory
		{
	    cout<<"sw instruction"<<endl;
		readData=myDataMem.MemoryAccess(result, myRF.ReadData2, 0, 1);
		cout<<"R["<<rt.to_ulong()<<"]="<<myRF.ReadData2<<endl;				
		cout<<"M[R[rs]+SignExtImm]=";	
		cout<<"M[R["<<rs.to_ulong()<<"]+SignExtImm]="<<endl;
		cout<<"M["<<myRF.ReadData1;
		cout<<" + " <<offset<<"]"<<endl;
		cout<<"= M["<<result.to_ulong()<<"]"<<endl;	
		cout<<"change M["<<result.to_ulong()<<"]'s vale to R["<<rt.to_ulong()<<"]"<<endl;		
		}
		else if (opcode == 0x09)       //addiu, update data stored in memory
		{
	    cout<<"addiu instruction"<<endl;
	    cout<<"R[rs] = ";
	    cout<<"R["<<rs.to_ulong()<<"]=     "<<myRF.ReadData1<<endl;
		cout<<"SignExtImm =      "<<offset<<endl;			
		cout<<"R["<<rs.to_ulong()<<"] + SignExtImm="<<result<<"="<<result.to_ulong()<<endl;	
		cout<<"Store in R["<<rt.to_ulong()<<"]"<<endl;
		}


		// Write back to RF
		if (opcode != 0x04 && opcode != 0x2b && opcode != 0x02) {
			if (opcode == 0x00)     
			{
			myRF.ReadWrite(rs, rt, rd, result, 1);            //R type write to R[rd]      the rd means WrtReg and result is WrtData
			cout<<"write to R[rd]=R["<<rd.to_ulong()<<"]"<<endl;
			}                  

			else if(opcode==0x23)
			myRF.ReadWrite(rs, rt, rt, readData, 1);          //lw addiu write to R[rt]    the second rt means WrtReg and readData is WrtData
            else
            myRF.ReadWrite(rs, rt, rt, result, 1);          //lw addiu write to R[rt]    the second rt means WrtReg and readData is WrtData
		}

        if(!isEqual)
		cout << "PC: " << pc.to_ulong() << endl<<endl;
	
		if (opcode == 0x02)
		pc = jAddr;	                   //jump


		else if (opcode == 0x04 && isEqual)          //beq true
		{
		for (i = 2; i < 18; i++) { //immediate 0-15
		BranchAddr[i] = imm[i-2];
			}
			for (i = 18; i < 32; i++) { //14 immediate[15]
				BranchAddr[i] = imm[15];
			}
			for (i = 0; i < 2; i++) { //shift left 2 position
				BranchAddr[i] = 0;
			}
			
	    bitset<32> pctemp;
		pctemp=pc;	
		pc = pc.to_ulong() + 4 + BranchAddr.to_ulong()*4 ;            //beq jump 
		cout<<"PC jump to: "<<pc.to_ulong()<<endl;
		cout << "PC: " << pctemp.to_ulong() <<endl<<endl;	
		isEqual=false;                  //reset the beq statues
		}

		else
			pc = pc.to_ulong() + 4;        //common instruction, pc=pc+4;

			
        myRF.OutputRF(); 
    }
        myDataMem.OutputDataMem(); // output data mem,   if sw run, than the result will different from before

        system("pause");
        return 0;
        
}
