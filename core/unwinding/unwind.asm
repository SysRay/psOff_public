.CODE

PUBLIC  unwinding_setjmp
unwinding_setjmp PROC
  MOV RAX, RDI
  MOV RDX ,qword ptr [RSP ]
  MOV qword ptr [RAX ],RDX
  MOV qword ptr [RAX  + 8h ],RBX
  MOV qword ptr [RAX  + 10h ],RSP
  MOV qword ptr [RAX  + 18h ],RBP
  MOV qword ptr [RAX  + 20h ],R12
  MOV qword ptr [RAX  + 28h ],R13
  MOV qword ptr [RAX  + 30h ],R14
  MOV qword ptr [RAX  + 38h ],R15

  FNSTCW     word ptr [RAX  + 40h ]
  STMXCSR    dword ptr [RAX  + 44h ]
  mov rax, 1
  ret
unwinding_setjmp ENDP

PUBLIC unwinding_longjmp
unwinding_longjmp PROC
  MOV        RDX ,RDI
  STMXCSR    dword ptr [RSP - 4h ]
  MOV        EAX ,dword ptr [RDX  + 44h ]
  AND        EAX , 0ffffffc0h
  MOV        EDI,dword ptr [RSP  - 4h ]
  AND        EDI ,3fh
  XOR        EDI ,EAX
  MOV        dword ptr [RSP  - 4h ],EDI
  LDMXCSR    dword ptr [RSP  - 4h ]

  MOV        RCX ,qword ptr [RDX ]
  MOV        RBX ,qword ptr [RDX  + 8h ]
  MOV        RSP ,qword ptr [RDX  + 10h ]
  MOV        RBP ,qword ptr [RDX  + 18h ]
  MOV        R12 ,qword ptr [RDX  + 20h ]
  MOV        R13 ,qword ptr [RDX  + 28h ]
  MOV        R14 ,qword ptr [RDX  + 30h ]
  MOV        R15 ,qword ptr [RDX  + 38h ]
  FLDCW      word ptr [RDX  + 40h ]
  MOV        RAX ,0

  MOV        qword ptr [RSP ],RCX
  ret
unwinding_longjmp ENDP
End