	.cpu cortex-m7
	.arch armv7e-m
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 1
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #32
	movs	r3, #104
	strb	r3, [sp, #16]
	movs	r3, #97
	strb	r3, [sp, #17]
	movs	r3, #108
	strb	r3, [sp, #18]
	strb	r3, [sp, #19]
	movs	r2, #111
	strb	r2, [sp, #20]
	movs	r2, #32
	strb	r2, [sp, #21]
	movs	r2, #0
	strb	r2, [sp, #22]
	movs	r1, #119
	strb	r1, [sp]
	movs	r1, #101
	strb	r1, [sp, #1]
	strb	r3, [sp, #2]
	movs	r3, #116
	strb	r3, [sp, #3]
	strb	r2, [sp, #4]
	add	r3, sp, #15
.L2:
	ldrb	r2, [r3, #1]!	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L2
	mov	r1, sp
	subs	r2, r3, #1
	adds	r3, r3, #10
.L4:
	ldrb	r0, [r1]	@ zero_extendqisi2
	strb	r0, [r2, #1]!
	cbz	r0, .L3
	adds	r1, r1, #1
	cmp	r2, r3
	bne	.L4
.L3:
	movs	r0, #0
	add	sp, sp, #32
	@ sp needed
	bx	lr
	.size	main, .-main
	.ident	"GCC: (Arm GNU Toolchain 15.2.Rel1 (Build arm-15.86)) 15.2.1 20251203"
