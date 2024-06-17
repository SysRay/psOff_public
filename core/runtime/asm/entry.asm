.CODE

PUBLIC  jmpEntry
jmpEntry PROC

  ; Input
  ;   RDI addr
  ;   RSI EntryParams
  ;   RDX length of EntryParams
  ;   RCX exitHandler

  mov RAX, RDI

  ; Calc needed stacksize
  mov RDI, RDX
  add RDI, 1 ; entryparams + it's length
  shl RDI, 3

  ; align stack
  SUB RSP, RDI
  AND RSP, -10h

  ; copy entry param to stack
  add RDI, 8
  add RSP, RDI

; ### do while
  ; counter (offset)
  mov RDI, RDX
  shl RDI, 3

loopStart:
  sub RDI, 8
  push [RSI + RDI]
  test RDI, RDI
  jnz loopStart

  push RDX
  ; -

  lea RDI, [RSP] ; param 1 (EntryParam) is @ rsp
  mov RSI, RCX ; param2 (atexit)
  jmp RAX

jmpEntry ENDP
End