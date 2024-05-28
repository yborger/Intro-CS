	.text
	.globl	sum
	.type	sum, @function
sum:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	subl	$24, %esp  # create some space for local variables (you can
                           # (allocate more if you'd like: ex. subl $30, %esp)

        # implement the body of this function here
        # the parameter is allocated on the stack at addresss:  8(%ebp)
        # you can used -4(%ebp) and -8(%ebp) as space for local vars i and res
        
        movl 8(%ebp), %edx #n stored at %edx
        movl $1, -4(%ebp) #i=1

        cmpl $0, %edx # n-0 check put into cc
        jle .L1 # jump <= so if n is negative or 0

        movl $0, -8(%ebp) #res = 0
        cmpl %edx, -4(%ebp)  #(i - n) put into cc
        jg .finish # jump > so if i is greater than n 
        
        .loop:
          movl -4(%ebp), %eax #i into %eax 
          addl %eax, -8(%ebp) #res + i 
          addl $1, -4(%ebp)  #i++
          cmpl %edx, -4(%ebp) #(i-n) new values put into cc  
          jle .loop #jump back to loop if i <= n
          jg .finish #once i>n get out of the loop
        
        .L1: 
          movl $-1, -8(%ebp)  #-1 into %ebp
          jmp .finish

        .finish:
          movl -8(%ebp), %eax #set what is in %ebp into the returning spot 

	leave
	ret #returning %eax
	.size	sum, .-sum
