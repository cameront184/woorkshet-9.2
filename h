CHIP JunctionController {
    IN PowerOn;
    OUT X[3], Z[3];
    
    PARTS:
    // Check if current state is 5 (101) - need to wrap to 0
    And(a=state0, b=notstate1, out=check5a);
    And(a=check5a, b=state2, out=isState5);
    
    // If state is 5 and PowerOn, load 0; otherwise increment normally
    Not(in=state0, out=notstate0);
    Not(in=state1, out=notstate1);
    Not(in=state2, out=notstate2);
    
    // Normal increment logic
    Not(in=state0, out=inc0);
    Xor(a=state0, b=state1, out=inc1);
    And(a=state0, b=state1, out=carry);
    Xor(a=carry, b=state2, out=inc2);
    
    // If state5, reset to 0; otherwise use incremented value
    Mux(a=inc0, b=false, sel=isState5, out=nextstate0);
    Mux(a=inc1, b=false, sel=isState5, out=nextstate1);
    Mux(a=inc2, b=false, sel=isState5, out=nextstate2);
    
    // Load logic - update when PowerOn is high
    Mux(a=state0, b=nextstate0, sel=PowerOn, out=toload0);
    Mux(a=state1, b=nextstate1, sel=PowerOn, out=toload1);
    Mux(a=state2, b=nextstate2, sel=PowerOn, out=toload2);
    
    // State registers
    Bit(in=toload0, load=PowerOn, out=state0);
    Bit(in=toload1, load=PowerOn, out=state1);
    Bit(in=toload2, load=PowerOn, out=state2);
    
    // Decode states
    // State 0: 000
    Not(in=state0, out=n0);
    Not(in=state1, out=n1);
    Not(in=state2, out=n2);
    And(a=n0, b=n1, out=s0temp);
    And(a=s0temp, b=n2, out=isS0);
    
    // State 1: 001
    And(a=state0, b=n1, out=s1temp);
    And(a=s1temp, b=n2, out=isS1);
    
    // State 2: 010
    And(a=n0, b=state1, out=s2temp);
    And(a=s2temp, b=n2, out=isS2);
    
    // State 3: 011
    And(a=state0, b=state1, out=s3temp);
    And(a=s3temp, b=n2, out=isS3);
    
    // State 4: 100
    And(a=n0, b=n1, out=s4temp);
    And(a=s4temp, b=state2, out=isS4);
    
    // State 5: 101
    And(a=state0, b=n1, out=s5temp);
    And(a=s5temp, b=state2, out=isS5);
    
    // Traffic Light X
    // X[2] (RED): states 2,3,4,5
    Or(a=isS2, b=isS3, out=xred1);
    Or(a=isS4, b=isS5, out=xred2);
    Or(a=xred1, b=xred2, out=X[2]);
    
    // X[1] (AMBER): state 1 only
    Or(a=isS1, b=false, out=X[1]);
    
    // X[0] (GREEN): state 0 only
    Or(a=isS0, b=false, out=X[0]);
    
    // Traffic Light Z
    // Z[2] (RED): states 0,1,2,5
    Or(a=isS0, b=isS1, out=zred1);
    Or(a=isS2, b=isS5, out=zred2);
    Or(a=zred1, b=zred2, out=Z[2]);
    
    // Z[1] (AMBER): state 4 only
    Or(a=isS4, b=false, out=Z[1]);
    
    // Z[0] (GREEN): state 3 only
    Or(a=isS3, b=false, out=Z[0]);
}