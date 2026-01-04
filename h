CHIP JunctionController {
    IN PowerOn;
    OUT X[3], Z[3];
    
    PARTS:
    // Simple 3-bit counter counts 0 to 7
    Not(in=state0, out=notstate0);
    Not(in=state1, out=notstate1);
    Not(in=state2, out=notstate2);
    
    // Increment logic
    Not(in=state0, out=next0);
    Xor(a=state0, b=state1, out=next1);
    And(a=state0, b=state1, out=carry);
    Xor(a=carry, b=state2, out=next2);
    
    // Load next state when PowerOn=1 else keep current state
    Mux(a=state0, b=next0, sel=PowerOn, out=in0);
    Mux(a=state1, b=next1, sel=PowerOn, out=in1);
    Mux(a=state2, b=next2, sel=PowerOn, out=in2);
    
    // State storage
    Bit(in=in0, load=PowerOn, out=state0);
    Bit(in=in1, load=PowerOn, out=state1);
    Bit(in=in2, load=PowerOn, out=state2);
    
    // Decode 8 states 000 to 111
    And(a=notstate0, b=notstate1, out=s0a);
    And(a=s0a, b=notstate2, out=s0);
    
    And(a=state0, b=notstate1, out=s1a);
    And(a=s1a, b=notstate2, out=s1);
    
    And(a=notstate0, b=state1, out=s2a);
    And(a=s2a, b=notstate2, out=s2);
    
    And(a=state0, b=state1, out=s3a);
    And(a=s3a, b=notstate2, out=s3);
    
    And(a=notstate0, b=notstate1, out=s4a);
    And(a=s4a, b=state2, out=s4);
    
    And(a=state0, b=notstate1, out=s5a);
    And(a=s5a, b=state2, out=s5);
    
    And(a=notstate0, b=state1, out=s6a);
    And(a=s6a, b=state2, out=s6);
    
    And(a=state0, b=state1, out=s7a);
    And(a=s7a, b=state2, out=s7);
    
    // X outputs based on state
    // X[2] (RED): states 0,1,4,5,6,7
    Or(a=s0, b=s1, out=xr1);
    Or(a=s4, b=s5, out=xr2);
    Or(a=s6, b=s7, out=xr3);
    Or(a=xr1, b=xr2, out=xr4);
    Or(a=xr4, b=xr3, out=X[2]);
    
    // X[1] (AMBER): states 1,3
    Or(a=s1, b=s3, out=X[1]);
    
    // X[0] (GREEN): state 2 only
    Or(a=s2, b=false, out=X[0]);
    
    // Z outputs based on state
    // Z[2] (RED): states 0,1,2,3,4,5 only (NOT 6,7!)
    Or(a=s0, b=s1, out=zr1);
    Or(a=s2, b=s3, out=zr2);
    Or(a=s4, b=s5, out=zr3);
    Or(a=zr1, b=zr2, out=zr4);
    Or(a=zr4, b=zr3, out=Z[2]);
    
    // Z[1] (AMBER): states 5,7
    Or(a=s5, b=s7, out=Z[1]);
    
    // Z[0] (GREEN): states 6,7
    Or(a=s6, b=s7, out=Z[0]);
}