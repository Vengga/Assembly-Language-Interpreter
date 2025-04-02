#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <bitset>

class VirtualMachine {
private:
    int R[7];
    unsigned int PC;
    bool OF, UF, CF, ZF;
    std::vector<int> memory;
    std::ofstream outputFile;

public:
    VirtualMachine();
    ~VirtualMachine();
    void displayState() const;
    void executeProgram(const std::string& inputFilePath);

private:
    bool isValidInstruction(const std::string& instruction) const;
    int getOperandValue(const std::string& operand) const;
    void updateFlags(int result);
    void shift(const std::string& instruction, const std::string& operand1, const std::string& operand2);
    unsigned char ror(unsigned char value, int shiftAmount);
    void move(const std::string& operand1, const std::string& operand2);
    void arithmeticOperation(const std::string& instruction, const std::string& operand1, const std::string& operand2);
    void incrementDecrement(const std::string& instruction, const std::string& operand);
    void input(const std::string& operand);
    void output(const std::string& operand);
    void loadStore(const std::string& instruction, const std::string& operand1, const std::string& operand2);
    void rotateLeft(const std::string& operand, int shiftAmount);
    void rotateRight(const std::string& operand, int shiftAmount);
};

#endif // VIRTUALMACHINE_H
