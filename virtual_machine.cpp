#include <iostream>  // Input/output stream handling
#include <fstream>   // File stream handling
#include <sstream>   // String stream handling
#include <vector>    // Dynamic array (vector) handling
#include <iomanip>   // Input/output manipulators (e.g., setw, setfill)
#include <bitset>    // Bit manipulation

using namespace std;
// Class representing a simple virtual machine
class VirtualMachine {
private:
    int R[7];             // Array for registers
    unsigned int PC;      // Program counter
    bool OF, UF, CF, ZF;   // Flags
    vector<int> memory;    // Memory for the virtual machine
    ofstream outputFile;   // Output file stream for logging

public:
    // Constructor initializing the virtual machine
    VirtualMachine() : R{}, PC(0), OF(false), UF(false), CF(false), ZF(false), memory(64, 0) {
        // Open output file for logging
        outputFile.open("output.txt");
    }
    int stringToInt(const string& str) // Convert string to integer
    {
        stringstream ss(str); //Convert a stringstream object with the input string
        int result; //Declare an integer to hold the result
        ss >> result; //Extract the integer from stringstream
        return result; //Return the integer
    }

    void displayState() const // Display the current state of the virtual machine
    {
        cout << "Registers: "; // Display registers
        for (int i = 0; i < 7; ++i) //Loop through each register
        {   //Display the value of the register in decimal format, padded with zeros
            cout << setw(2) << setfill('0') << dec << R[i] << " ";
        }
        //Display the flags and program counter
        cout <<"#\nFlags: " << OF << " " << UF << " " << CF << " " << ZF << "#\nPC: " << dec << PC << "\nMemory:\n";
        // Display memory
        for (int i = 0; i < memory.size(); ++i) //Loop through each memory location
        {
            cout << setw(2) << setfill('0') << dec << memory[i] << " ";
            if ((i + 1) % 8 == 0) //If the memory location is a multiple of 8
            {
                cout << endl; //Start a line
            }
        }
        cout << "#\n"; // Separator
    }

    void executeProgram(const string& inputFilePath) // Execute the program loaded from the input file
    {
        ifstream inputFile(inputFilePath);
        if (inputFile)
            {
            cout << "File opened successfully.\n";
            string line;

            while (getline(inputFile, line))
            {
                istringstream ins(line);
                string instruction;
                ins >> instruction;

                if (isValidInstruction(instruction))
                {
                    string operand1, operand2;
                    ins >> operand1 >> operand2;

                    if (isdigit(operand2[0]) && instruction != "STORE" && instruction != "LOAD")
                        {
                        operand2 = "#" + operand2;
                        }

                    if (instruction == "MOV") // Process the instructions
                        {
                        move(operand1, operand2);
                        }
                    else if (instruction == "ADD" || instruction == "SUB" || instruction == "MUL" || instruction == "DIV")
                        {
                        arithmeticOperation(instruction, operand1, operand2);
                        }
                    else if (instruction == "INC" || instruction == "DEC")
                        {
                        incrementDecrement(instruction, operand1);
                        }
                    else if (instruction == "IN")
                        {
                        input(operand1);
                        }
                    else if (instruction == "OUT")
                        {
                        output(operand1);
                        }
                    else if (instruction == "LOAD" || instruction == "STORE")
                        {
                        loadandStore(instruction, operand1, operand2);
                        }
                    else if (instruction == "SHL" || instruction == "SHR" || instruction == "ROL" || instruction == "ROR")
                        {
                        shift(instruction, operand1, operand2);
                        } else
                        {
                        cout << "Invalid instruction: " << instruction << endl;
                        }
                } else
                {
                    cout << "Invalid instruction: " << instruction << endl;
                }
                PC++; //Program counter will be incremented for each instruction
                displayState();
            }

            cout << "File closed successfully.\n";
        } else {
            cout << "Error opening file.\n";
              }
        inputFile.close();
    }

private:
    bool isValidInstruction(const string& instruction) const // Check if the instruction is valid
    {
        return (instruction == "MOV" || instruction == "ADD" || instruction == "SUB" || instruction == "MUL" ||
                instruction == "DIV" || instruction == "INC" || instruction == "DEC" || instruction == "IN" ||
                instruction == "OUT" || instruction == "LOAD" || instruction == "STORE" || instruction == "SHL" ||
                instruction == "SHR" || instruction == "ROL" || instruction == "ROR");
    }

    int getOperandValue(const string& operand) const // Get the value of an operand based on its type
    {
        // If the operand is a register, return the value in the register
        if (operand[0] == 'R') return R[operand[1] - '0'];
        if (operand[0] == '#') // If the operand is an immediate value
            {
            int value = 0;
            // For each character in the operand, starting from the second character
            for (size_t i = 1; i < operand.size(); ++i)
            {
                // Convert the character to a digit and add it to the value
                value = value * 10 + (operand[i] - '0');
            }
            return value; // Return the value
           }
          // If the operand is a memory location
        if (operand[0] == '[' && operand.back() == ']')
        {
            int memoryIndex = 0; // Initialize memory index
            // For each character in the operand, excluding the first and last characters
            for (size_t i = 1; i < operand.size() - 1; ++i)
            {
                // Convert the character to a digit and add it to the memory index
                memoryIndex = memoryIndex * 10 + (operand[i] - '0');
            }
            return memory[memoryIndex]; // Return the value at the memory location
        }
        return 0; // If the operand is not recognized, return 0
    }

    void updateFlags(int result)// Update flags based on the result of an operation
    {
        OF = (result > 0 && result < 255); // Overflow flag is result is in between 0 and 255
        UF = (result < 0); // Underflow flag if result < 0
        CF = (result > 255); // Carry flag if result > 255
        ZF = (result == 0); // Zero flag if result is 0
    }

    // Perform shift or rotate operation
   void shift(const string& instruction, const string& operand1, const string& operand2)
   {
    int shiftAmount = getOperandValue(operand2); //Get the shift amount from the second operand
    unsigned char value = getOperandValue(operand1); //Get the value to be shifted from the 1st operand
    unsigned char result = 0; //Initialize result
    // Display binary representation of operand before operation
    cout << "Binary of operand before shift/rotate: " << bitset<8>(value) << endl;
    if (instruction == "ROL")// If instruction is Shift Left
        {// Shift the bits of value to the left by shiftAmount and mask with 0xFF to keep within 8 bits
        result = (value << shiftAmount);
        }
    else if (instruction == "SHR")
        { // If instruction is Shift Right
        result = value >> shiftAmount;// Shift the bits of value to the right by shiftAmount
        }
    else if (instruction == "SHL")// If instruction is Rotate Left
        {
        result = (value << shiftAmount) | (value >> (8 - shiftAmount));
        // Rotate the bits of value to the left by shiftAmount
        result |= (value << (shiftAmount - 1)); // Set the carry bit in the rotated value
        }
    else if (instruction == "ROR")
        {// Rotate the bits of value to the right by shiftAmount and mask with 0xFF to keep within 8 bits
        result = ((value >> shiftAmount) | (value << (8 - shiftAmount))) & 0xFF;
        }
    else // If instruction is not recognized
        {
        cout << "Invalid shift/rotate instruction: " << instruction << endl; // Display error message
        return; // Exit function
        }
    cout << "Binary of result after shift/rotate: " << bitset<8>(result) << endl;
    // Display binary representation of result after operation
    cout << "Decimal of result after shift/rotate: " << static_cast<int>(result) << endl;  // Display the decimal value
    // Display decimal value of result after operation
    updateFlags(result); // Update flags based on the result of the operation
    if (operand1[0] == 'R' && isdigit(operand1[1]))// If the first operand is a register
        {
        R[operand1[1] - '0'] = result; // Store the result in the specified register
        }
   }
       // Rotate right operation
      unsigned char ror(unsigned char value, int shiftAmount)
      {
      return (value >> shiftAmount) | (value << ((sizeof(value) * 8) - shiftAmount));
      }
  // Move data between registers, memory, or immediate values
  void move(const string& operand1, const string& operand2)
  {
    if (operand1[0] == 'R' && operand2[0] == 'R')
        {
        int sourceValue = R[operand1[1] - '0'];// Get the value from the source register
        R[operand2[1] - '0'] = sourceValue;// Copy the value to the destination register
        cout << "Copied value " << sourceValue << " from register R" << (operand1[1] - '0')
        << " to register R" << (operand2[1] - '0') << endl;
        }// Display the operation

    else if (isdigit(operand1[0]) && operand2[0] == 'R') // If the first operand is an immediate value and the second operand is a register
        {
        int sourceValue = stringToInt(operand1);// Convert the string to an integer
        R[operand2[1] - '0'] = sourceValue;// Store the value in the specified register
        cout << "Stored value " << sourceValue << " to register R" << (operand2[1] - '0') << endl;
        } // Display the operation

    // If the first operand is a memory location and the second operand is a register
    else if (operand1[0] == '[' && operand1.back() == ']' && operand2[0] == 'R')
        {
        int memoryAddress = R[operand1[2] - '0'];// Get the memory address from the register specified in the first operand
        int sourceValue = R[operand2[1] - '0'];// Get the value from the register specified in the second operand
        memory[memoryAddress] = sourceValue;// Store the value in the specified memory location
        cout << "Copied value " << sourceValue << " from register R" << (operand2[1] - '0')
        << " to memory location " << &memory[memoryAddress] << endl;
        }
    else
        {// If the operands are not recognized, display error message
        cout << "Invalid operands: " << operand1 << ", " << operand2 << endl;
        }
 }
   // Perform arithmetic operations (ADD, SUB, MUL, DIV)
  // Perform arithmetic operations (ADD, SUB, MUL, DIV)
void arithmeticOperation(const string& instruction, const string& operand1, const string& operand2)
{
    int result;

    if (instruction == "ADD")
    {
        result = getOperandValue(operand1) + getOperandValue(operand2);
    }
    else if (instruction == "SUB")
    {
        result = getOperandValue(operand1) - getOperandValue(operand2);
    }
    else if (instruction == "MUL")
    {
        result = getOperandValue(operand1) * getOperandValue(operand2);
    }
    else if (instruction == "DIV")
    {
        int divisor = getOperandValue(operand2);
        if (divisor != 0)
        {
            result = getOperandValue(operand1) / divisor;
        }
        else
        {
            cout << "Error: Division by zero.\n";
            return;
        }
    }

    updateFlags(result); // Update flags based on the result of the operation

    // If operand1 is a register, store the result in the register
    if (operand1[0] == 'R')
    {
        R[operand1[1] - '0'] = result;
    }

    cout << "Result after " << instruction << ": " << result << endl;
}

  void incrementDecrement(const string& instruction, const string& operand)
  {
    int result;
    if (operand[0] == 'R')
        {
        if (instruction == "INC")
        {
            result = R[operand[1] - '0'] + 1;
        }
        else if (instruction == "DEC")
        {
            result = R[operand[1] - '0'] - 1;
        }
        else
        {
        cout << "Invalid increment/decrement instruction: " << instruction << endl;
        return;
        }
        updateFlags(result);
        R[operand[1] - '0'] = result;
        cout << "Register R" << (operand[1] - '0') << " incremented/decremented to " << result << endl;
        }
    else {
        cout << "Invalid operand for increment/decrement instruction: " << operand << endl;
         }
 }

  void input(const string& operand)
{
    int value;
    cout << "Enter a value: ";
    cin >> value;

    if (operand[0] == 'R')
    {
        R[operand[1] - '0'] = value;
        cout << "Entered value " << value << " stored in register R" << (operand[1] - '0') << endl;
    }
    else
    {
        cout << "Invalid operand for IN instruction: " << operand << endl;
    }

    updateFlags(value); // Update flags based on the input value
}

void output(const string& operand)
{
    if (operand[0] == 'R')
    {
        int value = R[operand[1] - '0'];
        cout << "Value in register R" << (operand[1] - '0') << ": " << value << endl;
        updateFlags(value); // Update flags based on the output value
    }
    else
    {
        cout << "Invalid operand for OUT instruction: " << operand << endl;
    }
}
    // Load or store data between registers and memory
   void loadandStore(const string& instruction, const string& operand1, const string& operand2) {
    if (instruction == "LOAD") {
    // If the first operand is a register and the second operand is a digit
        if (operand1[0] == 'R' && isdigit(operand2[0])) {
            int memoryIndex = 0;// Initialize memory index
            for (char c : operand2) { // For each character in the second operand
                memoryIndex = memoryIndex * 10 + (c - '0'); // Convert the character to a digit and add it to the memory index
            }
            R[operand1[1] - '0'] = memory[memoryIndex];// Load the value from the memory location into the register
            cout << "Loaded value " << R[operand1[1] - '0'] << " from memory location " << memoryIndex << " into register R" << (operand1[1] - '0') << endl;
        } else {
            cout << "Invalid operands for LOAD instruction: " << operand1 << ", " << operand2 << endl;
        }
    } else if (instruction == "STORE") {
        if (operand1[0] == 'R' && isdigit(operand2[0])) {
            int memoryIndex = 0;
            for (char c : operand2) {
                memoryIndex = memoryIndex * 10 + (c - '0');
            }
            memory[memoryIndex] = R[operand1[1] - '0'];
            cout << "Stored value " << R[operand1[1] - '0'] << " from register R" << (operand1[1] - '0') << " into memory location " << memoryIndex << endl;
        } else {
            cout << "Invalid operands for STORE instruction: " << operand1 << ", " << operand2 << endl;
            }
    }
   }
};

// Main function
int main()
{ // Create an instance of the VirtualMachine class and execute the program
VirtualMachine vm;
vm.executeProgram("assembly_lang.asm");
return 0;
}
