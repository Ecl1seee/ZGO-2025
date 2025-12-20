.586
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib ..\Debug\Static_Library.lib
ExitProcess PROTO :DWORD

outlich PROTO :DWORD
outrad  PROTO :DWORD
slength PROTO :DWORD, :DWORD
atoi_func PROTO :DWORD, :DWORD
itoa_func PROTO :DWORD, :DWORD
copystr PROTO :DWORD, :DWORD
compare PROTO :DWORD, :DWORD, :DWORD
concat  PROTO :DWORD, :DWORD, :DWORD
_pause  PROTO

.stack 4096

.const
	newline byte 13, 10, 0
	buffer  byte 256 dup(0)
	LTRL1 sdword 4
	LTRL2 sdword 2
	LTRL3 sdword 12
	LTRL4 sdword 24
	LTRL5 sdword 1
	LTRL6 byte 'Flag was FALSE, now TRUE', 0
	LTRL7 sdword 0
	LTRL8 byte 'Flag was TRUE, now FALSE', 0
	LTRL9 byte 'No status', 0
	LTRL10 byte '--- Math Operation ---', 0
	LTRL11 sdword 5
	LTRL12 byte 'Input: ', 0
	LTRL13 byte 'Result (expect 20): ', 0
	LTRL14 byte '--- String & Char & Libs ---', 0
	LTRL15 byte 'ZGO-2025', 0
	LTRL16 byte 'String: ', 0
	LTRL17 byte 'Length as string: ', 0
	LTRL18 byte 'F', 0
	LTRL19 byte 'Char literal: ', 0
	LTRL20 byte '--- Boolean Logic ---', 0
	LTRL21 byte 'Start boolean: ', 0
.data
	temp sdword ?
	func_arg_temp sdword 10 dup(0)
	mathOp_val sdword 0
	mathOp_res sdword 0
	logicOp_flag sdword 0
	logicOp_inverted sdword 0
	logicOp_status sdword 0
	main_x sdword 0
	main_y sdword 0
	main_str sdword 0
	main_len sdword 0
	main_lenStr sdword 0
	main_sym sdword 0
	main_bVal sdword 0
	main_logicRes sdword 0

.code

; ----------- mathOp ------------
mathOp PROC
	push ebx
	push edx
	push esi
	push edi
	push mathOp_res
	push mathOp_res
	pop eax
	pop ebx
	mov mathOp_res, eax
	push mathOp_val
	push LTRL1
	pop ebx
	pop eax
	sub eax, ebx
	push eax
	push mathOp_res
	push mathOp_res
	push mathOp_res
	pop eax
	pop ebx
	mov mathOp_res, eax
	push mathOp_res
	push mathOp_val
	pop ebx
	pop eax
	add eax, ebx
	push eax
	push LTRL2
	pop ebx
	pop eax
	cdq
	imul ebx
	push eax
	push mathOp_res
	push LTRL3
	push mathOp_res
	pop eax
	pop ebx
	mov mathOp_res, eax
	push mathOp_res
	push LTRL2
	pop ebx
	pop eax
	cdq
	imul ebx
	push eax
	push LTRL4
	push mathOp_res
	pop eax
	pop ebx
	mov mathOp_res, eax
	push mathOp_res
	pop edi
	pop esi
	pop edx
	pop ebx
	ret
mathOp ENDP
	pop eax
	pop edi
	pop esi
	pop edx
	pop ebx
	ret

; ----------- logicOp ------------
logicOp PROC
	push ebx
	push edx
	push esi
	push edi
	push logicOp_inverted
	push logicOp_status
	push logicOp_inverted
	pop eax
	pop ebx
	mov logicOp_inverted, eax
	push logicOp_flag
	push logicOp_inverted
	push LTRL5
	push logicOp_status
	pop eax
	pop ebx
	mov logicOp_status, eax
	push offset LTRL6
	push LTRL7
	push logicOp_status
	pop eax
	pop ebx
	mov logicOp_status, eax
	push offset LTRL8
	push logicOp_status
	pop eax
	pop ebx
	mov logicOp_status, eax
	push offset LTRL9
	pop edi
	pop esi
	pop edx
	pop ebx
	ret
logicOp ENDP
	pop eax
	pop edi
	pop esi
	pop edx
	pop ebx
	ret

; ----------- MAIN ------------
main PROC
	push offset LTRL10
	call outrad
	push offset newline
	call outrad
	push main_x
	pop eax
	pop ebx
	mov main_x, eax
	push LTRL11
	push main_y
	push main_y
	pop eax
	pop ebx
	mov main_y, eax
	push main_x
	push offset LTRL12
	call outrad
	push main_x
	call outlich
	push offset newline
	call outrad
	push offset LTRL13
	call outrad
	push main_y
	call outlich
	push offset newline
	call outrad
	push offset newline
	call outrad
	push offset LTRL14
	call outrad
	push offset newline
	call outrad
	push main_str
	pop eax
	pop ebx
	mov main_str, eax
	push offset LTRL15
	push main_len
	push main_lenStr
	push main_len
	pop eax
	pop ebx
	mov main_len, eax
	push main_str
	push main_lenStr
	pop eax
	pop ebx
	mov main_lenStr, eax
	push main_len
	push offset LTRL16
	call outrad
	push main_str
	call outrad
	push offset newline
	call outrad
	push offset LTRL17
	call outrad
	push main_lenStr
	call outrad
	push offset newline
	call outrad
	push main_sym
	pop eax
	pop ebx
	mov main_sym, eax
	push offset LTRL18
	push offset LTRL19
	call outrad
	push main_sym
	call outrad
	push offset newline
	call outrad
	push offset newline
	call outrad
	push offset LTRL20
	call outrad
	push offset newline
	call outrad
	push main_bVal
	pop eax
	pop ebx
	mov main_bVal, eax
	push LTRL5
	push main_logicRes
	push main_logicRes
	pop eax
	pop ebx
	mov main_logicRes, eax
	push main_bVal
	push offset LTRL21
	call outrad
	push main_logicRes
	call outrad
	push offset newline
	call outrad
	push 0
	call ExitProcess
main ENDP
end main
