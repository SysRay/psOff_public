.CODE

PUBLIC  jmpEntry
jmpEntry PROC

  ; Input
  ;   RDI addr
  ;   RSI EntryParams
  ;   RDX exitHandler

  mov RAX, RDI

  ; align stack
  SUB RSP, 32
  AND RSP, -10h

  ; copy entry param to stack
  ADD RSP, 24
  push [RSI+8]
  push [RSI]
  ; -

  lea rdi, [RSP] ; param 1 (EntryParam) is @ rsp
  mov rsi, RDX ; param2 (atexit)
  jmp RAX

jmpEntry ENDP
End