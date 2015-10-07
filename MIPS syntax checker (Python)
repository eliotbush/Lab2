#MIPS syntax checker for add, addi, sub, mul, lw, sw, and beq.
#Asks for instructions as input until a syntactically incorrect instruction is passed.
#MIPS syntax instructions are in comments.
#The logic can be ported to C but Python is much easier to write and debug. This script is very boolean logic heavy
#Not sure what the logic for valid addresses/immediates are
#for testing copy-paste into https://repl.it/languages/python3 and click "run"
#or use a Python3 IDE

#-----------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------

#this function converts an instruction (as a string) into an array where each parameter of the instruction is an entry
#this can probably be done more elegantly but I'm not sure how well the python->C translation will go so I bruteforced it.
#the array declarations won't be easy, python allocates array memory on the fly but C does not
def parseInstruction(s):
    #strip any trailing spaces or commas. Really this should throw a syntax error if it finds any (certainly for commas, not sure about trailing spaces).
    i=len(s)-1
    trip = True
    while i>=0:
        if ((s[i] == ' ') or (s[i] == ',')) and trip:
            s = s[0:i]
        else:
            trip = False
        i-=1
    #strip leading spaces. not sure if they should throw an error or not, I don't remember the MIPS whitespace rules
    while (s[0] == ' '):
        s = s[1:len(s)]
        
    
    #loop parameter
    i=0
    #location of the last delimiter
    last = 0
    #checks for whether or not argument delimiters are valid (otherwise instructions like "add a b c" will pass through)
    instrSep = True
    #initialize the array which will hold the instruction
    instructionArray = []
    #step through the string looking for delimiters and separating into an array
    while i<len(s):
        #if we find a space AND we haven't found the opcode yet
        if (s[i]==' ') and instrSep:
            instrSep = False
            #the append function adds the element in the parentheses to the array.
            #the operation s[m:n] slices the string between the indices [m,n)
            #ie if I declare s = "abcd", s[1:3] is "bc"
            instructionArray.append(s[last:i])
            last = i+1
        #if we find a space but we already have the opcode, we have bad argument delimiters.
        #As written this will just leave the loop and cause a failure later on.
        elif (s[i]==' ') and not instrSep:
            print("Invalid delimiter for instruction arguments. Arguments must be separated by commas.")
            i = len(s)
        #if we find a comma (argument delimiter)
        elif s[i]==',':
            #the comma may be followed by a space, as in "add a, b, c"
            #Right now it breaks (calls invalid delimiter) for instructions like "add a,         b, c." Need instructor clarification on that one.
            if s[i+1]==' ':
                instructionArray.append(s[last:i])
                last = i+2
                i+=1
            #if the comma isn't followed by a space.
            else:
                instructionArray.append(s[last:i])
                last = i+1            
        i+=1
    #this gets the last argument
    instructionArray.append(s[last:i])
    
    return instructionArray

#--------------------------------------------------------------------------------

#Check the instruction for the correct number of arguments.
#Returns a boolean so it can be used in conditionals.
def checkArgCount(instr, instrLength, instrName):
    if (len(instr) != instrLength):
        print("Invalid number of arguments for " + instrName + " instruction (" + str(len(instr)-1) + "). Expected " + str(instrLength-1) + ".")
        return False
    else:
        return True

#--------------------------------------------------------------------------------

#Verify that the register arguments are correctly identifying registers.
#I'm pretty sure we only have access to temp and saved, but argument registers may be valid too. Not a hard fix
#quick synopsis of MIPS registers:
#$t0-$t7 / $8-$15: temp registers
#$s0-$s7 / $16-$23: saved registers
#$t8-$t9 / $24-25: more temps
#returns a bool for conditional use
def verifyRegisters(instr, regArgCount):
    i=1
    while i<(regArgCount+1):
        #first step is to verify the argument length.
        #if length 2
        if (len(instr[i])==2):
            #the only valid arguments are $8 and $9
            #the "ord(c)" function gets the ASCII value of the char c. In C we can use the type recast "(int)(c)"
            if ((instr[i][0]=='$') and ((ord(instr[i][1])==ord('8')) or (ord(instr[i][1])==ord('9')))):
                i+=1
            else:
                print("Invalid register declaration: " + instr[i])
                return False
        #if it's length 3...
        elif (len(instr[i])==3):
            #lots of nested bools, I know this isn't easy to read.
            #first check that the first char in the reg argument is '$': (instr[i][0]=='$') AND...
            #then verify that the third char is a number within the correct range specified by the second char. this one's a mess, there are four bools within it:
            #if the second char was 't', the third should be between 0 and 9: ((instr[i][1]=='t') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('9')))) OR...
            #if the second char was 's', the third should be between 0 and 7: ((instr[i][1]=='s') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('7')))) OR...
            #if the second char was '1', the third should be between 0 and 9: ((instr[i][1]=='1') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('9')))) OR...
            #if the second char was '2', the third should be between 0 and 5: ((instr[i][1]=='2') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('5'))))
            if ((instr[i][0]=='$') and (((instr[i][1]=='t') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('9')))) or ((instr[i][1]=='s') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('7')))) or ((instr[i][1]=='1') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('9')))) or ((instr[i][1]=='2') and ((ord(instr[i][2])>=ord('0')) and (ord(instr[i][2])<=ord('5')))))):
                i+=1
            else:
                print("Invalid register declaration: " + instr[i])
                return False
        else:
            print("Invalid register declaration: " + instr[i])
            return False        
    return True

#--------------------------------------------------------------------------------

#Verify whether the address (for lw, sw, and beq) is valid.
def verifyAddress(instr, regArgCount):
    print("Address: " + instr[1+regArgCount])
    #do something to verify whether the address is valid or not
    return True

#---------------------------------------------------------------------------------

#Verify whether the immediate (for addi) is valid.
def verifyImmediate(instr):
    print("Immediate: " + instr[3])
    #do something to verify whether the immediate is valid or not
    return True

#---------------------------------------------------------------------------------

#verifies whether the instruction is valid or not
def verifyInstruction(instr):
    #convert instruction to array
    a = parseInstruction(instr)
        
    #Check for ADD instruction.
    #Description: Adds two registers and stores the result in a register
    #Operation: $d = $s + $t; advance_pc (4);
    #Syntax: add $d, $s, $t
    if (a[0] == "add"):
        if checkArgCount(a, 4, "ADD"):
            print("Instruction recognized as ADD. Arguments: " + a[1] + ", " + a[2] + ", " + a[3])
            return verifyRegisters(a, 3)
        else:
            return False
    
    #Check for SUB instruction.
    #Description: Subtracts two registers and stores the result in a register
    #Operation: $d = $s - $t; advance_pc (4);
    #Syntax: sub $d, $s, $t
    elif (a[0] == "sub"):
        if checkArgCount(a, 4, "SUB"):
            print("Instruction recognized as SUB. Arguments: " + a[1] + ", " + a[2] + ", " + a[3])
            return verifyRegisters(a, 3)
        else:
            return False
    
    #Check for ADDI instruction.
    #Description: Adds a register and a sign-extended immediate value and stores the result in a register
    #Operation: $t = $s + imm; advance_pc (4);
    #Syntax: addi $t, $s, imm
    elif (a[0] == "addi"):
        if checkArgCount(a, 4, "ADDI"):
            print("Instruction recognized as ADDI. Arguments: " + a[1] + ", " + a[2] + ", " + a[3])
            if verifyRegisters(a, 2):
                return verifyImmediate(a)
            else:
                return False
        else:
            return False
      
    #Check for MUL instruction.
    #Description: pseudoinstruction which makes it look as if MIPS has a 32-bit multiply instruction that places its 32-bit result
    #Operation: $d = $s * $t; advance_pc (4);
    #Syntax: mul $d, $s, $t
    elif (a[0] == "mul"):
        if checkArgCount(a, 4, "SUB"):
            print("Instruction recognized as MUL. Arguments: " + a[1] + ", " + a[2] + ", " + a[3])
            return verifyRegisters(a, 3)
        else:
            return False
    
    #Check for LW instruction.
    #Description: A word is loaded into a register from the specified address.
    #Operation: $t = MEM[$s + offset]; advance_pc (4);
    #Syntax: lw $t, offset($s)
    elif (a[0] == "lw"):
        if checkArgCount(a, 3, "LW"):
            print("Instruction recognized as LW. Arguments: " + a[1] + ", " + a[2])
            return (verifyRegisters(a, 1) and verifyAddress(a, 1))
        else:
            return False
    
    #Check for SW instruction.
    #Description: The contents of $t is stored at the specified address.
    #Operation: MEM[$s + offset] = $t; advance_pc (4);
    #Syntax: sw $t, offset($s)
    elif (a[0] == "sw"):
        if checkArgCount(a, 3, "SW"):
            print("Instruction recognized as SW. Arguments: " + a[1] + ", " + a[2])
            return (verifyRegisters(a, 1) and verifyAddress(a, 1))
        else:
            return False
    
    #Check for BEQ instruction.
    #Description: Branches if the two registers are equal
    #Operation: if $s == $t advance_pc (offset << 2)); else advance_pc (4);
    #Syntax: beq $s, $t, offset
    elif (a[0] == "beq"):
        if checkArgCount(a, 4, "BEQ"):
            print("Instruction recognized as BEQ. Arguments: " + a[1] + ", " + a[2] + ", " + a[3])
            return (verifyRegisters(a, 2) and verifyAddress(a, 2))
        else:
            return False
    
    else:
        print("Invalid instruction name. Only add, sub, addi, mul, lw, sw, and beq are accepted.")
        return False

#---------------------------------------------------------------------------------
#--------------------------------------------------------------------------------

#Function call for testing. Keeps asking for instructions until a syntax error is found.
s = input("Enter an instruction: ")
while verifyInstruction(s):
    print("Instruction parsed succesfully.")
    s = input("Enter an instruction: ")
