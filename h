CHIP JunctionController {
    IN PowerOn;
    OUT X[3], Z[3];
    
    PARTS:
    // 3-bit state counter that wraps from 5 back to 0
    Not(in=state0, out=notstate0);
    Not(in=state1, out=notstate1);
    Not(in=state2, out=notstate2);
    
    // Check if we're at state 5 (101)
    And(a=state0, b=notstate1, out=check5);
    And(a=check5, b=state2, out=atState5);
    
    // Increment: bit0 always flips
    Not(in=state0, out=nextbit0);
    
    // Bit1 flips when bit0 is 1
    Xor(a=state0, b=state1, out=nextbit1);
    
    // Bit2 flips when both bit0 and bit1 are 1
    And(a=state0, b=state1, out=both);
    Xor(a=both, b=state2, out=nextbit2);
    
    // If at state 5, next state is 0; otherwise increment
    Mux(a=nextbit0, b=false, sel=atState5, out=next0);
    Mux(a=nextbit1, b=false, sel=atState5, out=next1);
    Mux(a=nextbit2, b=false, sel=atState5, out=next2);
    
    // When PowerOn, load next state; otherwise keep current
    Mux(a=state0, b=next0, sel=PowerOn, out=in0);
    Mux(a=state1, b=next1, sel=PowerOn, out=in1);
    Mux(a=state2, b=next2, sel=PowerOn, out=in2);
    
    // Store state
    Bit(in=in0, load=PowerOn, out=state0);
    Bit(in=in1, load=PowerOn, out=state1);
    Bit(in=in2, load=PowerOn, out=state2);
    
    // Decode each of 6 states
    // State 0: 000
    And(a=notstate0, b=notstate1, out=decode0a);
    And(a=decode0a, b=notstate2, out=state0is);
    
    // State 1: 001
    And(a=state0, b=notstate1, out=decode1a);
    And(a=decode1a, b=notstate2, out=state1is);
    
    // State 2: 010
    And(a=notstate0, b=state1, out=decode2a);
    And(a=decode2a, b=notstate2, out=state2is);
    
    // State 3: 011
    And(a=state0, b=state1, out=decode3a);
    And(a=decode3a, b=notstate2, out=state3is);
    
    // State 4: 100
    And(a=notstate0, b=notstate1, out=decode4a);
    And(a=decode4a, b=state2, out=state4is);
    
    // State 5: 101
    And(a=state0, b=notstate1, out=decode5a);
    And(a=decode5a, b=state2, out=state5is);
    
    // Traffic Light X outputs
    // State 0: GREEN (001), State 1: AMBER (010), States 2-5: RED (100)
    
    // X[2] RED: ON in states 2,3,4,5
    Or(a=state2is, b=state3is, out=Xr1);
    Or(a=state4is, b=state5is, out=Xr2);
    Or(a=Xr1, b=Xr2, out=X[2]);
    
    // X[1] AMBER: ON in state 1 only
    And(a=state1is, b=true, out=X[1]);
    
    // X[0] GREEN: ON in state 0 only
    And(a=state0is, b=true, out=X[0]);
    
    // Traffic Light Z outputs
    // States 0-2,5: RED (100), State 3: GREEN (001), State 4: AMBER (010)
    
    // Z[2] RED: ON in states 0,1,2,5
    Or(a=state0is, b=state1is, out=Zr1);
    Or(a=state2is, b=state5is, out=Zr2);
    Or(a=Zr1, b=Zr2, out=Z[2]);
    
    // Z[1] AMBER: ON in state 4 only
    And(a=state4is, b=true, out=Z[1]);
    
    // Z[0] GREEN: ON in state 3 only
    And(a=state3is, b=true, out=Z[0]);
}