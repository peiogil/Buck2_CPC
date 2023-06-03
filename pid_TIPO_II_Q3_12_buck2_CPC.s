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
; void PIDInitBuck2 ( tPID *fooPIDStruct )
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

; definicion de la funcion _PIDInitBuck2
.global _PIDInitBuck2
; inicio de la funcion
_PIDInitBuck2:
; se resetean las posiciones de memoria donde iran los errores y los d.
push w0
add #offsetcontrolOutput, w0
clr [w0]
pop w0
push w0
mov [w0 + #offsetcontrolHistory], w0
clr [w0++] ; e[n] = 0
clr [w0++] ; e[n-1] = 0
clr [w0++] ; e[n-2] = 0
clr [w0++] ; d[n-1]=0
clr [w0] ; d[n-2] = 0

pop w0 
return
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _PIDBOOST:
; Prototype:
;              tPID PID ( tPID *fooPIDStruct )
;
; Operation:
;
;                                           
;Reference                                                        
;Input         ---                    ------------------                 ----------
;     --------| + |  Control         |b0+b1Z^-1..b3Z^-3 | Control       | Output   |       
;             |   |----------|----|  |---------------   | ------------  | Plant    |----
;        -----| - |Difference        | 1+a1Z^-1..a3Z^-3 |      	        |          |    |
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
;Justificación de cómo se trabaja con la reprentación Q3.12 en
;GuionNoAprendizajePorProyectosPictail_PRACTICAS_PRESENCIAL
  .global _PIDBUCK2                    ; provide global scope to routine
_PIDBUCK2:
        ;btg LATD, #1
        ; Save working registers.
        push.s
        push    w4
        push    w5        
        push    w8
        push    w10
        
        push    CORCON                  ; Prepare CORCON for fractional computation.


        fractsetup      w8	; macro para q
; se toman los valores de los campos de la variable tpid en los siguientes resitros de trabajo
        mov [w0 + #offsetabcCoefficients], w8    ; w8 = Base Address of _abcCoefficients array 
;w10 <----- puntero a la primera posicion de memoria con error o d (memoria y)      
 		mov [w0 + #offsetcontrolHistory], w10    ; w10 = Base Address of _ControlHistory array (state/delay line)

;w2 <----- valor actual de la variable a controlar (vout del reductor 2)
        mov [w0 + #offsetmeasuredOutput], w2
;w3 <----- valor actual de la referencia a alcanzar
        mov [w0 + #offsetcontrolReference], w3
;recolocacion de las posiciones de error en sus nuevas posiciones
; dejando e[n] listo para actualizarlo con el valor nuevo de error.
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
		; (se redondea, porque acc 40 bits y [w10], 16 bits)
		sac.r a, [w10]
;------Funcion de transferencia a programar----------------------
;
;		 1.638 z^2 + 0.02142 z - 1.617
;		-----------------------------
;   		z^2 - 1.968 z + 0.9678
;
;-----------------------------------------------------------------
		;[w10] apunta a e[n] de la matriz Buck2Votalge History

; w4 = b1, w5 = e[n-1] , [w8]=b2 ,[w10]=e[n-2]
		clr a
		mov [w8++],w4
		mov [w10++],w5		
		mac w4*w5, a, [w8]+=2, w4, [w10]+=2, w5
;-----------------------------------------------------------------
		; a = e[n]+b0 * e[n] + b1 * e[n-1]
		; w4 = b2, w5 = e[n-2], [w8]=a1 ,[w10]=d[n-1]
		mac w4*w5, a, [w8]+=2, w4, [w10]+=2, w5
;--------------------------------------------------------------------
		; a = b0 * e[n] + b1 * e[n-1] + b2 * e[n-2]
		; w4 = a1, w5 = d[n-1], [w8]=a2 ,[w10]=d[n-2]
		mac w4*w5, a, [w8]+=2, w4, [w10]+=2, w5

;-------------------------------------------------------------------
		;a = e[n]+b0 * e[n] + b1 * e[n-1] + b2 * e[n-2] +a1 * d[n-1]
		; w4 = a2, w5 = d[n-2], [w8]=a2 ,[w10]=d[n-1]
		mac w4*w5, a, [w8], w4, [w10]-=2, w5
		; se mete d[n-2] en el registro intermedio del acumulador b (high)
;-----------------------------------------------------------------------------
		; a = e[n]+b0 * e[n] + b1 * e[n-1] + b2 * e[n-2] +a1 * d[n-1]+ a2 * d[n-2] 
		mac w4*w5, a, 	
;-------------------------------------------------------------------------------
;Se guarda a en w5 para conservar el signo 
sac a,w5
;Se deplaza el acumulador 3 posiciones
;hacia la izquierda para ajustar a Q3_12
sftac a,#-3
sac.r a,w4
;Ahora, el signo guardado en w5, se pasa a w4
btss w5,#15
bra clr15
bset w4,#0xf
bra seguir
clr15:
bclr w4,#15

seguir:


;mov w4, [w0 + #offsetcontrolOutput]

mov [w10],w11
mov w11, [w10 + #+2] ; d[n-1,anterior]==>d[n-2]
mov w4,[w10]		;;w4==>d[n-1]
mov w4, [w0 + #offsetcontrolOutput]
; se recupera de la pila lo que había en los registros que han sido usados
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





