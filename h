CHIP JunctionController {
    IN PowerOn;
    OUT X[3], Z[3];
    
    PARTS:
    // State detection
    Or(a=state0, b=state1, out=or01);
    Or(a=state2, b=or01, out=or012);
    Not(in=or012, out=isstate0);
    
    Or(a=state1, b=state2, out=or12);
    And(a=state0, b=or12, out=notstate1);
    Not(in=notstate1, out=isstate1);
    
    And(a=state0, b=state1, out=and01);
    Not(in=and01, out=notstate2);
    And(a=state2, b=notstate2, out=isstate2temp);
    Not(in=isstate2temp, out=isstate2);
    
    // 3-bit counter increment logic
    Not(in=state0, out=next0);
    Xor(a=state0, b=state1, out=next1);
    And(a=state0, b=state1, out=both01);
    Xor(a=both01, b=state2, out=next2);
    
    // Load control
    Mux(a=state0, b=next0, sel=PowerOn, out=in0);
    Mux(a=state1, b=next1, sel=PowerOn, out=in1);
    Mux(a=state2, b=next2, sel=PowerOn, out=in2);
    Or(a=PowerOn, b=false, out=load);
    
    // State storage
    Bit(in=in0, load=load, out=state0);
    Bit(in=in1, load=load, out=state1);
    Bit(in=in2, load=load, out=state2);
    
    // State decoder
    Not(in=state0, out=notstate0);
    Not(in=state1, out=notstate1b);
    Not(in=state2, out=notstate2b);
    
    And(a=notstate0, b=notstate1b, out=s0check);
    And(a=s0check, b=notstate2b, out=isS0);
    
    And(a=state0, b=notstate1b, out=s1check);
    And(a=s1check, b=notstate2b, out=isS1);
    
    And(a=notstate0, b=state1, out=s2check);
    And(a=s2check, b=notstate2b, out=isS2);
    
    And(a=state0, b=state1, out=s3check);
    And(a=s3check, b=notstate2b, out=isS3);
    
    And(a=notstate0, b=notstate1b, out=s4check);
    And(a=s4check, b=state2, out=isS4);
    
    And(a=state0, b=notstate1b, out=s5check);
    And(a=s5check, b=state2, out=isS5);
    
    // Output mapping
    Or(a=isS2, b=isS3, out=X2temp1);
    Or(a=isS4, b=isS5, out=X2temp2);
    Or(a=X2temp1, b=X2temp2, out=X[2]);
    Or(a=isS1, b=false, out=X[1]);
    Or(a=isS0, b=false, out=X[0]);
    
    Or(a=isS0, b=isS1, out=Z2temp1);
    Or(a=isS2, b=isS5, out=Z2temp2);
    Or(a=Z2temp1, b=Z2temp2, out=Z[2]);
    Or(a=isS4, b=false, out=Z[1]);
    Or(a=isS3, b=false, out=Z[0]);
}