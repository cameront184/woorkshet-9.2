CHIP Crossing {
    IN PowerOn, Button;
    OUT ButtonPressed, Wait, X[3], Z[3], C[7];
    
    PARTS:
    // Button latch stays high once pressed until crossing completes
    Or(a=Button, b=buttonLatch, out=buttonOrLatch);
    And(a=buttonOrLatch, b=notCountingDone, out=newButtonLatch);
    Or(a=PowerOn, b=false, out=load0);
    Bit(in=newButtonLatch, load=load0, out=buttonLatch);
    Or(a=buttonLatch, b=false, out=ButtonPressed);
    
    // Junction state counter (0 to 7)
    Not(in=jstate0, out=notjstate0);
    Not(in=jstate1, out=notjstate1);
    Not(in=jstate2, out=notjstate2);
    
    // Check if at state 4 (100),  both lights RED, ready to stop for crossing
    And(a=notjstate0, b=notjstate1, out=checkState4a);
    And(a=checkState4a, b=jstate2, out=atState4);
    
    // Start crossing when button pressed and at state 4
    And(a=buttonLatch, b=atState4, out=startCrossing);
    
    // Crossing state, once started, stays active until countdown finishes
    Or(a=startCrossing, b=crossingActive, out=newCrossingActive);
    And(a=newCrossingActive, b=notCountingDone, out=nextCrossingActive);
    Or(a=PowerOn, b=false, out=load2);
    Bit(in=nextCrossingActive, load=load2, out=crossingActive);
    
    // 1 when lights are cycling normally, 0 during crossing
    Not(in=crossingActive, out=Wait);
    
    // Junction counter increment logic
    Not(in=jstate0, out=jnext0);
    Xor(a=jstate0, b=jstate1, out=jnext1);
    And(a=jstate0, b=jstate1, out=jcarry);
    Xor(a=jcarry, b=jstate2, out=jnext2);
    
    // Only increment junction counter when not in crossing mode
    And(a=PowerOn, b=Wait, out=junctionIncrement);
    
    Mux(a=jstate0, b=jnext0, sel=junctionIncrement, out=jin0);
    Mux(a=jstate1, b=jnext1, sel=junctionIncrement, out=jin1);
    Mux(a=jstate2, b=jnext2, sel=junctionIncrement, out=jin2);
    
    Or(a=PowerOn, b=false, out=load1);
    Bit(in=jin0, load=load1, out=jstate0);
    Bit(in=jin1, load=load1, out=jstate1);
    Bit(in=jin2, load=load1, out=jstate2);
    
    // Decode junction states
    And(a=notjstate0, b=notjstate1, out=js0a);
    And(a=js0a, b=notjstate2, out=js0);
    
    And(a=jstate0, b=notjstate1, out=js1a);
    And(a=js1a, b=notjstate2, out=js1);
    
    And(a=notjstate0, b=jstate1, out=js2a);
    And(a=js2a, b=notjstate2, out=js2);
    
    And(a=jstate0, b=jstate1, out=js3a);
    And(a=js3a, b=notjstate2, out=js3);
    
    And(a=notjstate0, b=notjstate1, out=js4a);
    And(a=js4a, b=jstate2, out=js4);
    
    And(a=jstate0, b=notjstate1, out=js5a);
    And(a=js5a, b=jstate2, out=js5);
    
    And(a=notjstate0, b=jstate1, out=js6a);
    And(a=js6a, b=jstate2, out=js6);
    
    And(a=jstate0, b=jstate1, out=js7a);
    And(a=js7a, b=jstate2, out=js7);
    
    // Traffic light X outputs
    Or(a=js0, b=js1, out=xr1);
    Or(a=js4, b=js5, out=xr2);
    Or(a=js6, b=js7, out=xr3);
    Or(a=xr1, b=xr2, out=xr4);
    Or(a=xr4, b=xr3, out=X[2]);
    
    Or(a=js1, b=js3, out=X[1]);
    Or(a=js2, b=false, out=X[0]);
    
    // Traffic light Z outputs
    Or(a=js0, b=js1, out=zr1);
    Or(a=js2, b=js3, out=zr2);
    Or(a=js4, b=js5, out=zr3);
    Or(a=zr1, b=zr2, out=zr4);
    Or(a=zr4, b=zr3, out=Z[2]);
    
    Or(a=js5, b=js7, out=Z[1]);
    Or(a=js6, b=false, out=Z[0]);
    
    // Countdown timer (9 to 0)
    Or(a=count0, b=count1, out=cor01);
    Or(a=count2, b=count3, out=cor23);
    Or(a=cor01, b=cor23, out=countNotZero);
    Not(in=countNotZero, out=countIsZero);
    
    // Counting done when counter reaches 0 and we're in crossing mode
    And(a=countIsZero, b=crossingActive, out=countingDone);
    Not(in=countingDone, out=notCountingDone);
    
    // Decrement counter when in crossing mode and not at 0
    And(a=crossingActive, b=countNotZero, out=shouldDecrement);
    
    // Counter decrement logic
    Not(in=count0, out=notcount0);
    Mux(a=count0, b=notcount0, sel=shouldDecrement, out=dec0);
    
    Not(in=count0, out=borrow0);
    And(a=borrow0, b=shouldDecrement, out=flip1);
    Not(in=count1, out=notcount1);
    Mux(a=count1, b=notcount1, sel=flip1, out=dec1);
    
    Or(a=count0, b=count1, out=or01b);
    Not(in=or01b, out=borrow1);
    And(a=borrow1, b=shouldDecrement, out=flip2);
    Not(in=count2, out=notcount2);
    Mux(a=count2, b=notcount2, sel=flip2, out=dec2);
    
    Or(a=or01b, b=count2, out=or012);
    Not(in=or012, out=borrow2);
    And(a=borrow2, b=shouldDecrement, out=flip3);
    Not(in=count3, out=notcount3);
    Mux(a=count3, b=notcount3, sel=flip3, out=dec3);
    
    // Wrap from 0 to 9 when starting crossing
    And(a=countIsZero, b=startCrossing, out=wrapTo9);
    
    Mux(a=dec0, b=true, sel=wrapTo9, out=next0);
    Mux(a=dec1, b=false, sel=wrapTo9, out=next1);
    Mux(a=dec2, b=false, sel=wrapTo9, out=next2);
    Mux(a=dec3, b=true, sel=wrapTo9, out=next3);
    
    Or(a=shouldDecrement, b=startCrossing, out=loadCounter);
    And(a=loadCounter, b=PowerOn, out=counterLoadTemp);
    Or(a=counterLoadTemp, b=false, out=loadCnt);
    
    Bit(in=next0, load=loadCnt, out=count0);
    Bit(in=next1, load=loadCnt, out=count1);
    Bit(in=next2, load=loadCnt, out=count2);
    Bit(in=next3, load=loadCnt, out=count3);
    
    // 7-segment decoder
    Decoder(in[0]=count0, in[1]=count1, in[2]=count2, in[3]=count3,
            a=C[0], b=C[1], c=C[2], d=C[3], e=C[4], f=C[5], g=C[6]);
}