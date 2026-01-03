CHIP JunctionController {
    IN PowerOn;
    OUT X[3], Z[3];
    
    PARTS:
    // ===== PART 1: STATE DETECTION LOGIC =====
    // Detect if we're in state 0 (000) - all bits are 0
    Or(a=state0, b=state1, out=or01);
    Or(a=state2, b=or01, out=or012);
    Not(in=or012, out=isstate0);
    
    // Detect if we're in state 1 (001)
    Or(a=state1, b=state2, out=or12);
    And(a=state0, b=or12, out=notstate1);
    Not(in=notstate1, out=isstate1);
    
    // Detect if we're in state 2 (010)
    And(a=state0, b=state1, out=and01);
    Not(in=and01, out=notstate2);
    And(a=state2, b=notstate2, out=isstate2temp);
    Not(in=isstate2temp, out=isstate2);
    
    // ===== PART 2: STATE INCREMENT LOGIC (3-BIT COUNTER) =====
    // Bit 0: Always flips on each increment
    Not(in=state0, out=next0);
    
    // Bit 1: Flips when bit 0 was 1 (XOR gives us the toggle pattern)
    Xor(a=state0, b=state1, out=next1);
    
    // Bit 2: Flips when both bit 0 and bit 1 were 1
    And(a=state0, b=state1, out=both01);
    Xor(a=both01, b=state2, out=next2);
    
    // ===== PART 3: LOAD LOGIC (UPDATE WHEN PowerOn IS HIGH) =====
    // Select between current state (stay) or next state (increment)
    Mux(a=state0, b=next0, sel=PowerOn, out=in0);  // If PowerOn=0: keep state0, if PowerOn=1: load next0
    Mux(a=state1, b=next1, sel=PowerOn, out=in1);  // If PowerOn=0: keep state1, if PowerOn=1: load next1
    Mux(a=state2, b=next2, sel=PowerOn, out=in2);  // If PowerOn=0: keep state2, if PowerOn=1: load next2
    
    // Create load signal for Bit registers (just passes through PowerOn)
    Or(a=PowerOn, b=false, out=load);
    
    // ===== PART 4: STATE STORAGE (3 BIT REGISTERS) =====
    // Store the current 3-bit state across clock cycles
    Bit(in=in0, load=load, out=state0);  // Stores bit 0 of state
    Bit(in=in1, load=load, out=state1);  // Stores bit 1 of state
    Bit(in=in2, load=load, out=state2);  // Stores bit 2 of state
    
    // ===== PART 5: STATE DECODER (WHICH STATE ARE WE IN?) =====
    // Create inverted versions of state bits for easier decoding
    Not(in=state0, out=notstate0);
    Not(in=state1, out=notstate1b);
    Not(in=state2, out=notstate2b);
    
    // Decode State 0 (000): X=GREEN, Z=RED
    And(a=notstate0, b=notstate1b, out=s0check);
    And(a=s0check, b=notstate2b, out=isS0);
    
    // Decode State 1 (100): X=AMBER, Z=RED
    And(a=state0, b=notstate1b, out=s1check);
    And(a=s1check, b=notstate2b, out=isS1);
    
    // Decode State 2 (010): X=RED, Z=RED (safety gap)
    And(a=notstate0, b=state1, out=s2check);
    And(a=s2check, b=notstate2b, out=isS2);
    
    // Decode State 3 (110): X=RED, Z=GREEN
    And(a=state0, b=state1, out=s3check);
    And(a=s3check, b=notstate2b, out=isS3);
    
    // Decode State 4 (001): X=RED, Z=AMBER
    And(a=notstate0, b=notstate1b, out=s4check);
    And(a=s4check, b=state2, out=isS4);
    
    // Decode State 5 (101): X=RED, Z=RED (safety gap)
    And(a=state0, b=notstate1b, out=s5check);
    And(a=s5check, b=state2, out=isS5);
    
    // ===== PART 6: OUTPUT LOGIC (MAP STATES TO LIGHT COLORS) =====
    // Traffic Light X outputs:
    // X[2] = RED light: ON in states 1, 2, 3 (when X must stop)
    Or(a=isS1, b=isS2, out=X2temp);
    Or(a=X2temp, b=isS3, out=X[2]);
    
    // X[1] = AMBER light: ON in states 0 and 5 (transition warning)
    Or(a=isS0, b=isS5, out=X[1]);
    
    // X[0] = GREEN light: ON in states 0 and 1 (X can go)
    Or(a=isS0, b=isS1, out=X[0]);
    
    // Traffic Light Z outputs:
    // Z[2] = RED light: ON in states 0, 1, 2 (when Z must stop)
    Or(a=isS0, b=isS1, out=Z2temp);
    Or(a=Z2temp, b=isS2, out=Z[2]);
    
    // Z[1] = AMBER light: ON in states 3 and 4 (transition warning)
    Or(a=isS3, b=isS4, out=Z[1]);
    
    // Z[0] = GREEN light: ON in states 3 and 5 (Z can go)
    Or(a=isS3, b=isS5, out=Z[0]);
}