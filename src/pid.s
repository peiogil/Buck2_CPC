; ******************************************************************************
; * © 2008 Microchip Technology Inc.
; *
; SOFTWARE LICENSE AGREEMENT:
; Microchip Technology Incorporated ("Microchip") retains all ownership and 
; intellectual property rights in the code accompanying this message and in all 
; derivatives hereto.  You may use this code, and any derivatives created by 
; any person or entity by or on your behalf, exclusively with Microchip's
; proprietary products.  Your acceptance and/or use of this code constitutes 
; agreement to the terms and conditions of this notice.
;
; CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
; WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
; TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
; PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
; PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
;
; YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
; IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
; STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
; PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
; ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
; ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
; ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
; THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
; HAVE THIS CODE DEVELOPED.
;
; You agree that you are solely responsible for testing the code and 
; determining its suitability.  Microchip has no obligation to modify, test, 
; certify, or support the code.
;
; *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        ; Local inclusions.
        .nolist
    	.include        "dspcommon.inc"         ; fractsetup
        .list

        .equ    offsetabcCoefficients, 0
        .equ    offsetcontrolHistory, 2
        .equ    offsetcontrolOutput, 4
        .equ    offsetmeasuredOutput, 6
        .equ    offsetcontrolReference, 8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .section .libdsp, code 	; la libreria dsp se añade

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _PIDInitBuck2:
;
; Prototype:
; void PIDInitBuck( tPID *fooPIDStruct )
;
; Operation: This routine clears the delay line elements in the array
;            _ControlHistory, as well as clears the current PID output
;            element, _ControlOutput
;
; Input:
;       w0 = Address of data structure tPID (type defined in dsp.h)
;
; Return:
;       (void)
;
; System resources usage:
;       w0             used, restored
;
; DO and REPEAT instruction usage.
;       0 level DO instruction
;       0 REPEAT intructions
;
; Program words (24-bit instructions):
;       ??
;
; Cycles (including C-function call and return overheads):
;       ??
;............................................................................

; definicion de la funcion _PIDInitBuck
.global _PIDInitBuck2
; inicio de la funcion
_PIDInitBuck2:
;Despues del SoftStart para que inicie con error 0 y d=0x34E0
push w1
push w0
mov #0x34E0,w1
add #offsetcontrolOutput, w0 ;d[n]=0X34E0
mov w1, [w0]
pop w0
push w0
mov [w0 + #offsetcontrolHistory], w0
clr [w0++] ; E[n] = 0
clr [w0++] ; E[n-1] = 0
clr [w0++] ; E[n-2] = 0

mov w1,[w0++] ; d[n-1]=0X34E0
mov w1,[w0] ; d[n-2]=0X34E0

pop w0
pop w1 
return
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _PIDBuck2:
; Prototype:
;              tPID PID ( tPID *fooPIDStruct )
;
; Operation:
;
;                                           
;Reference                                                        
;Input         ---                    ------------------                 ----------
;     --------| + |  Control         |B0+B1Z^-1+B3Z^-3 | Control       | Output   |       
;             |   |----------|----|  |---------------  | ------------  | Plant    |----
;        -----| - |Difference        | 1+A1Z^-1+A2Z^-2 |      	        |          |    |
;       |      ---  (error)  |        ------------------                 -----------    |
;       |                                                                               |
;       | Measured                                                                      |
;       | Outut                                                                         |
;       |                                                                               |
;       |                                                                               |
;       |                                                                               |
;        --------------------------------------------------------------------------------
;
; Input:
;       w0 = Address of tPID data structure

; Return:
;       w0 = Address of tPID data structure
;
; System resources usage:
;       {w0..w5}        used, not restored
;       {w8,w10}        saved, used, restored
;        AccA, AccB     used, not restored
;        CORCON         saved, used, restored
;
; DO and REPEAT instruction usage.
;       0 level DO instruction
;       0 REPEAT intructions
;
; Program words (24-bit instructions):
;       ??
;
; Cycles (including C-function call and return overheads):
;       ??
;............................................................................

  .global _PIDBuck2                   ; provide global scope to routine
_PIDBuck2:
        ;btg LATD, #1
        ; Save working registers.
        push.s
        push    w4
        push    w5        
        push    w8
        push    w10

        
        push    CORCON                  ; Prepare CORCON for fractional computation.

;	0.3707 z^2 + 0.03212 z - 0.3386
;	-------------------------------
;    z^2 - 1.026 z + 0.02604

        fractsetup      w8	; macro para programar CORCON: fraccional y saturacion
; se toman los valores de los campos de la variable tpid en los siguientes resitros de trabajo
        mov [w0 + #offsetabcCoefficients], w8    ; w8 = Base Address of _abcCoefficients array 
;w10 <----- puntero a la primera posicion de memoria con error o d (memoria y)      
 		mov [w0 + #offsetcontrolHistory], w10    ; w10 = Base Address of _ControlHistory array (state/delay line)
;w1 <----- valor d antes de hacer el nuevo calculo
        mov [w0 + #offsetcontrolOutput], w1
;w2 <----- valor actual de la variable a controlar (vout del reductor 2)
        mov [w0 + #offsetmeasuredOutput], w2
;w3 <----- valor actual de la referencia a alcanzar
        mov [w0 + #offsetcontrolReference], w3
;recolocacion de las posiciones de control y error en sus nuevas posiciones
;dejando d(n-1) y e[n] listo para actualizarlo con el valor nuevo de error.
		mov [w10+#+6],w11 
		mov w11,[w10+#+8]	;d(n-1)==> d[n-2]
		mov w1,[w10+#+6] 	;d[n]==> d[n-1]
		mov [w10 + #+2],w11
		mov w11, [w10 + #+4] ;e[n-1]==> e[n-2]
		mov [w10],w11
		mov w11, [w10 + #+2] ;e[n]==> e[n-1]

		; ([W10])-->e[n]<== LIBRE
; Calculate most recent error with saturation, no limit checking required
		; acumulador a<-----w3(referencia), a = tpid.controlreference
		lac w3, a
		; acumulador b<-----w2(valor actual de la variable a controlar), b = tpid.measuredoutput
		lac w2, b
		; acumulador a<-----referencia-valor actual, error [n]
		sub a
		; se redondea sac.r(store acumulator with rounding) 
		;y se mete en [w10], e[n]
		; (se redondea, porque acc 40 bits y [w10], 16 bits)AclararComoRedondeaDSPIC
		sac.r a, [w10]
		; se limpia el acumulador a, y se prepara en w4 y w5
		clr a, [w8]+=2, w4, [w10]+=2, w5 	; w4 = B0, w5 = E[n]
		; se limpia b
		clr b
;-----------------------------------------------------------------
		; se multiplica y se prepara la siguiente multiplicacion
		; a = b0 * e[n]
		; w4 = b1, w5 = e[n-1]
		mac w4*w5, a, [w8]+=2, w4, [w10]+=2, w5
;-----------------------------------------------------------------
		; a = b0 * e[n] + b1 * e[n-1]
		; w4 = b2, w5 = e[n-2]
		mac w4*w5, a, [w8]+=2, w4, [w10]+=2, w5
;--------------------------------------------------------------------
		; a = b0 * e[n] + b1 * e[n-1] + b2 * e[n-2]
		; w4 = a1, w5 = d[n-1]
		mac w4*w5, a, [w8]+=2, w4, [w10]+=2, w5
;------------------------------------------------------------------
		; se mete d[n-1] en b
		lac w5, b
		;a = b0 * e[n] + b1 * e[n-1] + b2 * e[n-2] + d[n-1]
		add a
		; a = b0 * e[n] + b1 * e[n-1] + b2 * e[n-2] +  d[n-1] + a1 * d[n-1]
		; en este caso el coeficiente a1, tiene parte entera '1' y parte fraccionaria a1
		; w4 = a2, w5 = d[n-2]
		mac w4*w5, a, [w8], w4, [w10]-=2, w5
		; se mete d[n-2] en el registro intermedio del acumulador b (high)
;-----------------------------------------------------------------------------
		;antes [w8]+=2 ahora [w8] Y [w10] retrocede de pos[6] a pos[4] en la matriz de historia
		mac w4*w5, a
		; a = b0 * e[n] + b1 * e[n-1] - b2 * e[n-2] + (1 + a1) * d[n-1] - (a2) * d[n-2]
		; se tiene en a, el valor de la ecuacion en diferencias del regulador tipo2
		; se redondea el valor de d que hay en acc a (40 bits) y se mete la parte mas significativa en w1(16 bits)
		sac.r a, w1
;-------------------------------------------------------------------------------

;0.3707 z^2 + 0.03212 z - 0.3386
;-------------------------------
;   z^2 - 1.026 z + 0.02604

;recolocacion de las posiciones de d en sus nuevas posiciones
;mov [w10 + #+4],w11
;mov w11, [w10 + #+6] ; d[n-2,anterior]==>d[n-3]
;mov [w10 + #+2],w11
;mov w11, [w10 + #+4] ; d[n-1,anterior]==>d[n-2]
;mov [w10],w11
;mov w11, [w10 + #+2] ; d[n,anterior]==>d[n-1]
;mov w1,[w10]
;mov w1, [w0 + #offsetcontrolOutput]
;---------------------------------------------------------
mov [w10],w11
mov w11, [w10 + #+2] ; d[n-1,anterior]==>d[n-2]
mov w1,[w10]		;w1==>d[n-1]
mov w1, [w0 + #offsetcontrolOutput]
; se recupera de la pila lo que habia en los registros que han sido usados
pop CORCON
pop w10
pop w8
pop w5
pop w4
pop.s
return

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF

