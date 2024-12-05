	.text

__sigfe_maybe:
	pushl	%ebx
	pushl	%edx
	movl	%fs:4,%ebx			# location of bottom of stack
	addl	$-7848,%ebx	# where we will be looking
	cmpl	%ebx,%esp			# stack loc > than tls
	jge	0f				# yep.  we don't have a tls.
	subl	$-7848,%ebx	# where we will be looking
	movl	-7848(%ebx),%eax
	cmpl	$0xc763173f,%eax		# initialized?
	je	1f
0:	popl	%edx
	popl	%ebx
	ret

__sigfe:
	pushl	%ebx
	pushl	%edx
	movl	%fs:4,%ebx			# location of bottom of stack
1:	movl	$1,%eax			# potential lock value
	xchgl	%eax,-8880(%ebx)	# see if we can grab it
	movl	%eax,-8884(%ebx)	# flag if we are waiting for lock
	testl	%eax,%eax			# it will be zero
	jz	2f				#  if so
	call	_yield				# should be a short-time thing, so
	jmp	1b				# sleep and loop
2:	movl	$4,%eax			# have the lock, now increment the
	xadd	%eax,-8876(%ebx)	#  stack pointer and get pointer
	leal	__sigbe,%edx			# new place to return to
	xchgl	%edx,12(%esp)			# exchange with real return value
	movl	%edx,(%eax)			# store real return value on alt stack
	incl	-8888(%ebx)
	decl	-8880(%ebx)		# remove lock
	popl	%edx				# restore saved value
	popl	%ebx
	ret

	.global	__sigbe
__sigbe:					# return here after cygwin syscall
	pushl	%eax				# don't clobber
	pushl	%ebx				# tls pointer
1:	movl	%fs:4,%ebx			# address of bottom of tls
	movl	$1,%eax			# potential lock value
	xchgl	%eax,-8880(%ebx)	# see if we can grab it
	movl	%eax,-8884(%ebx)	# flag if we are waiting for lock
	testl	%eax,%eax			# it will be zero
	jz	2f				#  if so
	call	_yield				# sleep
	jmp	1b				#  and loop
2:	movl	$-4,%eax			# now decrement aux stack
	xadd	%eax,-8876(%ebx)	#  and get pointer
	movl	-4(%eax),%eax			# get return address from signal stack
	xchgl	%eax,4(%esp)			# swap return address with saved eax
	decl	-8888(%ebx)
	decl	-8880(%ebx)		# release lock
	popl	%ebx
	ret

	.global	_sigdelayed
_sigdelayed:
	pushl   %ebp
	movl    %esp,%ebp
	# We could be called from an interrupted thread which doesn't know
	# about his fate, so save and restore everything and the kitchen sink.
	pushf
	pushl   %esi
	pushl   %edi
	pushl   %edx
	pushl   %ecx
	pushl   %ebx
	pushl   %eax
	subl	$0x84,%esp
	fnstcw  0x80(%esp)
	movdqu	%xmm7,0x70(%esp)
	movdqu	%xmm6,0x60(%esp)
	movdqu	%xmm5,0x50(%esp)
	movdqu	%xmm4,0x40(%esp)
	movdqu	%xmm3,0x30(%esp)
	movdqu	%xmm2,0x20(%esp)
	movdqu	%xmm1,0x10(%esp)
	movdqu	%xmm0,(%esp)
	movl	%fs:4,%ebx				# get tls
	pushl   -9888(%ebx) # saved errno

	movl	$-12700,%eax		# point to beginning
	addl	%ebx,%eax				#  of tls block
	call	__ZN7_cygtls19call_signal_handlerEv@4	# call handler

	movl	%fs:4,%ebx				# reget tls
1:	movl	$1,%eax				# potential lock value
	xchgl	%eax,-8880(%ebx)		# see if we can grab it
	movl	%eax,-8884(%ebx)		# flag if we are waiting for lock
	testl	%eax,%eax				# it will be zero
	jz	2f					#  if so
	call	_yield					# sleep
	jmp	1b					#  and loop
2:	popl	%edx					# saved errno
	testl	%edx,%edx				# Is it < 0
	jl	3f					# yup.  ignore it
	movl	-9872(%ebx),%eax
	movl	%edx,(%eax)
3:	movl	$-4,%eax				# now decrement aux stack
	xadd	%eax,-8876(%ebx)		#  and get pointer
	xorl	%ebp,%ebp
	xchgl	%ebp,-4(%eax)				# get return address from signal stack
	xchgl	%ebp,0xa0(%esp)				# store real return address
leave:	xorl	%eax,%eax
	movl	%eax,-8888(%ebx)
	movl	%eax,-8880(%ebx)		# unlock

	movdqu	(%esp),%xmm0
	movdqu	0x10(%esp),%xmm1
	movdqu	0x20(%esp),%xmm2
	movdqu	0x30(%esp),%xmm3
	movdqu	0x40(%esp),%xmm4
	movdqu	0x50(%esp),%xmm5
	movdqu	0x60(%esp),%xmm6
	movdqu	0x70(%esp),%xmm7
	fninit
	fldcw   0x80(%esp)
	addl	$0x84,%esp
	popl	%eax
	popl	%ebx
	popl	%ecx
	popl	%edx
	popl	%edi
	popl	%esi
	popf
	ret

	.global __ZN7_cygtls3popEv@4
__ZN7_cygtls3popEv@4:
1:	pushl	%ebx
	movl	%eax,%ebx			# this
	movl	$-4,%eax
	xadd	%eax,3824(%ebx)
	movl	-4(%eax),%eax
	popl	%ebx
	ret

# _cygtls::lock
	.global	__ZN7_cygtls4lockEv@4
__ZN7_cygtls4lockEv@4:
	pushl	%ebx
	movl	%eax,%ebx
1:	movl	$1,%eax
	xchgl	%eax,3820(%ebx)
	testl	%eax,%eax
	jz	2f
	call	_yield
	jmp	1b
2:	popl	%ebx
	ret

# _cygtls::unlock
	.global	__ZN7_cygtls6unlockEv@4
__ZN7_cygtls6unlockEv@4:
	decl	3820(%eax)
	ret

	.global	__ZN7_cygtls6lockedEv
__ZN7_cygtls6lockedEv:
	movl	3820(%eax),%eax
	ret

	.extern	__ZN7_cygtls19call_signal_handlerEv@4
stabilize_sig_stack:
	movl	%fs:4,%ebx
1:	movl	$1,%eax
	xchgl	%eax,-8880(%ebx)
	movl	%eax,-8884(%ebx)		# flag if we are waiting for lock
	testl	%eax,%eax
	jz	2f
	call	_yield
	jmp	1b
2:	incl	-8888(%ebx)
	cmpl	$0,-8892(%ebx)
	jz	3f
	decl	-8880(%ebx)			# unlock
	movl	$-12700,%eax		# point to beginning
	addl	%ebx,%eax				#  of tls block
	call	__ZN7_cygtls19call_signal_handlerEv@4
	jmp	1b
3:	decl	-8888(%ebx)
	ret

	.globl	_sigsetjmp
_sigsetjmp:
	pushl   %ebp
	movl    %esp,%ebp
	pushl   %edi
	movl	8(%ebp),%edi			# &sigjmp_buf
	movl	12(%ebp),%eax			# savemask
	movl	%eax,208(%edi)			# store savemask
	testl	%eax,%eax			# savemask != 0?
	je	1f				# no, skip fetching sigmask
	subl	$12,%esp
	leal	212(%edi),%eax			# &sigjmp_buf.sigmask
	movl	%eax,8(%esp)			# -> 3rd param "oldset"
	xorl	%eax,%eax
	movl	%eax,4(%esp)			# NULL -> 2nd param "set"
	movl	%eax,(%esp)			# SIG_SETMASK -> 1st param "how"
	call	_pthread_sigmask
	addl	$12,%esp
	jmp	1f

	.globl	_setjmp
_setjmp:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	movl	8(%ebp),%edi
1:
	movl	%eax,0(%edi)
	movl	%ebx,4(%edi)
	movl	%ecx,8(%edi)
	movl	%edx,12(%edi)
	movl	%esi,16(%edi)
	movl	-4(%ebp),%eax
	movl	%eax,20(%edi)
	movl	0(%ebp),%eax
	movl	%eax,24(%edi)
	movl	%esp,%eax
	addl	$12,%eax
	movl	%eax,28(%edi)
	movl	4(%ebp),%eax
	movl	%eax,32(%edi)
	movw	%es,%ax
	movw	%ax,36(%edi)
	movw	%fs,%ax
	movw	%ax,38(%edi)
	movw	%gs,%ax
	movw	%ax,40(%edi)
	movw	%ss,%ax
	movw	%ax,42(%edi)
	movl	%fs:0,%eax
	movl	%eax,44(%edi)
	fnstcw  48(%edi)
	pushl	%ebx
	call	stabilize_sig_stack
	movl	-8876(%ebx),%eax	# save stack pointer contents
	decl	-8880(%ebx)
	popl	%ebx
	movl	%eax,52(%edi)
	popl	%edi
	movl	$0,%eax
	leave
	ret

	.globl	___sjfault
___sjfault:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	movl	8(%ebp),%edi
	movl	%eax,0(%edi)
	movl	%ebx,4(%edi)
	movl	%ecx,8(%edi)
	movl	%edx,12(%edi)
	movl	%esi,16(%edi)
	movl	-4(%ebp),%eax
	movl	%eax,20(%edi)
	movl	0(%ebp),%eax
	movl	%eax,24(%edi)
	movl	%esp,%eax
	addl	$12,%eax
	movl	%eax,28(%edi)
	movl	4(%ebp),%eax
	movl	%eax,32(%edi)
	movw	%es,%ax
	movw	%ax,36(%edi)
	movw	%fs,%ax
	movw	%ax,38(%edi)
	movw	%gs,%ax
	movw	%ax,40(%edi)
	movw	%ss,%ax
	movw	%ax,42(%edi)
	movl	%fs:0,%eax
	movl	%eax,44(%edi)
	popl	%edi
	movl	$0,%eax
	leave
	ret

	.global	___ljfault
___ljfault:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edi

	movl	12(%ebp),%eax
	testl	%eax,%eax
	jne	0f
	incl	%eax

0:	movl	%eax,0(%edi)
	movl	24(%edi),%ebp
	pushfl
	popl	%ebx
	movl	44(%edi),%eax
	movl	%eax,%fs:0
	movw	42(%edi),%ax
	movw	%ax,%ss
	movl	28(%edi),%esp
	pushl	32(%edi)
	pushl	%ebx
	movw	36(%edi),%ax
	movw	%ax,%es
	movw	40(%edi),%ax
	movw	%ax,%gs
	movl	0(%edi),%eax
	movl	4(%edi),%ebx
	movl	8(%edi),%ecx
	movl	16(%edi),%esi
	movl	12(%edi),%edx
	movl	20(%edi),%edi
	popfl
	ret

	.globl	_siglongjmp
_siglongjmp:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edi			# &sigjmp_buf
	movl	208(%edi),%eax			# load savemask
	testl	%eax,%eax			# savemask != 0?
	je	1f				# no, skip restoring sigmask
	subl	$12,%esp
	leal	212(%edi),%eax			# &sigjmp_buf.sigmask
	movl	%eax,4(%esp)			# -> 2nd param "set"
	xorl	%eax,%eax
	movl	%eax,8(%esp)			# NULL -> 3rd param "oldset"
	movl	%eax,(%esp)			# SIG_SETMASK -> 1st param "how"
	call	_pthread_sigmask
	addl	$12,%esp
	jmp	1f

	.globl	_longjmp
_longjmp:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edi			# &jmp_buf
1:
	call	stabilize_sig_stack
	movl	52(%edi),%eax			# get old signal stack
	movl	%eax,-8876(%ebx)	# restore
	decl	-8880(%ebx)		# relinquish lock
	xorl	%eax,%eax
	movl	%eax,-8888(%ebx)		# we're not in cygwin anymore

	movl	12(%ebp),%eax
	testl	%eax,%eax
	jne	3f
	incl	%eax

3:	movl	%eax,0(%edi)
	movl	24(%edi),%ebp
	pushfl
	popl	%ebx
	fninit
	fldcw   48(%edi)
	movl	44(%edi),%eax
	movl	%eax,%fs:0
	movw	42(%edi),%ax
	movw	%ax,%ss
	movl	28(%edi),%esp
	pushl	32(%edi)
	pushl	%ebx
	movw	36(%edi),%ax
	movw	%ax,%es
	movw	40(%edi),%ax
	movw	%ax,%gs
	movl	0(%edi),%eax
	movl	4(%edi),%ebx
	movl	8(%edi),%ecx
	movl	16(%edi),%esi
	movl	12(%edi),%edx
	movl	20(%edi),%edi
	popfl
	ret
        .extern __Exit
        .global __sigfe__Exit
__sigfe__Exit:
        pushl   $__Exit
        jmp     __sigfe

        .extern ___chk_fail
        .global __sigfe___chk_fail
__sigfe___chk_fail:
        pushl   $___chk_fail
        jmp     __sigfe

        .extern ___cxa_finalize
        .global __sigfe___cxa_finalize
__sigfe___cxa_finalize:
        pushl   $___cxa_finalize
        jmp     __sigfe

        .extern ___dn_comp
        .global __sigfe___dn_comp
__sigfe___dn_comp:
        pushl   $___dn_comp
        jmp     __sigfe

        .extern ___dn_expand
        .global __sigfe___dn_expand
__sigfe___dn_expand:
        pushl   $___dn_expand
        jmp     __sigfe

        .extern ___dn_skipname
        .global __sigfe___dn_skipname
__sigfe___dn_skipname:
        pushl   $___dn_skipname
        jmp     __sigfe

        .extern ___eprintf
        .global __sigfe___eprintf
__sigfe___eprintf:
        pushl   $___eprintf
        jmp     __sigfe

        .extern ___fpurge
        .global __sigfe___fpurge
__sigfe___fpurge:
        pushl   $___fpurge
        jmp     __sigfe

        .extern ___fsetlocking
        .global __sigfe___fsetlocking
__sigfe___fsetlocking:
        pushl   $___fsetlocking
        jmp     __sigfe

        .extern ___getdelim
        .global __sigfe___getdelim
__sigfe___getdelim:
        pushl   $___getdelim
        jmp     __sigfe

        .extern ___getline
        .global __sigfe___getline
__sigfe___getline:
        pushl   $___getline
        jmp     __sigfe

        .extern ___gets_chk
        .global __sigfe___gets_chk
__sigfe___gets_chk:
        pushl   $___gets_chk
        jmp     __sigfe

        .extern ___opendir_with_d_ino
        .global __sigfe___opendir_with_d_ino
__sigfe___opendir_with_d_ino:
        pushl   $___opendir_with_d_ino
        jmp     __sigfe

        .extern ___res_close
        .global __sigfe___res_close
__sigfe___res_close:
        pushl   $___res_close
        jmp     __sigfe

        .extern ___res_init
        .global __sigfe___res_init
__sigfe___res_init:
        pushl   $___res_init
        jmp     __sigfe

        .extern ___res_mkquery
        .global __sigfe___res_mkquery
__sigfe___res_mkquery:
        pushl   $___res_mkquery
        jmp     __sigfe

        .extern ___res_nclose
        .global __sigfe___res_nclose
__sigfe___res_nclose:
        pushl   $___res_nclose
        jmp     __sigfe

        .extern ___res_ninit
        .global __sigfe___res_ninit
__sigfe___res_ninit:
        pushl   $___res_ninit
        jmp     __sigfe

        .extern ___res_nmkquery
        .global __sigfe___res_nmkquery
__sigfe___res_nmkquery:
        pushl   $___res_nmkquery
        jmp     __sigfe

        .extern ___res_nquery
        .global __sigfe___res_nquery
__sigfe___res_nquery:
        pushl   $___res_nquery
        jmp     __sigfe

        .extern ___res_nquerydomain
        .global __sigfe___res_nquerydomain
__sigfe___res_nquerydomain:
        pushl   $___res_nquerydomain
        jmp     __sigfe

        .extern ___res_nsearch
        .global __sigfe___res_nsearch
__sigfe___res_nsearch:
        pushl   $___res_nsearch
        jmp     __sigfe

        .extern ___res_nsend
        .global __sigfe___res_nsend
__sigfe___res_nsend:
        pushl   $___res_nsend
        jmp     __sigfe

        .extern ___res_query
        .global __sigfe___res_query
__sigfe___res_query:
        pushl   $___res_query
        jmp     __sigfe

        .extern ___res_querydomain
        .global __sigfe___res_querydomain
__sigfe___res_querydomain:
        pushl   $___res_querydomain
        jmp     __sigfe

        .extern ___res_search
        .global __sigfe___res_search
__sigfe___res_search:
        pushl   $___res_search
        jmp     __sigfe

        .extern ___res_send
        .global __sigfe___res_send
__sigfe___res_send:
        pushl   $___res_send
        jmp     __sigfe

        .extern ___res_state
        .global __sigfe___res_state
__sigfe___res_state:
        pushl   $___res_state
        jmp     __sigfe

        .extern ___sched_getaffinity_sys
        .global __sigfe___sched_getaffinity_sys
__sigfe___sched_getaffinity_sys:
        pushl   $___sched_getaffinity_sys
        jmp     __sigfe

        .extern ___snprintf_chk
        .global __sigfe___snprintf_chk
__sigfe___snprintf_chk:
        pushl   $___snprintf_chk
        jmp     __sigfe

        .extern ___sprintf_chk
        .global __sigfe___sprintf_chk
__sigfe___sprintf_chk:
        pushl   $___sprintf_chk
        jmp     __sigfe

        .extern ___srget
        .global __sigfe___srget
__sigfe___srget:
        pushl   $___srget
        jmp     __sigfe

        .extern ___srget_r
        .global __sigfe___srget_r
__sigfe___srget_r:
        pushl   $___srget_r
        jmp     __sigfe

        .extern ___stack_chk_fail
        .global __sigfe___stack_chk_fail
__sigfe___stack_chk_fail:
        pushl   $___stack_chk_fail
        jmp     __sigfe

        .extern ___swbuf
        .global __sigfe___swbuf
__sigfe___swbuf:
        pushl   $___swbuf
        jmp     __sigfe

        .extern ___swbuf_r
        .global __sigfe___swbuf_r
__sigfe___swbuf_r:
        pushl   $___swbuf_r
        jmp     __sigfe

        .extern ___vsnprintf_chk
        .global __sigfe___vsnprintf_chk
__sigfe___vsnprintf_chk:
        pushl   $___vsnprintf_chk
        jmp     __sigfe

        .extern ___vsprintf_chk
        .global __sigfe___vsprintf_chk
__sigfe___vsprintf_chk:
        pushl   $___vsprintf_chk
        jmp     __sigfe

        .extern ___xdrrec_getrec
        .global __sigfe___xdrrec_getrec
__sigfe___xdrrec_getrec:
        pushl   $___xdrrec_getrec
        jmp     __sigfe

        .extern ___xdrrec_setnonblock
        .global __sigfe___xdrrec_setnonblock
__sigfe___xdrrec_setnonblock:
        pushl   $___xdrrec_setnonblock
        jmp     __sigfe

        .extern ___xpg_sigpause
        .global __sigfe___xpg_sigpause
__sigfe___xpg_sigpause:
        pushl   $___xpg_sigpause
        jmp     __sigfe

        .extern ___xpg_strerror_r
        .global __sigfe___xpg_strerror_r
__sigfe___xpg_strerror_r:
        pushl   $___xpg_strerror_r
        jmp     __sigfe

        .extern __asprintf_r
        .global __sigfe__asprintf_r
__sigfe__asprintf_r:
        pushl   $__asprintf_r
        jmp     __sigfe

        .extern __exit
        .global __sigfe__exit
__sigfe__exit:
        pushl   $__exit
        jmp     __sigfe

        .extern __fcloseall_r
        .global __sigfe__fcloseall_r
__sigfe__fcloseall_r:
        pushl   $__fcloseall_r
        jmp     __sigfe

        .extern __fscanf_r
        .global __sigfe__fscanf_r
__sigfe__fscanf_r:
        pushl   $__fscanf_r
        jmp     __sigfe

        .extern __get_osfhandle
        .global __sigfe__get_osfhandle
__sigfe__get_osfhandle:
        pushl   $__get_osfhandle
        jmp     __sigfe

        .extern __open64
        .global __sigfe__open64
__sigfe__open64:
        pushl   $__open64
        jmp     __sigfe

        .extern __pipe
        .global __sigfe__pipe
__sigfe__pipe:
        pushl   $__pipe
        jmp     __sigfe

        .extern __pthread_cleanup_pop
        .global __sigfe__pthread_cleanup_pop
__sigfe__pthread_cleanup_pop:
        pushl   $__pthread_cleanup_pop
        jmp     __sigfe

        .extern __pthread_cleanup_push
        .global __sigfe__pthread_cleanup_push
__sigfe__pthread_cleanup_push:
        pushl   $__pthread_cleanup_push
        jmp     __sigfe

        .extern __scanf_r
        .global __sigfe__scanf_r
__sigfe__scanf_r:
        pushl   $__scanf_r
        jmp     __sigfe

        .extern __sscanf_r
        .global __sigfe__sscanf_r
__sigfe__sscanf_r:
        pushl   $__sscanf_r
        jmp     __sigfe

        .extern __vasprintf_r
        .global __sigfe__vasprintf_r
__sigfe__vasprintf_r:
        pushl   $__vasprintf_r
        jmp     __sigfe

        .extern __vfscanf_r
        .global __sigfe__vfscanf_r
__sigfe__vfscanf_r:
        pushl   $__vfscanf_r
        jmp     __sigfe

        .extern __vscanf_r
        .global __sigfe__vscanf_r
__sigfe__vscanf_r:
        pushl   $__vscanf_r
        jmp     __sigfe

        .extern __vsscanf_r
        .global __sigfe__vsscanf_r
__sigfe__vsscanf_r:
        pushl   $__vsscanf_r
        jmp     __sigfe

        .extern _accept4
        .global __sigfe_accept4
__sigfe_accept4:
        pushl   $_accept4
        jmp     __sigfe

        .extern _access
        .global __sigfe_access
__sigfe_access:
        pushl   $_access
        jmp     __sigfe

        .extern _acl
        .global __sigfe_acl
__sigfe_acl:
        pushl   $_acl
        jmp     __sigfe

        .extern _acl32
        .global __sigfe_acl32
__sigfe_acl32:
        pushl   $_acl32
        jmp     __sigfe

        .extern _acl_calc_mask
        .global __sigfe_acl_calc_mask
__sigfe_acl_calc_mask:
        pushl   $_acl_calc_mask
        jmp     __sigfe

        .extern _acl_cmp
        .global __sigfe_acl_cmp
__sigfe_acl_cmp:
        pushl   $_acl_cmp
        jmp     __sigfe

        .extern _acl_create_entry
        .global __sigfe_acl_create_entry
__sigfe_acl_create_entry:
        pushl   $_acl_create_entry
        jmp     __sigfe

        .extern _acl_delete_def_file
        .global __sigfe_acl_delete_def_file
__sigfe_acl_delete_def_file:
        pushl   $_acl_delete_def_file
        jmp     __sigfe

        .extern _acl_dup
        .global __sigfe_acl_dup
__sigfe_acl_dup:
        pushl   $_acl_dup
        jmp     __sigfe

        .extern _acl_equiv_mode
        .global __sigfe_acl_equiv_mode
__sigfe_acl_equiv_mode:
        pushl   $_acl_equiv_mode
        jmp     __sigfe

        .extern _acl_extended_fd
        .global __sigfe_acl_extended_fd
__sigfe_acl_extended_fd:
        pushl   $_acl_extended_fd
        jmp     __sigfe

        .extern _acl_extended_file
        .global __sigfe_acl_extended_file
__sigfe_acl_extended_file:
        pushl   $_acl_extended_file
        jmp     __sigfe

        .extern _acl_extended_file_nofollow
        .global __sigfe_acl_extended_file_nofollow
__sigfe_acl_extended_file_nofollow:
        pushl   $_acl_extended_file_nofollow
        jmp     __sigfe

        .extern _acl_free
        .global __sigfe_acl_free
__sigfe_acl_free:
        pushl   $_acl_free
        jmp     __sigfe

        .extern _acl_from_text
        .global __sigfe_acl_from_text
__sigfe_acl_from_text:
        pushl   $_acl_from_text
        jmp     __sigfe

        .extern _acl_get_fd
        .global __sigfe_acl_get_fd
__sigfe_acl_get_fd:
        pushl   $_acl_get_fd
        jmp     __sigfe

        .extern _acl_get_file
        .global __sigfe_acl_get_file
__sigfe_acl_get_file:
        pushl   $_acl_get_file
        jmp     __sigfe

        .extern _acl_get_qualifier
        .global __sigfe_acl_get_qualifier
__sigfe_acl_get_qualifier:
        pushl   $_acl_get_qualifier
        jmp     __sigfe

        .extern _acl_init
        .global __sigfe_acl_init
__sigfe_acl_init:
        pushl   $_acl_init
        jmp     __sigfe

        .extern _acl_set_fd
        .global __sigfe_acl_set_fd
__sigfe_acl_set_fd:
        pushl   $_acl_set_fd
        jmp     __sigfe

        .extern _acl_set_file
        .global __sigfe_acl_set_file
__sigfe_acl_set_file:
        pushl   $_acl_set_file
        jmp     __sigfe

        .extern _acl_to_any_text
        .global __sigfe_acl_to_any_text
__sigfe_acl_to_any_text:
        pushl   $_acl_to_any_text
        jmp     __sigfe

        .extern _acl_to_text
        .global __sigfe_acl_to_text
__sigfe_acl_to_text:
        pushl   $_acl_to_text
        jmp     __sigfe

        .extern _aclfrommode
        .global __sigfe_aclfrommode
__sigfe_aclfrommode:
        pushl   $_aclfrommode
        jmp     __sigfe

        .extern _aclfrommode32
        .global __sigfe_aclfrommode32
__sigfe_aclfrommode32:
        pushl   $_aclfrommode32
        jmp     __sigfe

        .extern _aclfrompbits
        .global __sigfe_aclfrompbits
__sigfe_aclfrompbits:
        pushl   $_aclfrompbits
        jmp     __sigfe

        .extern _aclfrompbits32
        .global __sigfe_aclfrompbits32
__sigfe_aclfrompbits32:
        pushl   $_aclfrompbits32
        jmp     __sigfe

        .extern _aclfromtext
        .global __sigfe_aclfromtext
__sigfe_aclfromtext:
        pushl   $_aclfromtext
        jmp     __sigfe

        .extern _aclfromtext32
        .global __sigfe_aclfromtext32
__sigfe_aclfromtext32:
        pushl   $_aclfromtext32
        jmp     __sigfe

        .extern _aclsort
        .global __sigfe_aclsort
__sigfe_aclsort:
        pushl   $_aclsort
        jmp     __sigfe

        .extern _aclsort32
        .global __sigfe_aclsort32
__sigfe_aclsort32:
        pushl   $_aclsort32
        jmp     __sigfe

        .extern _acltomode
        .global __sigfe_acltomode
__sigfe_acltomode:
        pushl   $_acltomode
        jmp     __sigfe

        .extern _acltomode32
        .global __sigfe_acltomode32
__sigfe_acltomode32:
        pushl   $_acltomode32
        jmp     __sigfe

        .extern _acltopbits
        .global __sigfe_acltopbits
__sigfe_acltopbits:
        pushl   $_acltopbits
        jmp     __sigfe

        .extern _acltopbits32
        .global __sigfe_acltopbits32
__sigfe_acltopbits32:
        pushl   $_acltopbits32
        jmp     __sigfe

        .extern _acltotext
        .global __sigfe_acltotext
__sigfe_acltotext:
        pushl   $_acltotext
        jmp     __sigfe

        .extern _acltotext32
        .global __sigfe_acltotext32
__sigfe_acltotext32:
        pushl   $_acltotext32
        jmp     __sigfe

        .extern _aio_cancel
        .global __sigfe_aio_cancel
__sigfe_aio_cancel:
        pushl   $_aio_cancel
        jmp     __sigfe

        .extern _aio_fsync
        .global __sigfe_aio_fsync
__sigfe_aio_fsync:
        pushl   $_aio_fsync
        jmp     __sigfe

        .extern _aio_read
        .global __sigfe_aio_read
__sigfe_aio_read:
        pushl   $_aio_read
        jmp     __sigfe

        .extern _aio_suspend
        .global __sigfe_aio_suspend
__sigfe_aio_suspend:
        pushl   $_aio_suspend
        jmp     __sigfe

        .extern _aio_write
        .global __sigfe_aio_write
__sigfe_aio_write:
        pushl   $_aio_write
        jmp     __sigfe

        .extern _alarm
        .global __sigfe_alarm
__sigfe_alarm:
        pushl   $_alarm
        jmp     __sigfe

        .extern _aligned_alloc
        .global __sigfe_aligned_alloc
__sigfe_aligned_alloc:
        pushl   $_aligned_alloc
        jmp     __sigfe

        .extern _argz_add
        .global __sigfe_argz_add
__sigfe_argz_add:
        pushl   $_argz_add
        jmp     __sigfe

        .extern _argz_add_sep
        .global __sigfe_argz_add_sep
__sigfe_argz_add_sep:
        pushl   $_argz_add_sep
        jmp     __sigfe

        .extern _argz_append
        .global __sigfe_argz_append
__sigfe_argz_append:
        pushl   $_argz_append
        jmp     __sigfe

        .extern _argz_create
        .global __sigfe_argz_create
__sigfe_argz_create:
        pushl   $_argz_create
        jmp     __sigfe

        .extern _argz_create_sep
        .global __sigfe_argz_create_sep
__sigfe_argz_create_sep:
        pushl   $_argz_create_sep
        jmp     __sigfe

        .extern _argz_delete
        .global __sigfe_argz_delete
__sigfe_argz_delete:
        pushl   $_argz_delete
        jmp     __sigfe

        .extern _argz_insert
        .global __sigfe_argz_insert
__sigfe_argz_insert:
        pushl   $_argz_insert
        jmp     __sigfe

        .extern _argz_replace
        .global __sigfe_argz_replace
__sigfe_argz_replace:
        pushl   $_argz_replace
        jmp     __sigfe

        .extern _asctime
        .global __sigfe_asctime
__sigfe_asctime:
        pushl   $_asctime
        jmp     __sigfe

        .extern _asctime_r
        .global __sigfe_asctime_r
__sigfe_asctime_r:
        pushl   $_asctime_r
        jmp     __sigfe

        .extern _asnprintf
        .global __sigfe_asnprintf
__sigfe_asnprintf:
        pushl   $_asnprintf
        jmp     __sigfe

        .extern _asprintf
        .global __sigfe_asprintf
__sigfe_asprintf:
        pushl   $_asprintf
        jmp     __sigfe

        .extern _at_quick_exit
        .global __sigfe_at_quick_exit
__sigfe_at_quick_exit:
        pushl   $_at_quick_exit
        jmp     __sigfe

        .extern _atof
        .global __sigfe_atof
__sigfe_atof:
        pushl   $_atof
        jmp     __sigfe

        .extern _atoff
        .global __sigfe_atoff
__sigfe_atoff:
        pushl   $_atoff
        jmp     __sigfe

        .extern _call_once
        .global __sigfe_call_once
__sigfe_call_once:
        pushl   $_call_once
        jmp     __sigfe

        .extern _calloc
        .global __sigfe_calloc
__sigfe_calloc:
        pushl   $_calloc
        jmp     __sigfe

        .extern _canonicalize_file_name
        .global __sigfe_canonicalize_file_name
__sigfe_canonicalize_file_name:
        pushl   $_canonicalize_file_name
        jmp     __sigfe

        .extern _catclose
        .global __sigfe_catclose
__sigfe_catclose:
        pushl   $_catclose
        jmp     __sigfe

        .extern _catgets
        .global __sigfe_catgets
__sigfe_catgets:
        pushl   $_catgets
        jmp     __sigfe

        .extern _catopen
        .global __sigfe_catopen
__sigfe_catopen:
        pushl   $_catopen
        jmp     __sigfe

        .extern _cfsetispeed
        .global __sigfe_cfsetispeed
__sigfe_cfsetispeed:
        pushl   $_cfsetispeed
        jmp     __sigfe

        .extern _cfsetospeed
        .global __sigfe_cfsetospeed
__sigfe_cfsetospeed:
        pushl   $_cfsetospeed
        jmp     __sigfe

        .extern _cfsetspeed
        .global __sigfe_cfsetspeed
__sigfe_cfsetspeed:
        pushl   $_cfsetspeed
        jmp     __sigfe

        .extern _chdir
        .global __sigfe_chdir
__sigfe_chdir:
        pushl   $_chdir
        jmp     __sigfe

        .extern _chmod
        .global __sigfe_chmod
__sigfe_chmod:
        pushl   $_chmod
        jmp     __sigfe

        .extern _chown
        .global __sigfe_chown
__sigfe_chown:
        pushl   $_chown
        jmp     __sigfe

        .extern _chown32
        .global __sigfe_chown32
__sigfe_chown32:
        pushl   $_chown32
        jmp     __sigfe

        .extern _chroot
        .global __sigfe_chroot
__sigfe_chroot:
        pushl   $_chroot
        jmp     __sigfe

        .extern _clearenv
        .global __sigfe_clearenv
__sigfe_clearenv:
        pushl   $_clearenv
        jmp     __sigfe

        .extern _clearerr
        .global __sigfe_clearerr
__sigfe_clearerr:
        pushl   $_clearerr
        jmp     __sigfe

        .extern _clearerr_unlocked
        .global __sigfe_clearerr_unlocked
__sigfe_clearerr_unlocked:
        pushl   $_clearerr_unlocked
        jmp     __sigfe

        .extern _clock
        .global __sigfe_clock
__sigfe_clock:
        pushl   $_clock
        jmp     __sigfe

        .extern _clock_getcpuclockid
        .global __sigfe_clock_getcpuclockid
__sigfe_clock_getcpuclockid:
        pushl   $_clock_getcpuclockid
        jmp     __sigfe

        .extern _clock_getres
        .global __sigfe_clock_getres
__sigfe_clock_getres:
        pushl   $_clock_getres
        jmp     __sigfe

        .extern _clock_gettime
        .global __sigfe_clock_gettime
__sigfe_clock_gettime:
        pushl   $_clock_gettime
        jmp     __sigfe

        .extern _clock_nanosleep
        .global __sigfe_clock_nanosleep
__sigfe_clock_nanosleep:
        pushl   $_clock_nanosleep
        jmp     __sigfe

        .extern _clock_setres
        .global __sigfe_clock_setres
__sigfe_clock_setres:
        pushl   $_clock_setres
        jmp     __sigfe

        .extern _clock_settime
        .global __sigfe_clock_settime
__sigfe_clock_settime:
        pushl   $_clock_settime
        jmp     __sigfe

        .extern _close
        .global __sigfe_close
__sigfe_close:
        pushl   $_close
        jmp     __sigfe

        .extern _closedir
        .global __sigfe_closedir
__sigfe_closedir:
        pushl   $_closedir
        jmp     __sigfe

        .extern _closelog
        .global __sigfe_closelog
__sigfe_closelog:
        pushl   $_closelog
        jmp     __sigfe

        .extern _cnd_broadcast
        .global __sigfe_cnd_broadcast
__sigfe_cnd_broadcast:
        pushl   $_cnd_broadcast
        jmp     __sigfe

        .extern _cnd_destroy
        .global __sigfe_cnd_destroy
__sigfe_cnd_destroy:
        pushl   $_cnd_destroy
        jmp     __sigfe

        .extern _cnd_init
        .global __sigfe_cnd_init
__sigfe_cnd_init:
        pushl   $_cnd_init
        jmp     __sigfe

        .extern _cnd_signal
        .global __sigfe_cnd_signal
__sigfe_cnd_signal:
        pushl   $_cnd_signal
        jmp     __sigfe

        .extern _cnd_timedwait
        .global __sigfe_cnd_timedwait
__sigfe_cnd_timedwait:
        pushl   $_cnd_timedwait
        jmp     __sigfe

        .extern _cnd_wait
        .global __sigfe_cnd_wait
__sigfe_cnd_wait:
        pushl   $_cnd_wait
        jmp     __sigfe

        .extern _creat
        .global __sigfe_creat
__sigfe_creat:
        pushl   $_creat
        jmp     __sigfe

        .extern _ctermid
        .global __sigfe_ctermid
__sigfe_ctermid:
        pushl   $_ctermid
        jmp     __sigfe

        .extern _ctime
        .global __sigfe_ctime
__sigfe_ctime:
        pushl   $_ctime
        jmp     __sigfe

        .extern _ctime_r
        .global __sigfe_ctime_r
__sigfe_ctime_r:
        pushl   $_ctime_r
        jmp     __sigfe

        .extern _cwait
        .global __sigfe_cwait
__sigfe_cwait:
        pushl   $_cwait
        jmp     __sigfe

        .extern _cygwin__cxa_atexit
        .global __sigfe_cygwin__cxa_atexit
__sigfe_cygwin__cxa_atexit:
        pushl   $_cygwin__cxa_atexit
        jmp     __sigfe

        .extern _cygwin_accept
        .global __sigfe_cygwin_accept
__sigfe_cygwin_accept:
        pushl   $_cygwin_accept
        jmp     __sigfe

        .extern _cygwin_atexit
        .global __sigfe_cygwin_atexit
__sigfe_cygwin_atexit:
        pushl   $_cygwin_atexit
        jmp     __sigfe

        .extern _cygwin_attach_handle_to_fd
        .global __sigfe_cygwin_attach_handle_to_fd
__sigfe_cygwin_attach_handle_to_fd:
        pushl   $_cygwin_attach_handle_to_fd
        jmp     __sigfe

        .extern _cygwin_bind
        .global __sigfe_cygwin_bind
__sigfe_cygwin_bind:
        pushl   $_cygwin_bind
        jmp     __sigfe

        .extern _cygwin_bindresvport
        .global __sigfe_cygwin_bindresvport
__sigfe_cygwin_bindresvport:
        pushl   $_cygwin_bindresvport
        jmp     __sigfe

        .extern _cygwin_bindresvport_sa
        .global __sigfe_cygwin_bindresvport_sa
__sigfe_cygwin_bindresvport_sa:
        pushl   $_cygwin_bindresvport_sa
        jmp     __sigfe

        .extern _cygwin_connect
        .global __sigfe_cygwin_connect
__sigfe_cygwin_connect:
        pushl   $_cygwin_connect
        jmp     __sigfe

        .extern _cygwin_conv_path
        .global __sigfe_cygwin_conv_path
__sigfe_cygwin_conv_path:
        pushl   $_cygwin_conv_path
        jmp     __sigfe

        .extern _cygwin_conv_path_list
        .global __sigfe_cygwin_conv_path_list
__sigfe_cygwin_conv_path_list:
        pushl   $_cygwin_conv_path_list
        jmp     __sigfe

        .extern _cygwin_conv_to_full_posix_path
        .global __sigfe_cygwin_conv_to_full_posix_path
__sigfe_cygwin_conv_to_full_posix_path:
        pushl   $_cygwin_conv_to_full_posix_path
        jmp     __sigfe

        .extern _cygwin_conv_to_full_win32_path
        .global __sigfe_cygwin_conv_to_full_win32_path
__sigfe_cygwin_conv_to_full_win32_path:
        pushl   $_cygwin_conv_to_full_win32_path
        jmp     __sigfe

        .extern _cygwin_conv_to_posix_path
        .global __sigfe_cygwin_conv_to_posix_path
__sigfe_cygwin_conv_to_posix_path:
        pushl   $_cygwin_conv_to_posix_path
        jmp     __sigfe

        .extern _cygwin_conv_to_win32_path
        .global __sigfe_cygwin_conv_to_win32_path
__sigfe_cygwin_conv_to_win32_path:
        pushl   $_cygwin_conv_to_win32_path
        jmp     __sigfe

        .extern _cygwin_create_path
        .global __sigfe_cygwin_create_path
__sigfe_cygwin_create_path:
        pushl   $_cygwin_create_path
        jmp     __sigfe

        .extern _cygwin_detach_dll
        .global __sigfe_maybe_cygwin_detach_dll
__sigfe_maybe_cygwin_detach_dll:
        pushl   $_cygwin_detach_dll
        jmp     __sigfe_maybe

        .extern _cygwin_endprotoent
        .global __sigfe_cygwin_endprotoent
__sigfe_cygwin_endprotoent:
        pushl   $_cygwin_endprotoent
        jmp     __sigfe

        .extern _cygwin_endservent
        .global __sigfe_cygwin_endservent
__sigfe_cygwin_endservent:
        pushl   $_cygwin_endservent
        jmp     __sigfe

        .extern _cygwin_exit
        .global __sigfe_cygwin_exit
__sigfe_cygwin_exit:
        pushl   $_cygwin_exit
        jmp     __sigfe

        .extern _cygwin_freeaddrinfo
        .global __sigfe_cygwin_freeaddrinfo
__sigfe_cygwin_freeaddrinfo:
        pushl   $_cygwin_freeaddrinfo
        jmp     __sigfe

        .extern _cygwin_getaddrinfo
        .global __sigfe_cygwin_getaddrinfo
__sigfe_cygwin_getaddrinfo:
        pushl   $_cygwin_getaddrinfo
        jmp     __sigfe

        .extern _cygwin_gethostbyaddr
        .global __sigfe_cygwin_gethostbyaddr
__sigfe_cygwin_gethostbyaddr:
        pushl   $_cygwin_gethostbyaddr
        jmp     __sigfe

        .extern _cygwin_gethostbyname
        .global __sigfe_cygwin_gethostbyname
__sigfe_cygwin_gethostbyname:
        pushl   $_cygwin_gethostbyname
        jmp     __sigfe

        .extern _cygwin_gethostname
        .global __sigfe_cygwin_gethostname
__sigfe_cygwin_gethostname:
        pushl   $_cygwin_gethostname
        jmp     __sigfe

        .extern _cygwin_getnameinfo
        .global __sigfe_cygwin_getnameinfo
__sigfe_cygwin_getnameinfo:
        pushl   $_cygwin_getnameinfo
        jmp     __sigfe

        .extern _cygwin_getpeername
        .global __sigfe_cygwin_getpeername
__sigfe_cygwin_getpeername:
        pushl   $_cygwin_getpeername
        jmp     __sigfe

        .extern _cygwin_getprotobyname
        .global __sigfe_cygwin_getprotobyname
__sigfe_cygwin_getprotobyname:
        pushl   $_cygwin_getprotobyname
        jmp     __sigfe

        .extern _cygwin_getprotobynumber
        .global __sigfe_cygwin_getprotobynumber
__sigfe_cygwin_getprotobynumber:
        pushl   $_cygwin_getprotobynumber
        jmp     __sigfe

        .extern _cygwin_getprotoent
        .global __sigfe_cygwin_getprotoent
__sigfe_cygwin_getprotoent:
        pushl   $_cygwin_getprotoent
        jmp     __sigfe

        .extern _cygwin_getservbyname
        .global __sigfe_cygwin_getservbyname
__sigfe_cygwin_getservbyname:
        pushl   $_cygwin_getservbyname
        jmp     __sigfe

        .extern _cygwin_getservbyport
        .global __sigfe_cygwin_getservbyport
__sigfe_cygwin_getservbyport:
        pushl   $_cygwin_getservbyport
        jmp     __sigfe

        .extern _cygwin_getservent
        .global __sigfe_cygwin_getservent
__sigfe_cygwin_getservent:
        pushl   $_cygwin_getservent
        jmp     __sigfe

        .extern _cygwin_getsockname
        .global __sigfe_cygwin_getsockname
__sigfe_cygwin_getsockname:
        pushl   $_cygwin_getsockname
        jmp     __sigfe

        .extern _cygwin_getsockopt
        .global __sigfe_cygwin_getsockopt
__sigfe_cygwin_getsockopt:
        pushl   $_cygwin_getsockopt
        jmp     __sigfe

        .extern _cygwin_herror
        .global __sigfe_cygwin_herror
__sigfe_cygwin_herror:
        pushl   $_cygwin_herror
        jmp     __sigfe

        .extern _cygwin_if_indextoname
        .global __sigfe_cygwin_if_indextoname
__sigfe_cygwin_if_indextoname:
        pushl   $_cygwin_if_indextoname
        jmp     __sigfe

        .extern _cygwin_if_nametoindex
        .global __sigfe_cygwin_if_nametoindex
__sigfe_cygwin_if_nametoindex:
        pushl   $_cygwin_if_nametoindex
        jmp     __sigfe

        .extern _cygwin_inet_addr
        .global __sigfe_cygwin_inet_addr
__sigfe_cygwin_inet_addr:
        pushl   $_cygwin_inet_addr
        jmp     __sigfe

        .extern _cygwin_inet_aton
        .global __sigfe_cygwin_inet_aton
__sigfe_cygwin_inet_aton:
        pushl   $_cygwin_inet_aton
        jmp     __sigfe

        .extern _cygwin_inet_network
        .global __sigfe_cygwin_inet_network
__sigfe_cygwin_inet_network:
        pushl   $_cygwin_inet_network
        jmp     __sigfe

        .extern _cygwin_inet_ntoa
        .global __sigfe_cygwin_inet_ntoa
__sigfe_cygwin_inet_ntoa:
        pushl   $_cygwin_inet_ntoa
        jmp     __sigfe

        .extern _cygwin_inet_ntop
        .global __sigfe_cygwin_inet_ntop
__sigfe_cygwin_inet_ntop:
        pushl   $_cygwin_inet_ntop
        jmp     __sigfe

        .extern _cygwin_inet_pton
        .global __sigfe_cygwin_inet_pton
__sigfe_cygwin_inet_pton:
        pushl   $_cygwin_inet_pton
        jmp     __sigfe

        .extern _cygwin_internal
        .global __sigfe_cygwin_internal
__sigfe_cygwin_internal:
        pushl   $_cygwin_internal
        jmp     __sigfe

        .extern _cygwin_listen
        .global __sigfe_cygwin_listen
__sigfe_cygwin_listen:
        pushl   $_cygwin_listen
        jmp     __sigfe

        .extern _cygwin_logon_user
        .global __sigfe_cygwin_logon_user
__sigfe_cygwin_logon_user:
        pushl   $_cygwin_logon_user
        jmp     __sigfe

        .extern _cygwin_posix_to_win32_path_list
        .global __sigfe_cygwin_posix_to_win32_path_list
__sigfe_cygwin_posix_to_win32_path_list:
        pushl   $_cygwin_posix_to_win32_path_list
        jmp     __sigfe

        .extern _cygwin_posix_to_win32_path_list_buf_size
        .global __sigfe_cygwin_posix_to_win32_path_list_buf_size
__sigfe_cygwin_posix_to_win32_path_list_buf_size:
        pushl   $_cygwin_posix_to_win32_path_list_buf_size
        jmp     __sigfe

        .extern _cygwin_rcmd
        .global __sigfe_cygwin_rcmd
__sigfe_cygwin_rcmd:
        pushl   $_cygwin_rcmd
        jmp     __sigfe

        .extern _cygwin_rcmd_af
        .global __sigfe_cygwin_rcmd_af
__sigfe_cygwin_rcmd_af:
        pushl   $_cygwin_rcmd_af
        jmp     __sigfe

        .extern _cygwin_recv
        .global __sigfe_cygwin_recv
__sigfe_cygwin_recv:
        pushl   $_cygwin_recv
        jmp     __sigfe

        .extern _cygwin_recvfrom
        .global __sigfe_cygwin_recvfrom
__sigfe_cygwin_recvfrom:
        pushl   $_cygwin_recvfrom
        jmp     __sigfe

        .extern _cygwin_recvmsg
        .global __sigfe_cygwin_recvmsg
__sigfe_cygwin_recvmsg:
        pushl   $_cygwin_recvmsg
        jmp     __sigfe

        .extern _cygwin_rexec
        .global __sigfe_cygwin_rexec
__sigfe_cygwin_rexec:
        pushl   $_cygwin_rexec
        jmp     __sigfe

        .extern _cygwin_rresvport
        .global __sigfe_cygwin_rresvport
__sigfe_cygwin_rresvport:
        pushl   $_cygwin_rresvport
        jmp     __sigfe

        .extern _cygwin_rresvport_af
        .global __sigfe_cygwin_rresvport_af
__sigfe_cygwin_rresvport_af:
        pushl   $_cygwin_rresvport_af
        jmp     __sigfe

        .extern _cygwin_select
        .global __sigfe_cygwin_select
__sigfe_cygwin_select:
        pushl   $_cygwin_select
        jmp     __sigfe

        .extern _cygwin_send
        .global __sigfe_cygwin_send
__sigfe_cygwin_send:
        pushl   $_cygwin_send
        jmp     __sigfe

        .extern _cygwin_sendmsg
        .global __sigfe_cygwin_sendmsg
__sigfe_cygwin_sendmsg:
        pushl   $_cygwin_sendmsg
        jmp     __sigfe

        .extern _cygwin_sendto
        .global __sigfe_cygwin_sendto
__sigfe_cygwin_sendto:
        pushl   $_cygwin_sendto
        jmp     __sigfe

        .extern _cygwin_set_impersonation_token
        .global __sigfe_cygwin_set_impersonation_token
__sigfe_cygwin_set_impersonation_token:
        pushl   $_cygwin_set_impersonation_token
        jmp     __sigfe

        .extern _cygwin_setmode
        .global __sigfe_cygwin_setmode
__sigfe_cygwin_setmode:
        pushl   $_cygwin_setmode
        jmp     __sigfe

        .extern _cygwin_setprotoent
        .global __sigfe_cygwin_setprotoent
__sigfe_cygwin_setprotoent:
        pushl   $_cygwin_setprotoent
        jmp     __sigfe

        .extern _cygwin_setservent
        .global __sigfe_cygwin_setservent
__sigfe_cygwin_setservent:
        pushl   $_cygwin_setservent
        jmp     __sigfe

        .extern _cygwin_setsockopt
        .global __sigfe_cygwin_setsockopt
__sigfe_cygwin_setsockopt:
        pushl   $_cygwin_setsockopt
        jmp     __sigfe

        .extern _cygwin_shutdown
        .global __sigfe_cygwin_shutdown
__sigfe_cygwin_shutdown:
        pushl   $_cygwin_shutdown
        jmp     __sigfe

        .extern _cygwin_socket
        .global __sigfe_cygwin_socket
__sigfe_cygwin_socket:
        pushl   $_cygwin_socket
        jmp     __sigfe

        .extern _cygwin_stackdump
        .global __sigfe_cygwin_stackdump
__sigfe_cygwin_stackdump:
        pushl   $_cygwin_stackdump
        jmp     __sigfe

        .extern _cygwin_umount
        .global __sigfe_cygwin_umount
__sigfe_cygwin_umount:
        pushl   $_cygwin_umount
        jmp     __sigfe

        .extern _cygwin_win32_to_posix_path_list
        .global __sigfe_cygwin_win32_to_posix_path_list
__sigfe_cygwin_win32_to_posix_path_list:
        pushl   $_cygwin_win32_to_posix_path_list
        jmp     __sigfe

        .extern _cygwin_win32_to_posix_path_list_buf_size
        .global __sigfe_cygwin_win32_to_posix_path_list_buf_size
__sigfe_cygwin_win32_to_posix_path_list_buf_size:
        pushl   $_cygwin_win32_to_posix_path_list_buf_size
        jmp     __sigfe

        .extern _cygwin_winpid_to_pid
        .global __sigfe_cygwin_winpid_to_pid
__sigfe_cygwin_winpid_to_pid:
        pushl   $_cygwin_winpid_to_pid
        jmp     __sigfe

        .extern _daemon
        .global __sigfe_daemon
__sigfe_daemon:
        pushl   $_daemon
        jmp     __sigfe

        .extern _dbm_clearerr
        .global __sigfe_dbm_clearerr
__sigfe_dbm_clearerr:
        pushl   $_dbm_clearerr
        jmp     __sigfe

        .extern _dbm_close
        .global __sigfe_dbm_close
__sigfe_dbm_close:
        pushl   $_dbm_close
        jmp     __sigfe

        .extern _dbm_delete
        .global __sigfe_dbm_delete
__sigfe_dbm_delete:
        pushl   $_dbm_delete
        jmp     __sigfe

        .extern _dbm_dirfno
        .global __sigfe_dbm_dirfno
__sigfe_dbm_dirfno:
        pushl   $_dbm_dirfno
        jmp     __sigfe

        .extern _dbm_error
        .global __sigfe_dbm_error
__sigfe_dbm_error:
        pushl   $_dbm_error
        jmp     __sigfe

        .extern _dbm_fetch
        .global __sigfe_dbm_fetch
__sigfe_dbm_fetch:
        pushl   $_dbm_fetch
        jmp     __sigfe

        .extern _dbm_firstkey
        .global __sigfe_dbm_firstkey
__sigfe_dbm_firstkey:
        pushl   $_dbm_firstkey
        jmp     __sigfe

        .extern _dbm_nextkey
        .global __sigfe_dbm_nextkey
__sigfe_dbm_nextkey:
        pushl   $_dbm_nextkey
        jmp     __sigfe

        .extern _dbm_open
        .global __sigfe_dbm_open
__sigfe_dbm_open:
        pushl   $_dbm_open
        jmp     __sigfe

        .extern _dbm_store
        .global __sigfe_dbm_store
__sigfe_dbm_store:
        pushl   $_dbm_store
        jmp     __sigfe

        .extern _dirfd
        .global __sigfe_dirfd
__sigfe_dirfd:
        pushl   $_dirfd
        jmp     __sigfe

        .extern _dladdr
        .global __sigfe_dladdr
__sigfe_dladdr:
        pushl   $_dladdr
        jmp     __sigfe

        .extern _dlclose
        .global __sigfe_dlclose
__sigfe_dlclose:
        pushl   $_dlclose
        jmp     __sigfe

        .extern _dlopen
        .global __sigfe_dlopen
__sigfe_dlopen:
        pushl   $_dlopen
        jmp     __sigfe

        .extern _dlsym
        .global __sigfe_dlsym
__sigfe_dlsym:
        pushl   $_dlsym
        jmp     __sigfe

        .extern _dprintf
        .global __sigfe_dprintf
__sigfe_dprintf:
        pushl   $_dprintf
        jmp     __sigfe

        .extern _dup
        .global __sigfe_dup
__sigfe_dup:
        pushl   $_dup
        jmp     __sigfe

        .extern _dup2
        .global __sigfe_dup2
__sigfe_dup2:
        pushl   $_dup2
        jmp     __sigfe

        .extern _dup3
        .global __sigfe_dup3
__sigfe_dup3:
        pushl   $_dup3
        jmp     __sigfe

        .extern _duplocale
        .global __sigfe_duplocale
__sigfe_duplocale:
        pushl   $_duplocale
        jmp     __sigfe

        .extern _ecvt
        .global __sigfe_ecvt
__sigfe_ecvt:
        pushl   $_ecvt
        jmp     __sigfe

        .extern _ecvtbuf
        .global __sigfe_ecvtbuf
__sigfe_ecvtbuf:
        pushl   $_ecvtbuf
        jmp     __sigfe

        .extern _ecvtf
        .global __sigfe_ecvtf
__sigfe_ecvtf:
        pushl   $_ecvtf
        jmp     __sigfe

        .extern _endusershell
        .global __sigfe_endusershell
__sigfe_endusershell:
        pushl   $_endusershell
        jmp     __sigfe

        .extern _endutent
        .global __sigfe_endutent
__sigfe_endutent:
        pushl   $_endutent
        jmp     __sigfe

        .extern _endutxent
        .global __sigfe_endutxent
__sigfe_endutxent:
        pushl   $_endutxent
        jmp     __sigfe

        .extern _envz_add
        .global __sigfe_envz_add
__sigfe_envz_add:
        pushl   $_envz_add
        jmp     __sigfe

        .extern _envz_merge
        .global __sigfe_envz_merge
__sigfe_envz_merge:
        pushl   $_envz_merge
        jmp     __sigfe

        .extern _envz_remove
        .global __sigfe_envz_remove
__sigfe_envz_remove:
        pushl   $_envz_remove
        jmp     __sigfe

        .extern _envz_strip
        .global __sigfe_envz_strip
__sigfe_envz_strip:
        pushl   $_envz_strip
        jmp     __sigfe

        .extern _err
        .global __sigfe_err
__sigfe_err:
        pushl   $_err
        jmp     __sigfe

        .extern _error
        .global __sigfe_error
__sigfe_error:
        pushl   $_error
        jmp     __sigfe

        .extern _error_at_line
        .global __sigfe_error_at_line
__sigfe_error_at_line:
        pushl   $_error_at_line
        jmp     __sigfe

        .extern _errx
        .global __sigfe_errx
__sigfe_errx:
        pushl   $_errx
        jmp     __sigfe

        .extern _euidaccess
        .global __sigfe_euidaccess
__sigfe_euidaccess:
        pushl   $_euidaccess
        jmp     __sigfe

        .extern _execl
        .global __sigfe_execl
__sigfe_execl:
        pushl   $_execl
        jmp     __sigfe

        .extern _execle
        .global __sigfe_execle
__sigfe_execle:
        pushl   $_execle
        jmp     __sigfe

        .extern _execlp
        .global __sigfe_execlp
__sigfe_execlp:
        pushl   $_execlp
        jmp     __sigfe

        .extern _execv
        .global __sigfe_execv
__sigfe_execv:
        pushl   $_execv
        jmp     __sigfe

        .extern _execve
        .global __sigfe_execve
__sigfe_execve:
        pushl   $_execve
        jmp     __sigfe

        .extern _execvp
        .global __sigfe_execvp
__sigfe_execvp:
        pushl   $_execvp
        jmp     __sigfe

        .extern _execvpe
        .global __sigfe_execvpe
__sigfe_execvpe:
        pushl   $_execvpe
        jmp     __sigfe

        .extern _faccessat
        .global __sigfe_faccessat
__sigfe_faccessat:
        pushl   $_faccessat
        jmp     __sigfe

        .extern _facl
        .global __sigfe_facl
__sigfe_facl:
        pushl   $_facl
        jmp     __sigfe

        .extern _facl32
        .global __sigfe_facl32
__sigfe_facl32:
        pushl   $_facl32
        jmp     __sigfe

        .extern _fchdir
        .global __sigfe_fchdir
__sigfe_fchdir:
        pushl   $_fchdir
        jmp     __sigfe

        .extern _fchmod
        .global __sigfe_fchmod
__sigfe_fchmod:
        pushl   $_fchmod
        jmp     __sigfe

        .extern _fchmodat
        .global __sigfe_fchmodat
__sigfe_fchmodat:
        pushl   $_fchmodat
        jmp     __sigfe

        .extern _fchown
        .global __sigfe_fchown
__sigfe_fchown:
        pushl   $_fchown
        jmp     __sigfe

        .extern _fchown32
        .global __sigfe_fchown32
__sigfe_fchown32:
        pushl   $_fchown32
        jmp     __sigfe

        .extern _fchownat
        .global __sigfe_fchownat
__sigfe_fchownat:
        pushl   $_fchownat
        jmp     __sigfe

        .extern _fclose
        .global __sigfe_fclose
__sigfe_fclose:
        pushl   $_fclose
        jmp     __sigfe

        .extern _fcloseall
        .global __sigfe_fcloseall
__sigfe_fcloseall:
        pushl   $_fcloseall
        jmp     __sigfe

        .extern _fcntl
        .global __sigfe_fcntl
__sigfe_fcntl:
        pushl   $_fcntl
        jmp     __sigfe

        .extern _fcntl64
        .global __sigfe_fcntl64
__sigfe_fcntl64:
        pushl   $_fcntl64
        jmp     __sigfe

        .extern _fcvt
        .global __sigfe_fcvt
__sigfe_fcvt:
        pushl   $_fcvt
        jmp     __sigfe

        .extern _fcvtbuf
        .global __sigfe_fcvtbuf
__sigfe_fcvtbuf:
        pushl   $_fcvtbuf
        jmp     __sigfe

        .extern _fcvtf
        .global __sigfe_fcvtf
__sigfe_fcvtf:
        pushl   $_fcvtf
        jmp     __sigfe

        .extern _fdatasync
        .global __sigfe_fdatasync
__sigfe_fdatasync:
        pushl   $_fdatasync
        jmp     __sigfe

        .extern _fdopen
        .global __sigfe_fdopen
__sigfe_fdopen:
        pushl   $_fdopen
        jmp     __sigfe

        .extern _fdopen64
        .global __sigfe_fdopen64
__sigfe_fdopen64:
        pushl   $_fdopen64
        jmp     __sigfe

        .extern _fdopendir
        .global __sigfe_fdopendir
__sigfe_fdopendir:
        pushl   $_fdopendir
        jmp     __sigfe

        .extern _feenableexcept
        .global __sigfe_feenableexcept
__sigfe_feenableexcept:
        pushl   $_feenableexcept
        jmp     __sigfe

        .extern _feholdexcept
        .global __sigfe_feholdexcept
__sigfe_feholdexcept:
        pushl   $_feholdexcept
        jmp     __sigfe

        .extern _feof
        .global __sigfe_feof
__sigfe_feof:
        pushl   $_feof
        jmp     __sigfe

        .extern _feof_unlocked
        .global __sigfe_feof_unlocked
__sigfe_feof_unlocked:
        pushl   $_feof_unlocked
        jmp     __sigfe

        .extern _feraiseexcept
        .global __sigfe_feraiseexcept
__sigfe_feraiseexcept:
        pushl   $_feraiseexcept
        jmp     __sigfe

        .extern _ferror
        .global __sigfe_ferror
__sigfe_ferror:
        pushl   $_ferror
        jmp     __sigfe

        .extern _ferror_unlocked
        .global __sigfe_ferror_unlocked
__sigfe_ferror_unlocked:
        pushl   $_ferror_unlocked
        jmp     __sigfe

        .extern _fesetenv
        .global __sigfe_fesetenv
__sigfe_fesetenv:
        pushl   $_fesetenv
        jmp     __sigfe

        .extern _fesetexceptflag
        .global __sigfe_fesetexceptflag
__sigfe_fesetexceptflag:
        pushl   $_fesetexceptflag
        jmp     __sigfe

        .extern _feupdateenv
        .global __sigfe_feupdateenv
__sigfe_feupdateenv:
        pushl   $_feupdateenv
        jmp     __sigfe

        .extern _fexecve
        .global __sigfe_fexecve
__sigfe_fexecve:
        pushl   $_fexecve
        jmp     __sigfe

        .extern _fflush
        .global __sigfe_fflush
__sigfe_fflush:
        pushl   $_fflush
        jmp     __sigfe

        .extern _fflush_unlocked
        .global __sigfe_fflush_unlocked
__sigfe_fflush_unlocked:
        pushl   $_fflush_unlocked
        jmp     __sigfe

        .extern _fgetc
        .global __sigfe_fgetc
__sigfe_fgetc:
        pushl   $_fgetc
        jmp     __sigfe

        .extern _fgetc_unlocked
        .global __sigfe_fgetc_unlocked
__sigfe_fgetc_unlocked:
        pushl   $_fgetc_unlocked
        jmp     __sigfe

        .extern _fgetpos
        .global __sigfe_fgetpos
__sigfe_fgetpos:
        pushl   $_fgetpos
        jmp     __sigfe

        .extern _fgetpos64
        .global __sigfe_fgetpos64
__sigfe_fgetpos64:
        pushl   $_fgetpos64
        jmp     __sigfe

        .extern _fgets
        .global __sigfe_fgets
__sigfe_fgets:
        pushl   $_fgets
        jmp     __sigfe

        .extern _fgets_unlocked
        .global __sigfe_fgets_unlocked
__sigfe_fgets_unlocked:
        pushl   $_fgets_unlocked
        jmp     __sigfe

        .extern _fgetwc
        .global __sigfe_fgetwc
__sigfe_fgetwc:
        pushl   $_fgetwc
        jmp     __sigfe

        .extern _fgetwc_unlocked
        .global __sigfe_fgetwc_unlocked
__sigfe_fgetwc_unlocked:
        pushl   $_fgetwc_unlocked
        jmp     __sigfe

        .extern _fgetws
        .global __sigfe_fgetws
__sigfe_fgetws:
        pushl   $_fgetws
        jmp     __sigfe

        .extern _fgetws_unlocked
        .global __sigfe_fgetws_unlocked
__sigfe_fgetws_unlocked:
        pushl   $_fgetws_unlocked
        jmp     __sigfe

        .extern _fgetxattr
        .global __sigfe_fgetxattr
__sigfe_fgetxattr:
        pushl   $_fgetxattr
        jmp     __sigfe

        .extern _fileno
        .global __sigfe_fileno
__sigfe_fileno:
        pushl   $_fileno
        jmp     __sigfe

        .extern _fileno_unlocked
        .global __sigfe_fileno_unlocked
__sigfe_fileno_unlocked:
        pushl   $_fileno_unlocked
        jmp     __sigfe

        .extern _fiprintf
        .global __sigfe_fiprintf
__sigfe_fiprintf:
        pushl   $_fiprintf
        jmp     __sigfe

        .extern _flistxattr
        .global __sigfe_flistxattr
__sigfe_flistxattr:
        pushl   $_flistxattr
        jmp     __sigfe

        .extern _flock
        .global __sigfe_flock
__sigfe_flock:
        pushl   $_flock
        jmp     __sigfe

        .extern _flockfile
        .global __sigfe_flockfile
__sigfe_flockfile:
        pushl   $_flockfile
        jmp     __sigfe

        .extern _fmemopen
        .global __sigfe_fmemopen
__sigfe_fmemopen:
        pushl   $_fmemopen
        jmp     __sigfe

        .extern _fopen
        .global __sigfe_fopen
__sigfe_fopen:
        pushl   $_fopen
        jmp     __sigfe

        .extern _fopen64
        .global __sigfe_fopen64
__sigfe_fopen64:
        pushl   $_fopen64
        jmp     __sigfe

        .extern _fopencookie
        .global __sigfe_fopencookie
__sigfe_fopencookie:
        pushl   $_fopencookie
        jmp     __sigfe

        .extern _fork
        .global __sigfe_fork
__sigfe_fork:
        pushl   $_fork
        jmp     __sigfe

        .extern _forkpty
        .global __sigfe_forkpty
__sigfe_forkpty:
        pushl   $_forkpty
        jmp     __sigfe

        .extern _fpathconf
        .global __sigfe_fpathconf
__sigfe_fpathconf:
        pushl   $_fpathconf
        jmp     __sigfe

        .extern _fprintf
        .global __sigfe_fprintf
__sigfe_fprintf:
        pushl   $_fprintf
        jmp     __sigfe

        .extern _fpurge
        .global __sigfe_fpurge
__sigfe_fpurge:
        pushl   $_fpurge
        jmp     __sigfe

        .extern _fputc
        .global __sigfe_fputc
__sigfe_fputc:
        pushl   $_fputc
        jmp     __sigfe

        .extern _fputc_unlocked
        .global __sigfe_fputc_unlocked
__sigfe_fputc_unlocked:
        pushl   $_fputc_unlocked
        jmp     __sigfe

        .extern _fputs
        .global __sigfe_fputs
__sigfe_fputs:
        pushl   $_fputs
        jmp     __sigfe

        .extern _fputs_unlocked
        .global __sigfe_fputs_unlocked
__sigfe_fputs_unlocked:
        pushl   $_fputs_unlocked
        jmp     __sigfe

        .extern _fputwc
        .global __sigfe_fputwc
__sigfe_fputwc:
        pushl   $_fputwc
        jmp     __sigfe

        .extern _fputwc_unlocked
        .global __sigfe_fputwc_unlocked
__sigfe_fputwc_unlocked:
        pushl   $_fputwc_unlocked
        jmp     __sigfe

        .extern _fputws
        .global __sigfe_fputws
__sigfe_fputws:
        pushl   $_fputws
        jmp     __sigfe

        .extern _fputws_unlocked
        .global __sigfe_fputws_unlocked
__sigfe_fputws_unlocked:
        pushl   $_fputws_unlocked
        jmp     __sigfe

        .extern _fread
        .global __sigfe_fread
__sigfe_fread:
        pushl   $_fread
        jmp     __sigfe

        .extern _fread_unlocked
        .global __sigfe_fread_unlocked
__sigfe_fread_unlocked:
        pushl   $_fread_unlocked
        jmp     __sigfe

        .extern _free
        .global __sigfe_free
__sigfe_free:
        pushl   $_free
        jmp     __sigfe

        .extern _freeifaddrs
        .global __sigfe_freeifaddrs
__sigfe_freeifaddrs:
        pushl   $_freeifaddrs
        jmp     __sigfe

        .extern _freelocale
        .global __sigfe_freelocale
__sigfe_freelocale:
        pushl   $_freelocale
        jmp     __sigfe

        .extern _fremovexattr
        .global __sigfe_fremovexattr
__sigfe_fremovexattr:
        pushl   $_fremovexattr
        jmp     __sigfe

        .extern _freopen
        .global __sigfe_freopen
__sigfe_freopen:
        pushl   $_freopen
        jmp     __sigfe

        .extern _freopen64
        .global __sigfe_freopen64
__sigfe_freopen64:
        pushl   $_freopen64
        jmp     __sigfe

        .extern _fscanf
        .global __sigfe_fscanf
__sigfe_fscanf:
        pushl   $_fscanf
        jmp     __sigfe

        .extern _fseek
        .global __sigfe_fseek
__sigfe_fseek:
        pushl   $_fseek
        jmp     __sigfe

        .extern _fseeko
        .global __sigfe_fseeko
__sigfe_fseeko:
        pushl   $_fseeko
        jmp     __sigfe

        .extern _fseeko64
        .global __sigfe_fseeko64
__sigfe_fseeko64:
        pushl   $_fseeko64
        jmp     __sigfe

        .extern _fsetpos
        .global __sigfe_fsetpos
__sigfe_fsetpos:
        pushl   $_fsetpos
        jmp     __sigfe

        .extern _fsetpos64
        .global __sigfe_fsetpos64
__sigfe_fsetpos64:
        pushl   $_fsetpos64
        jmp     __sigfe

        .extern _fsetxattr
        .global __sigfe_fsetxattr
__sigfe_fsetxattr:
        pushl   $_fsetxattr
        jmp     __sigfe

        .extern _fstat
        .global __sigfe_fstat
__sigfe_fstat:
        pushl   $_fstat
        jmp     __sigfe

        .extern _fstat64
        .global __sigfe_fstat64
__sigfe_fstat64:
        pushl   $_fstat64
        jmp     __sigfe

        .extern _fstatat
        .global __sigfe_fstatat
__sigfe_fstatat:
        pushl   $_fstatat
        jmp     __sigfe

        .extern _fstatfs
        .global __sigfe_fstatfs
__sigfe_fstatfs:
        pushl   $_fstatfs
        jmp     __sigfe

        .extern _fstatvfs
        .global __sigfe_fstatvfs
__sigfe_fstatvfs:
        pushl   $_fstatvfs
        jmp     __sigfe

        .extern _fsync
        .global __sigfe_fsync
__sigfe_fsync:
        pushl   $_fsync
        jmp     __sigfe

        .extern _ftell
        .global __sigfe_ftell
__sigfe_ftell:
        pushl   $_ftell
        jmp     __sigfe

        .extern _ftello
        .global __sigfe_ftello
__sigfe_ftello:
        pushl   $_ftello
        jmp     __sigfe

        .extern _ftello64
        .global __sigfe_ftello64
__sigfe_ftello64:
        pushl   $_ftello64
        jmp     __sigfe

        .extern _ftime
        .global __sigfe_ftime
__sigfe_ftime:
        pushl   $_ftime
        jmp     __sigfe

        .extern _ftok
        .global __sigfe_ftok
__sigfe_ftok:
        pushl   $_ftok
        jmp     __sigfe

        .extern _ftruncate
        .global __sigfe_ftruncate
__sigfe_ftruncate:
        pushl   $_ftruncate
        jmp     __sigfe

        .extern _ftruncate64
        .global __sigfe_ftruncate64
__sigfe_ftruncate64:
        pushl   $_ftruncate64
        jmp     __sigfe

        .extern _ftrylockfile
        .global __sigfe_ftrylockfile
__sigfe_ftrylockfile:
        pushl   $_ftrylockfile
        jmp     __sigfe

        .extern _fts_children
        .global __sigfe_fts_children
__sigfe_fts_children:
        pushl   $_fts_children
        jmp     __sigfe

        .extern _fts_close
        .global __sigfe_fts_close
__sigfe_fts_close:
        pushl   $_fts_close
        jmp     __sigfe

        .extern _fts_open
        .global __sigfe_fts_open
__sigfe_fts_open:
        pushl   $_fts_open
        jmp     __sigfe

        .extern _fts_read
        .global __sigfe_fts_read
__sigfe_fts_read:
        pushl   $_fts_read
        jmp     __sigfe

        .extern _ftw
        .global __sigfe_ftw
__sigfe_ftw:
        pushl   $_ftw
        jmp     __sigfe

        .extern _funlockfile
        .global __sigfe_funlockfile
__sigfe_funlockfile:
        pushl   $_funlockfile
        jmp     __sigfe

        .extern _funopen
        .global __sigfe_funopen
__sigfe_funopen:
        pushl   $_funopen
        jmp     __sigfe

        .extern _futimens
        .global __sigfe_futimens
__sigfe_futimens:
        pushl   $_futimens
        jmp     __sigfe

        .extern _futimes
        .global __sigfe_futimes
__sigfe_futimes:
        pushl   $_futimes
        jmp     __sigfe

        .extern _futimesat
        .global __sigfe_futimesat
__sigfe_futimesat:
        pushl   $_futimesat
        jmp     __sigfe

        .extern _fwide
        .global __sigfe_fwide
__sigfe_fwide:
        pushl   $_fwide
        jmp     __sigfe

        .extern _fwprintf
        .global __sigfe_fwprintf
__sigfe_fwprintf:
        pushl   $_fwprintf
        jmp     __sigfe

        .extern _fwrite
        .global __sigfe_fwrite
__sigfe_fwrite:
        pushl   $_fwrite
        jmp     __sigfe

        .extern _fwrite_unlocked
        .global __sigfe_fwrite_unlocked
__sigfe_fwrite_unlocked:
        pushl   $_fwrite_unlocked
        jmp     __sigfe

        .extern _fwscanf
        .global __sigfe_fwscanf
__sigfe_fwscanf:
        pushl   $_fwscanf
        jmp     __sigfe

        .extern _gcvt
        .global __sigfe_gcvt
__sigfe_gcvt:
        pushl   $_gcvt
        jmp     __sigfe

        .extern _gcvtf
        .global __sigfe_gcvtf
__sigfe_gcvtf:
        pushl   $_gcvtf
        jmp     __sigfe

        .extern _get_avphys_pages
        .global __sigfe_get_avphys_pages
__sigfe_get_avphys_pages:
        pushl   $_get_avphys_pages
        jmp     __sigfe

        .extern _get_current_dir_name
        .global __sigfe_get_current_dir_name
__sigfe_get_current_dir_name:
        pushl   $_get_current_dir_name
        jmp     __sigfe

        .extern _get_nprocs
        .global __sigfe_get_nprocs
__sigfe_get_nprocs:
        pushl   $_get_nprocs
        jmp     __sigfe

        .extern _get_nprocs_conf
        .global __sigfe_get_nprocs_conf
__sigfe_get_nprocs_conf:
        pushl   $_get_nprocs_conf
        jmp     __sigfe

        .extern _get_phys_pages
        .global __sigfe_get_phys_pages
__sigfe_get_phys_pages:
        pushl   $_get_phys_pages
        jmp     __sigfe

        .extern _getc
        .global __sigfe_getc
__sigfe_getc:
        pushl   $_getc
        jmp     __sigfe

        .extern _getc_unlocked
        .global __sigfe_getc_unlocked
__sigfe_getc_unlocked:
        pushl   $_getc_unlocked
        jmp     __sigfe

        .extern _getchar
        .global __sigfe_getchar
__sigfe_getchar:
        pushl   $_getchar
        jmp     __sigfe

        .extern _getchar_unlocked
        .global __sigfe_getchar_unlocked
__sigfe_getchar_unlocked:
        pushl   $_getchar_unlocked
        jmp     __sigfe

        .extern _getcwd
        .global __sigfe_getcwd
__sigfe_getcwd:
        pushl   $_getcwd
        jmp     __sigfe

        .extern _getdomainname
        .global __sigfe_getdomainname
__sigfe_getdomainname:
        pushl   $_getdomainname
        jmp     __sigfe

        .extern _getentropy
        .global __sigfe_getentropy
__sigfe_getentropy:
        pushl   $_getentropy
        jmp     __sigfe

        .extern _getgrent
        .global __sigfe_getgrent
__sigfe_getgrent:
        pushl   $_getgrent
        jmp     __sigfe

        .extern _getgrent32
        .global __sigfe_getgrent32
__sigfe_getgrent32:
        pushl   $_getgrent32
        jmp     __sigfe

        .extern _getgrgid
        .global __sigfe_getgrgid
__sigfe_getgrgid:
        pushl   $_getgrgid
        jmp     __sigfe

        .extern _getgrgid32
        .global __sigfe_getgrgid32
__sigfe_getgrgid32:
        pushl   $_getgrgid32
        jmp     __sigfe

        .extern _getgrgid_r
        .global __sigfe_getgrgid_r
__sigfe_getgrgid_r:
        pushl   $_getgrgid_r
        jmp     __sigfe

        .extern _getgrnam
        .global __sigfe_getgrnam
__sigfe_getgrnam:
        pushl   $_getgrnam
        jmp     __sigfe

        .extern _getgrnam32
        .global __sigfe_getgrnam32
__sigfe_getgrnam32:
        pushl   $_getgrnam32
        jmp     __sigfe

        .extern _getgrnam_r
        .global __sigfe_getgrnam_r
__sigfe_getgrnam_r:
        pushl   $_getgrnam_r
        jmp     __sigfe

        .extern _getgrouplist
        .global __sigfe_getgrouplist
__sigfe_getgrouplist:
        pushl   $_getgrouplist
        jmp     __sigfe

        .extern _getgroups
        .global __sigfe_getgroups
__sigfe_getgroups:
        pushl   $_getgroups
        jmp     __sigfe

        .extern _getgroups32
        .global __sigfe_getgroups32
__sigfe_getgroups32:
        pushl   $_getgroups32
        jmp     __sigfe

        .extern _gethostbyname2
        .global __sigfe_gethostbyname2
__sigfe_gethostbyname2:
        pushl   $_gethostbyname2
        jmp     __sigfe

        .extern _gethostid
        .global __sigfe_gethostid
__sigfe_gethostid:
        pushl   $_gethostid
        jmp     __sigfe

        .extern _getifaddrs
        .global __sigfe_getifaddrs
__sigfe_getifaddrs:
        pushl   $_getifaddrs
        jmp     __sigfe

        .extern _getitimer
        .global __sigfe_getitimer
__sigfe_getitimer:
        pushl   $_getitimer
        jmp     __sigfe

        .extern _getloadavg
        .global __sigfe_getloadavg
__sigfe_getloadavg:
        pushl   $_getloadavg
        jmp     __sigfe

        .extern _getmntent
        .global __sigfe_getmntent
__sigfe_getmntent:
        pushl   $_getmntent
        jmp     __sigfe

        .extern _getmntent_r
        .global __sigfe_getmntent_r
__sigfe_getmntent_r:
        pushl   $_getmntent_r
        jmp     __sigfe

        .extern _getmode
        .global __sigfe_getmode
__sigfe_getmode:
        pushl   $_getmode
        jmp     __sigfe

        .extern _getopt
        .global __sigfe_getopt
__sigfe_getopt:
        pushl   $_getopt
        jmp     __sigfe

        .extern _getopt_long
        .global __sigfe_getopt_long
__sigfe_getopt_long:
        pushl   $_getopt_long
        jmp     __sigfe

        .extern _getopt_long_only
        .global __sigfe_getopt_long_only
__sigfe_getopt_long_only:
        pushl   $_getopt_long_only
        jmp     __sigfe

        .extern _getpagesize
        .global __sigfe_getpagesize
__sigfe_getpagesize:
        pushl   $_getpagesize
        jmp     __sigfe

        .extern _getpass
        .global __sigfe_getpass
__sigfe_getpass:
        pushl   $_getpass
        jmp     __sigfe

        .extern _getpeereid
        .global __sigfe_getpeereid
__sigfe_getpeereid:
        pushl   $_getpeereid
        jmp     __sigfe

        .extern _getpgid
        .global __sigfe_getpgid
__sigfe_getpgid:
        pushl   $_getpgid
        jmp     __sigfe

        .extern _getpgrp
        .global __sigfe_getpgrp
__sigfe_getpgrp:
        pushl   $_getpgrp
        jmp     __sigfe

        .extern _getpriority
        .global __sigfe_getpriority
__sigfe_getpriority:
        pushl   $_getpriority
        jmp     __sigfe

        .extern _getpt
        .global __sigfe_getpt
__sigfe_getpt:
        pushl   $_getpt
        jmp     __sigfe

        .extern _getpwent
        .global __sigfe_getpwent
__sigfe_getpwent:
        pushl   $_getpwent
        jmp     __sigfe

        .extern _getpwnam
        .global __sigfe_getpwnam
__sigfe_getpwnam:
        pushl   $_getpwnam
        jmp     __sigfe

        .extern _getpwnam_r
        .global __sigfe_getpwnam_r
__sigfe_getpwnam_r:
        pushl   $_getpwnam_r
        jmp     __sigfe

        .extern _getpwuid
        .global __sigfe_getpwuid
__sigfe_getpwuid:
        pushl   $_getpwuid
        jmp     __sigfe

        .extern _getpwuid32
        .global __sigfe_getpwuid32
__sigfe_getpwuid32:
        pushl   $_getpwuid32
        jmp     __sigfe

        .extern _getpwuid_r
        .global __sigfe_getpwuid_r
__sigfe_getpwuid_r:
        pushl   $_getpwuid_r
        jmp     __sigfe

        .extern _getpwuid_r32
        .global __sigfe_getpwuid_r32
__sigfe_getpwuid_r32:
        pushl   $_getpwuid_r32
        jmp     __sigfe

        .extern _getrandom
        .global __sigfe_getrandom
__sigfe_getrandom:
        pushl   $_getrandom
        jmp     __sigfe

        .extern _getrlimit
        .global __sigfe_getrlimit
__sigfe_getrlimit:
        pushl   $_getrlimit
        jmp     __sigfe

        .extern _getrusage
        .global __sigfe_getrusage
__sigfe_getrusage:
        pushl   $_getrusage
        jmp     __sigfe

        .extern _gets
        .global __sigfe_gets
__sigfe_gets:
        pushl   $_gets
        jmp     __sigfe

        .extern _getsid
        .global __sigfe_getsid
__sigfe_getsid:
        pushl   $_getsid
        jmp     __sigfe

        .extern _gettimeofday
        .global __sigfe_gettimeofday
__sigfe_gettimeofday:
        pushl   $_gettimeofday
        jmp     __sigfe

        .extern _getusershell
        .global __sigfe_getusershell
__sigfe_getusershell:
        pushl   $_getusershell
        jmp     __sigfe

        .extern _getutent
        .global __sigfe_getutent
__sigfe_getutent:
        pushl   $_getutent
        jmp     __sigfe

        .extern _getutid
        .global __sigfe_getutid
__sigfe_getutid:
        pushl   $_getutid
        jmp     __sigfe

        .extern _getutline
        .global __sigfe_getutline
__sigfe_getutline:
        pushl   $_getutline
        jmp     __sigfe

        .extern _getutxent
        .global __sigfe_getutxent
__sigfe_getutxent:
        pushl   $_getutxent
        jmp     __sigfe

        .extern _getutxid
        .global __sigfe_getutxid
__sigfe_getutxid:
        pushl   $_getutxid
        jmp     __sigfe

        .extern _getutxline
        .global __sigfe_getutxline
__sigfe_getutxline:
        pushl   $_getutxline
        jmp     __sigfe

        .extern _getw
        .global __sigfe_getw
__sigfe_getw:
        pushl   $_getw
        jmp     __sigfe

        .extern _getwc
        .global __sigfe_getwc
__sigfe_getwc:
        pushl   $_getwc
        jmp     __sigfe

        .extern _getwc_unlocked
        .global __sigfe_getwc_unlocked
__sigfe_getwc_unlocked:
        pushl   $_getwc_unlocked
        jmp     __sigfe

        .extern _getwchar
        .global __sigfe_getwchar
__sigfe_getwchar:
        pushl   $_getwchar
        jmp     __sigfe

        .extern _getwchar_unlocked
        .global __sigfe_getwchar_unlocked
__sigfe_getwchar_unlocked:
        pushl   $_getwchar_unlocked
        jmp     __sigfe

        .extern _getwd
        .global __sigfe_getwd
__sigfe_getwd:
        pushl   $_getwd
        jmp     __sigfe

        .extern _getxattr
        .global __sigfe_getxattr
__sigfe_getxattr:
        pushl   $_getxattr
        jmp     __sigfe

        .extern _glob
        .global __sigfe_glob
__sigfe_glob:
        pushl   $_glob
        jmp     __sigfe

        .extern _glob_pattern_p
        .global __sigfe_glob_pattern_p
__sigfe_glob_pattern_p:
        pushl   $_glob_pattern_p
        jmp     __sigfe

        .extern _globfree
        .global __sigfe_globfree
__sigfe_globfree:
        pushl   $_globfree
        jmp     __sigfe

        .extern _gmtime
        .global __sigfe_gmtime
__sigfe_gmtime:
        pushl   $_gmtime
        jmp     __sigfe

        .extern _gmtime_r
        .global __sigfe_gmtime_r
__sigfe_gmtime_r:
        pushl   $_gmtime_r
        jmp     __sigfe

        .extern _hcreate
        .global __sigfe_hcreate
__sigfe_hcreate:
        pushl   $_hcreate
        jmp     __sigfe

        .extern _hcreate_r
        .global __sigfe_hcreate_r
__sigfe_hcreate_r:
        pushl   $_hcreate_r
        jmp     __sigfe

        .extern _hdestroy
        .global __sigfe_hdestroy
__sigfe_hdestroy:
        pushl   $_hdestroy
        jmp     __sigfe

        .extern _hdestroy_r
        .global __sigfe_hdestroy_r
__sigfe_hdestroy_r:
        pushl   $_hdestroy_r
        jmp     __sigfe

        .extern _hsearch
        .global __sigfe_hsearch
__sigfe_hsearch:
        pushl   $_hsearch
        jmp     __sigfe

        .extern _hsearch_r
        .global __sigfe_hsearch_r
__sigfe_hsearch_r:
        pushl   $_hsearch_r
        jmp     __sigfe

        .extern _if_freenameindex
        .global __sigfe_if_freenameindex
__sigfe_if_freenameindex:
        pushl   $_if_freenameindex
        jmp     __sigfe

        .extern _if_nameindex
        .global __sigfe_if_nameindex
__sigfe_if_nameindex:
        pushl   $_if_nameindex
        jmp     __sigfe

        .extern _initgroups
        .global __sigfe_initgroups
__sigfe_initgroups:
        pushl   $_initgroups
        jmp     __sigfe

        .extern _initgroups32
        .global __sigfe_initgroups32
__sigfe_initgroups32:
        pushl   $_initgroups32
        jmp     __sigfe

        .extern _ioctl
        .global __sigfe_ioctl
__sigfe_ioctl:
        pushl   $_ioctl
        jmp     __sigfe

        .extern _iprintf
        .global __sigfe_iprintf
__sigfe_iprintf:
        pushl   $_iprintf
        jmp     __sigfe

        .extern _iruserok
        .global __sigfe_iruserok
__sigfe_iruserok:
        pushl   $_iruserok
        jmp     __sigfe

        .extern _iruserok_sa
        .global __sigfe_iruserok_sa
__sigfe_iruserok_sa:
        pushl   $_iruserok_sa
        jmp     __sigfe

        .extern _isatty
        .global __sigfe_isatty
__sigfe_isatty:
        pushl   $_isatty
        jmp     __sigfe

        .extern _kill
        .global __sigfe_kill
__sigfe_kill:
        pushl   $_kill
        jmp     __sigfe

        .extern _killpg
        .global __sigfe_killpg
__sigfe_killpg:
        pushl   $_killpg
        jmp     __sigfe

        .extern _lacl
        .global __sigfe_lacl
__sigfe_lacl:
        pushl   $_lacl
        jmp     __sigfe

        .extern _lchown
        .global __sigfe_lchown
__sigfe_lchown:
        pushl   $_lchown
        jmp     __sigfe

        .extern _lchown32
        .global __sigfe_lchown32
__sigfe_lchown32:
        pushl   $_lchown32
        jmp     __sigfe

        .extern _lgetxattr
        .global __sigfe_lgetxattr
__sigfe_lgetxattr:
        pushl   $_lgetxattr
        jmp     __sigfe

        .extern _link
        .global __sigfe_link
__sigfe_link:
        pushl   $_link
        jmp     __sigfe

        .extern _linkat
        .global __sigfe_linkat
__sigfe_linkat:
        pushl   $_linkat
        jmp     __sigfe

        .extern _lio_listio
        .global __sigfe_lio_listio
__sigfe_lio_listio:
        pushl   $_lio_listio
        jmp     __sigfe

        .extern _listxattr
        .global __sigfe_listxattr
__sigfe_listxattr:
        pushl   $_listxattr
        jmp     __sigfe

        .extern _llistxattr
        .global __sigfe_llistxattr
__sigfe_llistxattr:
        pushl   $_llistxattr
        jmp     __sigfe

        .extern _localtime
        .global __sigfe_localtime
__sigfe_localtime:
        pushl   $_localtime
        jmp     __sigfe

        .extern _localtime_r
        .global __sigfe_localtime_r
__sigfe_localtime_r:
        pushl   $_localtime_r
        jmp     __sigfe

        .extern _lockf
        .global __sigfe_lockf
__sigfe_lockf:
        pushl   $_lockf
        jmp     __sigfe

        .extern _login
        .global __sigfe_login
__sigfe_login:
        pushl   $_login
        jmp     __sigfe

        .extern _login_tty
        .global __sigfe_login_tty
__sigfe_login_tty:
        pushl   $_login_tty
        jmp     __sigfe

        .extern _logout
        .global __sigfe_logout
__sigfe_logout:
        pushl   $_logout
        jmp     __sigfe

        .extern _logwtmp
        .global __sigfe_logwtmp
__sigfe_logwtmp:
        pushl   $_logwtmp
        jmp     __sigfe

        .extern _lremovexattr
        .global __sigfe_lremovexattr
__sigfe_lremovexattr:
        pushl   $_lremovexattr
        jmp     __sigfe

        .extern _lseek
        .global __sigfe_lseek
__sigfe_lseek:
        pushl   $_lseek
        jmp     __sigfe

        .extern _lseek64
        .global __sigfe_lseek64
__sigfe_lseek64:
        pushl   $_lseek64
        jmp     __sigfe

        .extern _lsetxattr
        .global __sigfe_lsetxattr
__sigfe_lsetxattr:
        pushl   $_lsetxattr
        jmp     __sigfe

        .extern _lstat
        .global __sigfe_lstat
__sigfe_lstat:
        pushl   $_lstat
        jmp     __sigfe

        .extern _lstat64
        .global __sigfe_lstat64
__sigfe_lstat64:
        pushl   $_lstat64
        jmp     __sigfe

        .extern _lutimes
        .global __sigfe_lutimes
__sigfe_lutimes:
        pushl   $_lutimes
        jmp     __sigfe

        .extern _mallinfo
        .global __sigfe_mallinfo
__sigfe_mallinfo:
        pushl   $_mallinfo
        jmp     __sigfe

        .extern _malloc
        .global __sigfe_malloc
__sigfe_malloc:
        pushl   $_malloc
        jmp     __sigfe

        .extern _malloc_stats
        .global __sigfe_malloc_stats
__sigfe_malloc_stats:
        pushl   $_malloc_stats
        jmp     __sigfe

        .extern _malloc_trim
        .global __sigfe_malloc_trim
__sigfe_malloc_trim:
        pushl   $_malloc_trim
        jmp     __sigfe

        .extern _malloc_usable_size
        .global __sigfe_malloc_usable_size
__sigfe_malloc_usable_size:
        pushl   $_malloc_usable_size
        jmp     __sigfe

        .extern _mallopt
        .global __sigfe_mallopt
__sigfe_mallopt:
        pushl   $_mallopt
        jmp     __sigfe

        .extern _memalign
        .global __sigfe_memalign
__sigfe_memalign:
        pushl   $_memalign
        jmp     __sigfe

        .extern _mkdir
        .global __sigfe_mkdir
__sigfe_mkdir:
        pushl   $_mkdir
        jmp     __sigfe

        .extern _mkdirat
        .global __sigfe_mkdirat
__sigfe_mkdirat:
        pushl   $_mkdirat
        jmp     __sigfe

        .extern _mkdtemp
        .global __sigfe_mkdtemp
__sigfe_mkdtemp:
        pushl   $_mkdtemp
        jmp     __sigfe

        .extern _mkfifo
        .global __sigfe_mkfifo
__sigfe_mkfifo:
        pushl   $_mkfifo
        jmp     __sigfe

        .extern _mkfifoat
        .global __sigfe_mkfifoat
__sigfe_mkfifoat:
        pushl   $_mkfifoat
        jmp     __sigfe

        .extern _mknod
        .global __sigfe_mknod
__sigfe_mknod:
        pushl   $_mknod
        jmp     __sigfe

        .extern _mknod32
        .global __sigfe_mknod32
__sigfe_mknod32:
        pushl   $_mknod32
        jmp     __sigfe

        .extern _mknodat
        .global __sigfe_mknodat
__sigfe_mknodat:
        pushl   $_mknodat
        jmp     __sigfe

        .extern _mkostemp
        .global __sigfe_mkostemp
__sigfe_mkostemp:
        pushl   $_mkostemp
        jmp     __sigfe

        .extern _mkostemps
        .global __sigfe_mkostemps
__sigfe_mkostemps:
        pushl   $_mkostemps
        jmp     __sigfe

        .extern _mkstemp
        .global __sigfe_mkstemp
__sigfe_mkstemp:
        pushl   $_mkstemp
        jmp     __sigfe

        .extern _mkstemps
        .global __sigfe_mkstemps
__sigfe_mkstemps:
        pushl   $_mkstemps
        jmp     __sigfe

        .extern _mktemp
        .global __sigfe_mktemp
__sigfe_mktemp:
        pushl   $_mktemp
        jmp     __sigfe

        .extern _mktime
        .global __sigfe_mktime
__sigfe_mktime:
        pushl   $_mktime
        jmp     __sigfe

        .extern _mlock
        .global __sigfe_mlock
__sigfe_mlock:
        pushl   $_mlock
        jmp     __sigfe

        .extern _mmap
        .global __sigfe_mmap
__sigfe_mmap:
        pushl   $_mmap
        jmp     __sigfe

        .extern _mmap64
        .global __sigfe_mmap64
__sigfe_mmap64:
        pushl   $_mmap64
        jmp     __sigfe

        .extern _mount
        .global __sigfe_mount
__sigfe_mount:
        pushl   $_mount
        jmp     __sigfe

        .extern _mprotect
        .global __sigfe_mprotect
__sigfe_mprotect:
        pushl   $_mprotect
        jmp     __sigfe

        .extern _mq_close
        .global __sigfe_mq_close
__sigfe_mq_close:
        pushl   $_mq_close
        jmp     __sigfe

        .extern _mq_getattr
        .global __sigfe_mq_getattr
__sigfe_mq_getattr:
        pushl   $_mq_getattr
        jmp     __sigfe

        .extern _mq_notify
        .global __sigfe_mq_notify
__sigfe_mq_notify:
        pushl   $_mq_notify
        jmp     __sigfe

        .extern _mq_open
        .global __sigfe_mq_open
__sigfe_mq_open:
        pushl   $_mq_open
        jmp     __sigfe

        .extern _mq_receive
        .global __sigfe_mq_receive
__sigfe_mq_receive:
        pushl   $_mq_receive
        jmp     __sigfe

        .extern _mq_send
        .global __sigfe_mq_send
__sigfe_mq_send:
        pushl   $_mq_send
        jmp     __sigfe

        .extern _mq_setattr
        .global __sigfe_mq_setattr
__sigfe_mq_setattr:
        pushl   $_mq_setattr
        jmp     __sigfe

        .extern _mq_timedreceive
        .global __sigfe_mq_timedreceive
__sigfe_mq_timedreceive:
        pushl   $_mq_timedreceive
        jmp     __sigfe

        .extern _mq_timedsend
        .global __sigfe_mq_timedsend
__sigfe_mq_timedsend:
        pushl   $_mq_timedsend
        jmp     __sigfe

        .extern _mq_unlink
        .global __sigfe_mq_unlink
__sigfe_mq_unlink:
        pushl   $_mq_unlink
        jmp     __sigfe

        .extern _msgctl
        .global __sigfe_msgctl
__sigfe_msgctl:
        pushl   $_msgctl
        jmp     __sigfe

        .extern _msgget
        .global __sigfe_msgget
__sigfe_msgget:
        pushl   $_msgget
        jmp     __sigfe

        .extern _msgrcv
        .global __sigfe_msgrcv
__sigfe_msgrcv:
        pushl   $_msgrcv
        jmp     __sigfe

        .extern _msgsnd
        .global __sigfe_msgsnd
__sigfe_msgsnd:
        pushl   $_msgsnd
        jmp     __sigfe

        .extern _msync
        .global __sigfe_msync
__sigfe_msync:
        pushl   $_msync
        jmp     __sigfe

        .extern _mtx_destroy
        .global __sigfe_mtx_destroy
__sigfe_mtx_destroy:
        pushl   $_mtx_destroy
        jmp     __sigfe

        .extern _mtx_init
        .global __sigfe_mtx_init
__sigfe_mtx_init:
        pushl   $_mtx_init
        jmp     __sigfe

        .extern _mtx_lock
        .global __sigfe_mtx_lock
__sigfe_mtx_lock:
        pushl   $_mtx_lock
        jmp     __sigfe

        .extern _mtx_timedlock
        .global __sigfe_mtx_timedlock
__sigfe_mtx_timedlock:
        pushl   $_mtx_timedlock
        jmp     __sigfe

        .extern _mtx_trylock
        .global __sigfe_mtx_trylock
__sigfe_mtx_trylock:
        pushl   $_mtx_trylock
        jmp     __sigfe

        .extern _mtx_unlock
        .global __sigfe_mtx_unlock
__sigfe_mtx_unlock:
        pushl   $_mtx_unlock
        jmp     __sigfe

        .extern _munlock
        .global __sigfe_munlock
__sigfe_munlock:
        pushl   $_munlock
        jmp     __sigfe

        .extern _munmap
        .global __sigfe_munmap
__sigfe_munmap:
        pushl   $_munmap
        jmp     __sigfe

        .extern _nanosleep
        .global __sigfe_nanosleep
__sigfe_nanosleep:
        pushl   $_nanosleep
        jmp     __sigfe

        .extern _newlocale
        .global __sigfe_newlocale
__sigfe_newlocale:
        pushl   $_newlocale
        jmp     __sigfe

        .extern _nftw
        .global __sigfe_nftw
__sigfe_nftw:
        pushl   $_nftw
        jmp     __sigfe

        .extern _nice
        .global __sigfe_nice
__sigfe_nice:
        pushl   $_nice
        jmp     __sigfe

        .extern _nl_langinfo
        .global __sigfe_nl_langinfo
__sigfe_nl_langinfo:
        pushl   $_nl_langinfo
        jmp     __sigfe

        .extern _nl_langinfo_l
        .global __sigfe_nl_langinfo_l
__sigfe_nl_langinfo_l:
        pushl   $_nl_langinfo_l
        jmp     __sigfe

        .extern _on_exit
        .global __sigfe_on_exit
__sigfe_on_exit:
        pushl   $_on_exit
        jmp     __sigfe

        .extern _open
        .global __sigfe_open
__sigfe_open:
        pushl   $_open
        jmp     __sigfe

        .extern _open_memstream
        .global __sigfe_open_memstream
__sigfe_open_memstream:
        pushl   $_open_memstream
        jmp     __sigfe

        .extern _open_wmemstream
        .global __sigfe_open_wmemstream
__sigfe_open_wmemstream:
        pushl   $_open_wmemstream
        jmp     __sigfe

        .extern _openat
        .global __sigfe_openat
__sigfe_openat:
        pushl   $_openat
        jmp     __sigfe

        .extern _opendir
        .global __sigfe_opendir
__sigfe_opendir:
        pushl   $_opendir
        jmp     __sigfe

        .extern _openlog
        .global __sigfe_openlog
__sigfe_openlog:
        pushl   $_openlog
        jmp     __sigfe

        .extern _openpty
        .global __sigfe_openpty
__sigfe_openpty:
        pushl   $_openpty
        jmp     __sigfe

        .extern _pathconf
        .global __sigfe_pathconf
__sigfe_pathconf:
        pushl   $_pathconf
        jmp     __sigfe

        .extern _pause
        .global __sigfe_pause
__sigfe_pause:
        pushl   $_pause
        jmp     __sigfe

        .extern _pclose
        .global __sigfe_pclose
__sigfe_pclose:
        pushl   $_pclose
        jmp     __sigfe

        .extern _perror
        .global __sigfe_perror
__sigfe_perror:
        pushl   $_perror
        jmp     __sigfe

        .extern _pipe
        .global __sigfe_pipe
__sigfe_pipe:
        pushl   $_pipe
        jmp     __sigfe

        .extern _pipe2
        .global __sigfe_pipe2
__sigfe_pipe2:
        pushl   $_pipe2
        jmp     __sigfe

        .extern _poll
        .global __sigfe_poll
__sigfe_poll:
        pushl   $_poll
        jmp     __sigfe

        .extern _popen
        .global __sigfe_popen
__sigfe_popen:
        pushl   $_popen
        jmp     __sigfe

        .extern _posix_fadvise
        .global __sigfe_posix_fadvise
__sigfe_posix_fadvise:
        pushl   $_posix_fadvise
        jmp     __sigfe

        .extern _posix_fallocate
        .global __sigfe_posix_fallocate
__sigfe_posix_fallocate:
        pushl   $_posix_fallocate
        jmp     __sigfe

        .extern _posix_madvise
        .global __sigfe_posix_madvise
__sigfe_posix_madvise:
        pushl   $_posix_madvise
        jmp     __sigfe

        .extern _posix_memalign
        .global __sigfe_posix_memalign
__sigfe_posix_memalign:
        pushl   $_posix_memalign
        jmp     __sigfe

        .extern _posix_openpt
        .global __sigfe_posix_openpt
__sigfe_posix_openpt:
        pushl   $_posix_openpt
        jmp     __sigfe

        .extern _posix_spawn
        .global __sigfe_posix_spawn
__sigfe_posix_spawn:
        pushl   $_posix_spawn
        jmp     __sigfe

        .extern _posix_spawn_file_actions_addclose
        .global __sigfe_posix_spawn_file_actions_addclose
__sigfe_posix_spawn_file_actions_addclose:
        pushl   $_posix_spawn_file_actions_addclose
        jmp     __sigfe

        .extern _posix_spawn_file_actions_adddup2
        .global __sigfe_posix_spawn_file_actions_adddup2
__sigfe_posix_spawn_file_actions_adddup2:
        pushl   $_posix_spawn_file_actions_adddup2
        jmp     __sigfe

        .extern _posix_spawn_file_actions_addopen
        .global __sigfe_posix_spawn_file_actions_addopen
__sigfe_posix_spawn_file_actions_addopen:
        pushl   $_posix_spawn_file_actions_addopen
        jmp     __sigfe

        .extern _posix_spawn_file_actions_destroy
        .global __sigfe_posix_spawn_file_actions_destroy
__sigfe_posix_spawn_file_actions_destroy:
        pushl   $_posix_spawn_file_actions_destroy
        jmp     __sigfe

        .extern _posix_spawn_file_actions_init
        .global __sigfe_posix_spawn_file_actions_init
__sigfe_posix_spawn_file_actions_init:
        pushl   $_posix_spawn_file_actions_init
        jmp     __sigfe

        .extern _posix_spawnattr_destroy
        .global __sigfe_posix_spawnattr_destroy
__sigfe_posix_spawnattr_destroy:
        pushl   $_posix_spawnattr_destroy
        jmp     __sigfe

        .extern _posix_spawnattr_init
        .global __sigfe_posix_spawnattr_init
__sigfe_posix_spawnattr_init:
        pushl   $_posix_spawnattr_init
        jmp     __sigfe

        .extern _posix_spawnp
        .global __sigfe_posix_spawnp
__sigfe_posix_spawnp:
        pushl   $_posix_spawnp
        jmp     __sigfe

        .extern _ppoll
        .global __sigfe_ppoll
__sigfe_ppoll:
        pushl   $_ppoll
        jmp     __sigfe

        .extern _pread
        .global __sigfe_pread
__sigfe_pread:
        pushl   $_pread
        jmp     __sigfe

        .extern _printf
        .global __sigfe_printf
__sigfe_printf:
        pushl   $_printf
        jmp     __sigfe

        .extern _pselect
        .global __sigfe_pselect
__sigfe_pselect:
        pushl   $_pselect
        jmp     __sigfe

        .extern _psiginfo
        .global __sigfe_psiginfo
__sigfe_psiginfo:
        pushl   $_psiginfo
        jmp     __sigfe

        .extern _psignal
        .global __sigfe_psignal
__sigfe_psignal:
        pushl   $_psignal
        jmp     __sigfe

        .extern _pthread_atfork
        .global __sigfe_pthread_atfork
__sigfe_pthread_atfork:
        pushl   $_pthread_atfork
        jmp     __sigfe

        .extern _pthread_attr_destroy
        .global __sigfe_pthread_attr_destroy
__sigfe_pthread_attr_destroy:
        pushl   $_pthread_attr_destroy
        jmp     __sigfe

        .extern _pthread_attr_getdetachstate
        .global __sigfe_pthread_attr_getdetachstate
__sigfe_pthread_attr_getdetachstate:
        pushl   $_pthread_attr_getdetachstate
        jmp     __sigfe

        .extern _pthread_attr_getguardsize
        .global __sigfe_pthread_attr_getguardsize
__sigfe_pthread_attr_getguardsize:
        pushl   $_pthread_attr_getguardsize
        jmp     __sigfe

        .extern _pthread_attr_getinheritsched
        .global __sigfe_pthread_attr_getinheritsched
__sigfe_pthread_attr_getinheritsched:
        pushl   $_pthread_attr_getinheritsched
        jmp     __sigfe

        .extern _pthread_attr_getschedparam
        .global __sigfe_pthread_attr_getschedparam
__sigfe_pthread_attr_getschedparam:
        pushl   $_pthread_attr_getschedparam
        jmp     __sigfe

        .extern _pthread_attr_getschedpolicy
        .global __sigfe_pthread_attr_getschedpolicy
__sigfe_pthread_attr_getschedpolicy:
        pushl   $_pthread_attr_getschedpolicy
        jmp     __sigfe

        .extern _pthread_attr_getscope
        .global __sigfe_pthread_attr_getscope
__sigfe_pthread_attr_getscope:
        pushl   $_pthread_attr_getscope
        jmp     __sigfe

        .extern _pthread_attr_getstack
        .global __sigfe_pthread_attr_getstack
__sigfe_pthread_attr_getstack:
        pushl   $_pthread_attr_getstack
        jmp     __sigfe

        .extern _pthread_attr_getstackaddr
        .global __sigfe_pthread_attr_getstackaddr
__sigfe_pthread_attr_getstackaddr:
        pushl   $_pthread_attr_getstackaddr
        jmp     __sigfe

        .extern _pthread_attr_getstacksize
        .global __sigfe_pthread_attr_getstacksize
__sigfe_pthread_attr_getstacksize:
        pushl   $_pthread_attr_getstacksize
        jmp     __sigfe

        .extern _pthread_attr_init
        .global __sigfe_pthread_attr_init
__sigfe_pthread_attr_init:
        pushl   $_pthread_attr_init
        jmp     __sigfe

        .extern _pthread_attr_setdetachstate
        .global __sigfe_pthread_attr_setdetachstate
__sigfe_pthread_attr_setdetachstate:
        pushl   $_pthread_attr_setdetachstate
        jmp     __sigfe

        .extern _pthread_attr_setguardsize
        .global __sigfe_pthread_attr_setguardsize
__sigfe_pthread_attr_setguardsize:
        pushl   $_pthread_attr_setguardsize
        jmp     __sigfe

        .extern _pthread_attr_setinheritsched
        .global __sigfe_pthread_attr_setinheritsched
__sigfe_pthread_attr_setinheritsched:
        pushl   $_pthread_attr_setinheritsched
        jmp     __sigfe

        .extern _pthread_attr_setschedparam
        .global __sigfe_pthread_attr_setschedparam
__sigfe_pthread_attr_setschedparam:
        pushl   $_pthread_attr_setschedparam
        jmp     __sigfe

        .extern _pthread_attr_setschedpolicy
        .global __sigfe_pthread_attr_setschedpolicy
__sigfe_pthread_attr_setschedpolicy:
        pushl   $_pthread_attr_setschedpolicy
        jmp     __sigfe

        .extern _pthread_attr_setscope
        .global __sigfe_pthread_attr_setscope
__sigfe_pthread_attr_setscope:
        pushl   $_pthread_attr_setscope
        jmp     __sigfe

        .extern _pthread_attr_setstack
        .global __sigfe_pthread_attr_setstack
__sigfe_pthread_attr_setstack:
        pushl   $_pthread_attr_setstack
        jmp     __sigfe

        .extern _pthread_attr_setstackaddr
        .global __sigfe_pthread_attr_setstackaddr
__sigfe_pthread_attr_setstackaddr:
        pushl   $_pthread_attr_setstackaddr
        jmp     __sigfe

        .extern _pthread_attr_setstacksize
        .global __sigfe_pthread_attr_setstacksize
__sigfe_pthread_attr_setstacksize:
        pushl   $_pthread_attr_setstacksize
        jmp     __sigfe

        .extern _pthread_barrier_destroy
        .global __sigfe_pthread_barrier_destroy
__sigfe_pthread_barrier_destroy:
        pushl   $_pthread_barrier_destroy
        jmp     __sigfe

        .extern _pthread_barrier_init
        .global __sigfe_pthread_barrier_init
__sigfe_pthread_barrier_init:
        pushl   $_pthread_barrier_init
        jmp     __sigfe

        .extern _pthread_barrier_wait
        .global __sigfe_pthread_barrier_wait
__sigfe_pthread_barrier_wait:
        pushl   $_pthread_barrier_wait
        jmp     __sigfe

        .extern _pthread_barrierattr_destroy
        .global __sigfe_pthread_barrierattr_destroy
__sigfe_pthread_barrierattr_destroy:
        pushl   $_pthread_barrierattr_destroy
        jmp     __sigfe

        .extern _pthread_barrierattr_getpshared
        .global __sigfe_pthread_barrierattr_getpshared
__sigfe_pthread_barrierattr_getpshared:
        pushl   $_pthread_barrierattr_getpshared
        jmp     __sigfe

        .extern _pthread_barrierattr_init
        .global __sigfe_pthread_barrierattr_init
__sigfe_pthread_barrierattr_init:
        pushl   $_pthread_barrierattr_init
        jmp     __sigfe

        .extern _pthread_barrierattr_setpshared
        .global __sigfe_pthread_barrierattr_setpshared
__sigfe_pthread_barrierattr_setpshared:
        pushl   $_pthread_barrierattr_setpshared
        jmp     __sigfe

        .extern _pthread_cancel
        .global __sigfe_pthread_cancel
__sigfe_pthread_cancel:
        pushl   $_pthread_cancel
        jmp     __sigfe

        .extern _pthread_cond_broadcast
        .global __sigfe_pthread_cond_broadcast
__sigfe_pthread_cond_broadcast:
        pushl   $_pthread_cond_broadcast
        jmp     __sigfe

        .extern _pthread_cond_clockwait
        .global __sigfe_pthread_cond_clockwait
__sigfe_pthread_cond_clockwait:
        pushl   $_pthread_cond_clockwait
        jmp     __sigfe

        .extern _pthread_cond_destroy
        .global __sigfe_pthread_cond_destroy
__sigfe_pthread_cond_destroy:
        pushl   $_pthread_cond_destroy
        jmp     __sigfe

        .extern _pthread_cond_init
        .global __sigfe_pthread_cond_init
__sigfe_pthread_cond_init:
        pushl   $_pthread_cond_init
        jmp     __sigfe

        .extern _pthread_cond_signal
        .global __sigfe_pthread_cond_signal
__sigfe_pthread_cond_signal:
        pushl   $_pthread_cond_signal
        jmp     __sigfe

        .extern _pthread_cond_timedwait
        .global __sigfe_pthread_cond_timedwait
__sigfe_pthread_cond_timedwait:
        pushl   $_pthread_cond_timedwait
        jmp     __sigfe

        .extern _pthread_cond_wait
        .global __sigfe_pthread_cond_wait
__sigfe_pthread_cond_wait:
        pushl   $_pthread_cond_wait
        jmp     __sigfe

        .extern _pthread_condattr_destroy
        .global __sigfe_pthread_condattr_destroy
__sigfe_pthread_condattr_destroy:
        pushl   $_pthread_condattr_destroy
        jmp     __sigfe

        .extern _pthread_condattr_getclock
        .global __sigfe_pthread_condattr_getclock
__sigfe_pthread_condattr_getclock:
        pushl   $_pthread_condattr_getclock
        jmp     __sigfe

        .extern _pthread_condattr_getpshared
        .global __sigfe_pthread_condattr_getpshared
__sigfe_pthread_condattr_getpshared:
        pushl   $_pthread_condattr_getpshared
        jmp     __sigfe

        .extern _pthread_condattr_init
        .global __sigfe_pthread_condattr_init
__sigfe_pthread_condattr_init:
        pushl   $_pthread_condattr_init
        jmp     __sigfe

        .extern _pthread_condattr_setclock
        .global __sigfe_pthread_condattr_setclock
__sigfe_pthread_condattr_setclock:
        pushl   $_pthread_condattr_setclock
        jmp     __sigfe

        .extern _pthread_condattr_setpshared
        .global __sigfe_pthread_condattr_setpshared
__sigfe_pthread_condattr_setpshared:
        pushl   $_pthread_condattr_setpshared
        jmp     __sigfe

        .extern _pthread_continue
        .global __sigfe_pthread_continue
__sigfe_pthread_continue:
        pushl   $_pthread_continue
        jmp     __sigfe

        .extern _pthread_create
        .global __sigfe_pthread_create
__sigfe_pthread_create:
        pushl   $_pthread_create
        jmp     __sigfe

        .extern _pthread_detach
        .global __sigfe_pthread_detach
__sigfe_pthread_detach:
        pushl   $_pthread_detach
        jmp     __sigfe

        .extern _pthread_equal
        .global __sigfe_pthread_equal
__sigfe_pthread_equal:
        pushl   $_pthread_equal
        jmp     __sigfe

        .extern _pthread_exit
        .global __sigfe_pthread_exit
__sigfe_pthread_exit:
        pushl   $_pthread_exit
        jmp     __sigfe

        .extern _pthread_getaffinity_np
        .global __sigfe_pthread_getaffinity_np
__sigfe_pthread_getaffinity_np:
        pushl   $_pthread_getaffinity_np
        jmp     __sigfe

        .extern _pthread_getattr_np
        .global __sigfe_pthread_getattr_np
__sigfe_pthread_getattr_np:
        pushl   $_pthread_getattr_np
        jmp     __sigfe

        .extern _pthread_getconcurrency
        .global __sigfe_pthread_getconcurrency
__sigfe_pthread_getconcurrency:
        pushl   $_pthread_getconcurrency
        jmp     __sigfe

        .extern _pthread_getcpuclockid
        .global __sigfe_pthread_getcpuclockid
__sigfe_pthread_getcpuclockid:
        pushl   $_pthread_getcpuclockid
        jmp     __sigfe

        .extern _pthread_getname_np
        .global __sigfe_pthread_getname_np
__sigfe_pthread_getname_np:
        pushl   $_pthread_getname_np
        jmp     __sigfe

        .extern _pthread_getschedparam
        .global __sigfe_pthread_getschedparam
__sigfe_pthread_getschedparam:
        pushl   $_pthread_getschedparam
        jmp     __sigfe

        .extern _pthread_getsequence_np
        .global __sigfe_pthread_getsequence_np
__sigfe_pthread_getsequence_np:
        pushl   $_pthread_getsequence_np
        jmp     __sigfe

        .extern _pthread_getspecific
        .global __sigfe_pthread_getspecific
__sigfe_pthread_getspecific:
        pushl   $_pthread_getspecific
        jmp     __sigfe

        .extern _pthread_join
        .global __sigfe_pthread_join
__sigfe_pthread_join:
        pushl   $_pthread_join
        jmp     __sigfe

        .extern _pthread_key_create
        .global __sigfe_pthread_key_create
__sigfe_pthread_key_create:
        pushl   $_pthread_key_create
        jmp     __sigfe

        .extern _pthread_key_delete
        .global __sigfe_pthread_key_delete
__sigfe_pthread_key_delete:
        pushl   $_pthread_key_delete
        jmp     __sigfe

        .extern _pthread_kill
        .global __sigfe_pthread_kill
__sigfe_pthread_kill:
        pushl   $_pthread_kill
        jmp     __sigfe

        .extern _pthread_mutex_clocklock
        .global __sigfe_pthread_mutex_clocklock
__sigfe_pthread_mutex_clocklock:
        pushl   $_pthread_mutex_clocklock
        jmp     __sigfe

        .extern _pthread_mutex_destroy
        .global __sigfe_pthread_mutex_destroy
__sigfe_pthread_mutex_destroy:
        pushl   $_pthread_mutex_destroy
        jmp     __sigfe

        .extern _pthread_mutex_getprioceiling
        .global __sigfe_pthread_mutex_getprioceiling
__sigfe_pthread_mutex_getprioceiling:
        pushl   $_pthread_mutex_getprioceiling
        jmp     __sigfe

        .extern _pthread_mutex_init
        .global __sigfe_pthread_mutex_init
__sigfe_pthread_mutex_init:
        pushl   $_pthread_mutex_init
        jmp     __sigfe

        .extern _pthread_mutex_lock
        .global __sigfe_pthread_mutex_lock
__sigfe_pthread_mutex_lock:
        pushl   $_pthread_mutex_lock
        jmp     __sigfe

        .extern _pthread_mutex_setprioceiling
        .global __sigfe_pthread_mutex_setprioceiling
__sigfe_pthread_mutex_setprioceiling:
        pushl   $_pthread_mutex_setprioceiling
        jmp     __sigfe

        .extern _pthread_mutex_timedlock
        .global __sigfe_pthread_mutex_timedlock
__sigfe_pthread_mutex_timedlock:
        pushl   $_pthread_mutex_timedlock
        jmp     __sigfe

        .extern _pthread_mutex_trylock
        .global __sigfe_pthread_mutex_trylock
__sigfe_pthread_mutex_trylock:
        pushl   $_pthread_mutex_trylock
        jmp     __sigfe

        .extern _pthread_mutex_unlock
        .global __sigfe_pthread_mutex_unlock
__sigfe_pthread_mutex_unlock:
        pushl   $_pthread_mutex_unlock
        jmp     __sigfe

        .extern _pthread_mutexattr_destroy
        .global __sigfe_pthread_mutexattr_destroy
__sigfe_pthread_mutexattr_destroy:
        pushl   $_pthread_mutexattr_destroy
        jmp     __sigfe

        .extern _pthread_mutexattr_getprioceiling
        .global __sigfe_pthread_mutexattr_getprioceiling
__sigfe_pthread_mutexattr_getprioceiling:
        pushl   $_pthread_mutexattr_getprioceiling
        jmp     __sigfe

        .extern _pthread_mutexattr_getprotocol
        .global __sigfe_pthread_mutexattr_getprotocol
__sigfe_pthread_mutexattr_getprotocol:
        pushl   $_pthread_mutexattr_getprotocol
        jmp     __sigfe

        .extern _pthread_mutexattr_getpshared
        .global __sigfe_pthread_mutexattr_getpshared
__sigfe_pthread_mutexattr_getpshared:
        pushl   $_pthread_mutexattr_getpshared
        jmp     __sigfe

        .extern _pthread_mutexattr_gettype
        .global __sigfe_pthread_mutexattr_gettype
__sigfe_pthread_mutexattr_gettype:
        pushl   $_pthread_mutexattr_gettype
        jmp     __sigfe

        .extern _pthread_mutexattr_init
        .global __sigfe_pthread_mutexattr_init
__sigfe_pthread_mutexattr_init:
        pushl   $_pthread_mutexattr_init
        jmp     __sigfe

        .extern _pthread_mutexattr_setprioceiling
        .global __sigfe_pthread_mutexattr_setprioceiling
__sigfe_pthread_mutexattr_setprioceiling:
        pushl   $_pthread_mutexattr_setprioceiling
        jmp     __sigfe

        .extern _pthread_mutexattr_setprotocol
        .global __sigfe_pthread_mutexattr_setprotocol
__sigfe_pthread_mutexattr_setprotocol:
        pushl   $_pthread_mutexattr_setprotocol
        jmp     __sigfe

        .extern _pthread_mutexattr_setpshared
        .global __sigfe_pthread_mutexattr_setpshared
__sigfe_pthread_mutexattr_setpshared:
        pushl   $_pthread_mutexattr_setpshared
        jmp     __sigfe

        .extern _pthread_mutexattr_settype
        .global __sigfe_pthread_mutexattr_settype
__sigfe_pthread_mutexattr_settype:
        pushl   $_pthread_mutexattr_settype
        jmp     __sigfe

        .extern _pthread_once
        .global __sigfe_pthread_once
__sigfe_pthread_once:
        pushl   $_pthread_once
        jmp     __sigfe

        .extern _pthread_rwlock_clockrdlock
        .global __sigfe_pthread_rwlock_clockrdlock
__sigfe_pthread_rwlock_clockrdlock:
        pushl   $_pthread_rwlock_clockrdlock
        jmp     __sigfe

        .extern _pthread_rwlock_clockwrlock
        .global __sigfe_pthread_rwlock_clockwrlock
__sigfe_pthread_rwlock_clockwrlock:
        pushl   $_pthread_rwlock_clockwrlock
        jmp     __sigfe

        .extern _pthread_rwlock_destroy
        .global __sigfe_pthread_rwlock_destroy
__sigfe_pthread_rwlock_destroy:
        pushl   $_pthread_rwlock_destroy
        jmp     __sigfe

        .extern _pthread_rwlock_init
        .global __sigfe_pthread_rwlock_init
__sigfe_pthread_rwlock_init:
        pushl   $_pthread_rwlock_init
        jmp     __sigfe

        .extern _pthread_rwlock_rdlock
        .global __sigfe_pthread_rwlock_rdlock
__sigfe_pthread_rwlock_rdlock:
        pushl   $_pthread_rwlock_rdlock
        jmp     __sigfe

        .extern _pthread_rwlock_timedrdlock
        .global __sigfe_pthread_rwlock_timedrdlock
__sigfe_pthread_rwlock_timedrdlock:
        pushl   $_pthread_rwlock_timedrdlock
        jmp     __sigfe

        .extern _pthread_rwlock_timedwrlock
        .global __sigfe_pthread_rwlock_timedwrlock
__sigfe_pthread_rwlock_timedwrlock:
        pushl   $_pthread_rwlock_timedwrlock
        jmp     __sigfe

        .extern _pthread_rwlock_tryrdlock
        .global __sigfe_pthread_rwlock_tryrdlock
__sigfe_pthread_rwlock_tryrdlock:
        pushl   $_pthread_rwlock_tryrdlock
        jmp     __sigfe

        .extern _pthread_rwlock_trywrlock
        .global __sigfe_pthread_rwlock_trywrlock
__sigfe_pthread_rwlock_trywrlock:
        pushl   $_pthread_rwlock_trywrlock
        jmp     __sigfe

        .extern _pthread_rwlock_unlock
        .global __sigfe_pthread_rwlock_unlock
__sigfe_pthread_rwlock_unlock:
        pushl   $_pthread_rwlock_unlock
        jmp     __sigfe

        .extern _pthread_rwlock_wrlock
        .global __sigfe_pthread_rwlock_wrlock
__sigfe_pthread_rwlock_wrlock:
        pushl   $_pthread_rwlock_wrlock
        jmp     __sigfe

        .extern _pthread_rwlockattr_destroy
        .global __sigfe_pthread_rwlockattr_destroy
__sigfe_pthread_rwlockattr_destroy:
        pushl   $_pthread_rwlockattr_destroy
        jmp     __sigfe

        .extern _pthread_rwlockattr_getpshared
        .global __sigfe_pthread_rwlockattr_getpshared
__sigfe_pthread_rwlockattr_getpshared:
        pushl   $_pthread_rwlockattr_getpshared
        jmp     __sigfe

        .extern _pthread_rwlockattr_init
        .global __sigfe_pthread_rwlockattr_init
__sigfe_pthread_rwlockattr_init:
        pushl   $_pthread_rwlockattr_init
        jmp     __sigfe

        .extern _pthread_rwlockattr_setpshared
        .global __sigfe_pthread_rwlockattr_setpshared
__sigfe_pthread_rwlockattr_setpshared:
        pushl   $_pthread_rwlockattr_setpshared
        jmp     __sigfe

        .extern _pthread_self
        .global __sigfe_pthread_self
__sigfe_pthread_self:
        pushl   $_pthread_self
        jmp     __sigfe

        .extern _pthread_setaffinity_np
        .global __sigfe_pthread_setaffinity_np
__sigfe_pthread_setaffinity_np:
        pushl   $_pthread_setaffinity_np
        jmp     __sigfe

        .extern _pthread_setcancelstate
        .global __sigfe_pthread_setcancelstate
__sigfe_pthread_setcancelstate:
        pushl   $_pthread_setcancelstate
        jmp     __sigfe

        .extern _pthread_setcanceltype
        .global __sigfe_pthread_setcanceltype
__sigfe_pthread_setcanceltype:
        pushl   $_pthread_setcanceltype
        jmp     __sigfe

        .extern _pthread_setconcurrency
        .global __sigfe_pthread_setconcurrency
__sigfe_pthread_setconcurrency:
        pushl   $_pthread_setconcurrency
        jmp     __sigfe

        .extern _pthread_setname_np
        .global __sigfe_pthread_setname_np
__sigfe_pthread_setname_np:
        pushl   $_pthread_setname_np
        jmp     __sigfe

        .extern _pthread_setschedparam
        .global __sigfe_pthread_setschedparam
__sigfe_pthread_setschedparam:
        pushl   $_pthread_setschedparam
        jmp     __sigfe

        .extern _pthread_setschedprio
        .global __sigfe_pthread_setschedprio
__sigfe_pthread_setschedprio:
        pushl   $_pthread_setschedprio
        jmp     __sigfe

        .extern _pthread_setspecific
        .global __sigfe_pthread_setspecific
__sigfe_pthread_setspecific:
        pushl   $_pthread_setspecific
        jmp     __sigfe

        .extern _pthread_sigmask
        .global __sigfe_pthread_sigmask
__sigfe_pthread_sigmask:
        pushl   $_pthread_sigmask
        jmp     __sigfe

        .extern _pthread_sigqueue
        .global __sigfe_pthread_sigqueue
__sigfe_pthread_sigqueue:
        pushl   $_pthread_sigqueue
        jmp     __sigfe

        .extern _pthread_spin_destroy
        .global __sigfe_pthread_spin_destroy
__sigfe_pthread_spin_destroy:
        pushl   $_pthread_spin_destroy
        jmp     __sigfe

        .extern _pthread_spin_init
        .global __sigfe_pthread_spin_init
__sigfe_pthread_spin_init:
        pushl   $_pthread_spin_init
        jmp     __sigfe

        .extern _pthread_spin_lock
        .global __sigfe_pthread_spin_lock
__sigfe_pthread_spin_lock:
        pushl   $_pthread_spin_lock
        jmp     __sigfe

        .extern _pthread_spin_trylock
        .global __sigfe_pthread_spin_trylock
__sigfe_pthread_spin_trylock:
        pushl   $_pthread_spin_trylock
        jmp     __sigfe

        .extern _pthread_spin_unlock
        .global __sigfe_pthread_spin_unlock
__sigfe_pthread_spin_unlock:
        pushl   $_pthread_spin_unlock
        jmp     __sigfe

        .extern _pthread_suspend
        .global __sigfe_pthread_suspend
__sigfe_pthread_suspend:
        pushl   $_pthread_suspend
        jmp     __sigfe

        .extern _pthread_testcancel
        .global __sigfe_pthread_testcancel
__sigfe_pthread_testcancel:
        pushl   $_pthread_testcancel
        jmp     __sigfe

        .extern _pthread_timedjoin_np
        .global __sigfe_pthread_timedjoin_np
__sigfe_pthread_timedjoin_np:
        pushl   $_pthread_timedjoin_np
        jmp     __sigfe

        .extern _pthread_tryjoin_np
        .global __sigfe_pthread_tryjoin_np
__sigfe_pthread_tryjoin_np:
        pushl   $_pthread_tryjoin_np
        jmp     __sigfe

        .extern _pthread_yield
        .global __sigfe_pthread_yield
__sigfe_pthread_yield:
        pushl   $_pthread_yield
        jmp     __sigfe

        .extern _ptsname
        .global __sigfe_ptsname
__sigfe_ptsname:
        pushl   $_ptsname
        jmp     __sigfe

        .extern _ptsname_r
        .global __sigfe_ptsname_r
__sigfe_ptsname_r:
        pushl   $_ptsname_r
        jmp     __sigfe

        .extern _putc
        .global __sigfe_putc
__sigfe_putc:
        pushl   $_putc
        jmp     __sigfe

        .extern _putc_unlocked
        .global __sigfe_putc_unlocked
__sigfe_putc_unlocked:
        pushl   $_putc_unlocked
        jmp     __sigfe

        .extern _putchar
        .global __sigfe_putchar
__sigfe_putchar:
        pushl   $_putchar
        jmp     __sigfe

        .extern _putchar_unlocked
        .global __sigfe_putchar_unlocked
__sigfe_putchar_unlocked:
        pushl   $_putchar_unlocked
        jmp     __sigfe

        .extern _putenv
        .global __sigfe_putenv
__sigfe_putenv:
        pushl   $_putenv
        jmp     __sigfe

        .extern _puts
        .global __sigfe_puts
__sigfe_puts:
        pushl   $_puts
        jmp     __sigfe

        .extern _pututline
        .global __sigfe_pututline
__sigfe_pututline:
        pushl   $_pututline
        jmp     __sigfe

        .extern _pututxline
        .global __sigfe_pututxline
__sigfe_pututxline:
        pushl   $_pututxline
        jmp     __sigfe

        .extern _putw
        .global __sigfe_putw
__sigfe_putw:
        pushl   $_putw
        jmp     __sigfe

        .extern _putwc
        .global __sigfe_putwc
__sigfe_putwc:
        pushl   $_putwc
        jmp     __sigfe

        .extern _putwc_unlocked
        .global __sigfe_putwc_unlocked
__sigfe_putwc_unlocked:
        pushl   $_putwc_unlocked
        jmp     __sigfe

        .extern _putwchar
        .global __sigfe_putwchar
__sigfe_putwchar:
        pushl   $_putwchar
        jmp     __sigfe

        .extern _putwchar_unlocked
        .global __sigfe_putwchar_unlocked
__sigfe_putwchar_unlocked:
        pushl   $_putwchar_unlocked
        jmp     __sigfe

        .extern _pwrite
        .global __sigfe_pwrite
__sigfe_pwrite:
        pushl   $_pwrite
        jmp     __sigfe

        .extern _quick_exit
        .global __sigfe_quick_exit
__sigfe_quick_exit:
        pushl   $_quick_exit
        jmp     __sigfe

        .extern _quotactl
        .global __sigfe_quotactl
__sigfe_quotactl:
        pushl   $_quotactl
        jmp     __sigfe

        .extern _raise
        .global __sigfe_raise
__sigfe_raise:
        pushl   $_raise
        jmp     __sigfe

        .extern _read
        .global __sigfe_read
__sigfe_read:
        pushl   $_read
        jmp     __sigfe

        .extern _readdir
        .global __sigfe_readdir
__sigfe_readdir:
        pushl   $_readdir
        jmp     __sigfe

        .extern _readdir_r
        .global __sigfe_readdir_r
__sigfe_readdir_r:
        pushl   $_readdir_r
        jmp     __sigfe

        .extern _readlink
        .global __sigfe_readlink
__sigfe_readlink:
        pushl   $_readlink
        jmp     __sigfe

        .extern _readlinkat
        .global __sigfe_readlinkat
__sigfe_readlinkat:
        pushl   $_readlinkat
        jmp     __sigfe

        .extern _readv
        .global __sigfe_readv
__sigfe_readv:
        pushl   $_readv
        jmp     __sigfe

        .extern _realloc
        .global __sigfe_realloc
__sigfe_realloc:
        pushl   $_realloc
        jmp     __sigfe

        .extern _reallocarray
        .global __sigfe_reallocarray
__sigfe_reallocarray:
        pushl   $_reallocarray
        jmp     __sigfe

        .extern _reallocf
        .global __sigfe_reallocf
__sigfe_reallocf:
        pushl   $_reallocf
        jmp     __sigfe

        .extern _realpath
        .global __sigfe_realpath
__sigfe_realpath:
        pushl   $_realpath
        jmp     __sigfe

        .extern _regcomp
        .global __sigfe_regcomp
__sigfe_regcomp:
        pushl   $_regcomp
        jmp     __sigfe

        .extern _regerror
        .global __sigfe_regerror
__sigfe_regerror:
        pushl   $_regerror
        jmp     __sigfe

        .extern _regexec
        .global __sigfe_regexec
__sigfe_regexec:
        pushl   $_regexec
        jmp     __sigfe

        .extern _regfree
        .global __sigfe_regfree
__sigfe_regfree:
        pushl   $_regfree
        jmp     __sigfe

        .extern _remove
        .global __sigfe_remove
__sigfe_remove:
        pushl   $_remove
        jmp     __sigfe

        .extern _removexattr
        .global __sigfe_removexattr
__sigfe_removexattr:
        pushl   $_removexattr
        jmp     __sigfe

        .extern _rename
        .global __sigfe_rename
__sigfe_rename:
        pushl   $_rename
        jmp     __sigfe

        .extern _renameat
        .global __sigfe_renameat
__sigfe_renameat:
        pushl   $_renameat
        jmp     __sigfe

        .extern _renameat2
        .global __sigfe_renameat2
__sigfe_renameat2:
        pushl   $_renameat2
        jmp     __sigfe

        .extern _revoke
        .global __sigfe_revoke
__sigfe_revoke:
        pushl   $_revoke
        jmp     __sigfe

        .extern _rewind
        .global __sigfe_rewind
__sigfe_rewind:
        pushl   $_rewind
        jmp     __sigfe

        .extern _rewinddir
        .global __sigfe_rewinddir
__sigfe_rewinddir:
        pushl   $_rewinddir
        jmp     __sigfe

        .extern _rmdir
        .global __sigfe_rmdir
__sigfe_rmdir:
        pushl   $_rmdir
        jmp     __sigfe

        .extern _rpmatch
        .global __sigfe_rpmatch
__sigfe_rpmatch:
        pushl   $_rpmatch
        jmp     __sigfe

        .extern _ruserok
        .global __sigfe_ruserok
__sigfe_ruserok:
        pushl   $_ruserok
        jmp     __sigfe

        .extern _sbrk
        .global __sigfe_sbrk
__sigfe_sbrk:
        pushl   $_sbrk
        jmp     __sigfe

        .extern _scandir
        .global __sigfe_scandir
__sigfe_scandir:
        pushl   $_scandir
        jmp     __sigfe

        .extern _scandirat
        .global __sigfe_scandirat
__sigfe_scandirat:
        pushl   $_scandirat
        jmp     __sigfe

        .extern _scanf
        .global __sigfe_scanf
__sigfe_scanf:
        pushl   $_scanf
        jmp     __sigfe

        .extern _sched_get_priority_max
        .global __sigfe_sched_get_priority_max
__sigfe_sched_get_priority_max:
        pushl   $_sched_get_priority_max
        jmp     __sigfe

        .extern _sched_get_priority_min
        .global __sigfe_sched_get_priority_min
__sigfe_sched_get_priority_min:
        pushl   $_sched_get_priority_min
        jmp     __sigfe

        .extern _sched_getaffinity
        .global __sigfe_sched_getaffinity
__sigfe_sched_getaffinity:
        pushl   $_sched_getaffinity
        jmp     __sigfe

        .extern _sched_getcpu
        .global __sigfe_sched_getcpu
__sigfe_sched_getcpu:
        pushl   $_sched_getcpu
        jmp     __sigfe

        .extern _sched_getparam
        .global __sigfe_sched_getparam
__sigfe_sched_getparam:
        pushl   $_sched_getparam
        jmp     __sigfe

        .extern _sched_rr_get_interval
        .global __sigfe_sched_rr_get_interval
__sigfe_sched_rr_get_interval:
        pushl   $_sched_rr_get_interval
        jmp     __sigfe

        .extern _sched_setaffinity
        .global __sigfe_sched_setaffinity
__sigfe_sched_setaffinity:
        pushl   $_sched_setaffinity
        jmp     __sigfe

        .extern _sched_setparam
        .global __sigfe_sched_setparam
__sigfe_sched_setparam:
        pushl   $_sched_setparam
        jmp     __sigfe

        .extern _sched_setscheduler
        .global __sigfe_sched_setscheduler
__sigfe_sched_setscheduler:
        pushl   $_sched_setscheduler
        jmp     __sigfe

        .extern _sched_yield
        .global __sigfe_sched_yield
__sigfe_sched_yield:
        pushl   $_sched_yield
        jmp     __sigfe

        .extern _seekdir
        .global __sigfe_seekdir
__sigfe_seekdir:
        pushl   $_seekdir
        jmp     __sigfe

        .extern _seekdir64
        .global __sigfe_seekdir64
__sigfe_seekdir64:
        pushl   $_seekdir64
        jmp     __sigfe

        .extern _sem_clockwait
        .global __sigfe_sem_clockwait
__sigfe_sem_clockwait:
        pushl   $_sem_clockwait
        jmp     __sigfe

        .extern _sem_close
        .global __sigfe_sem_close
__sigfe_sem_close:
        pushl   $_sem_close
        jmp     __sigfe

        .extern _sem_destroy
        .global __sigfe_sem_destroy
__sigfe_sem_destroy:
        pushl   $_sem_destroy
        jmp     __sigfe

        .extern _sem_getvalue
        .global __sigfe_sem_getvalue
__sigfe_sem_getvalue:
        pushl   $_sem_getvalue
        jmp     __sigfe

        .extern _sem_init
        .global __sigfe_sem_init
__sigfe_sem_init:
        pushl   $_sem_init
        jmp     __sigfe

        .extern _sem_open
        .global __sigfe_sem_open
__sigfe_sem_open:
        pushl   $_sem_open
        jmp     __sigfe

        .extern _sem_post
        .global __sigfe_sem_post
__sigfe_sem_post:
        pushl   $_sem_post
        jmp     __sigfe

        .extern _sem_timedwait
        .global __sigfe_sem_timedwait
__sigfe_sem_timedwait:
        pushl   $_sem_timedwait
        jmp     __sigfe

        .extern _sem_trywait
        .global __sigfe_sem_trywait
__sigfe_sem_trywait:
        pushl   $_sem_trywait
        jmp     __sigfe

        .extern _sem_unlink
        .global __sigfe_sem_unlink
__sigfe_sem_unlink:
        pushl   $_sem_unlink
        jmp     __sigfe

        .extern _sem_wait
        .global __sigfe_sem_wait
__sigfe_sem_wait:
        pushl   $_sem_wait
        jmp     __sigfe

        .extern _semctl
        .global __sigfe_semctl
__sigfe_semctl:
        pushl   $_semctl
        jmp     __sigfe

        .extern _semget
        .global __sigfe_semget
__sigfe_semget:
        pushl   $_semget
        jmp     __sigfe

        .extern _semop
        .global __sigfe_semop
__sigfe_semop:
        pushl   $_semop
        jmp     __sigfe

        .extern _setbuf
        .global __sigfe_setbuf
__sigfe_setbuf:
        pushl   $_setbuf
        jmp     __sigfe

        .extern _setbuffer
        .global __sigfe_setbuffer
__sigfe_setbuffer:
        pushl   $_setbuffer
        jmp     __sigfe

        .extern _setdtablesize
        .global __sigfe_setdtablesize
__sigfe_setdtablesize:
        pushl   $_setdtablesize
        jmp     __sigfe

        .extern _setegid
        .global __sigfe_setegid
__sigfe_setegid:
        pushl   $_setegid
        jmp     __sigfe

        .extern _setegid32
        .global __sigfe_setegid32
__sigfe_setegid32:
        pushl   $_setegid32
        jmp     __sigfe

        .extern _setenv
        .global __sigfe_setenv
__sigfe_setenv:
        pushl   $_setenv
        jmp     __sigfe

        .extern _seteuid
        .global __sigfe_seteuid
__sigfe_seteuid:
        pushl   $_seteuid
        jmp     __sigfe

        .extern _seteuid32
        .global __sigfe_seteuid32
__sigfe_seteuid32:
        pushl   $_seteuid32
        jmp     __sigfe

        .extern _setgid
        .global __sigfe_setgid
__sigfe_setgid:
        pushl   $_setgid
        jmp     __sigfe

        .extern _setgid32
        .global __sigfe_setgid32
__sigfe_setgid32:
        pushl   $_setgid32
        jmp     __sigfe

        .extern _setgroups
        .global __sigfe_setgroups
__sigfe_setgroups:
        pushl   $_setgroups
        jmp     __sigfe

        .extern _setgroups32
        .global __sigfe_setgroups32
__sigfe_setgroups32:
        pushl   $_setgroups32
        jmp     __sigfe

        .extern _sethostname
        .global __sigfe_sethostname
__sigfe_sethostname:
        pushl   $_sethostname
        jmp     __sigfe

        .extern _setitimer
        .global __sigfe_setitimer
__sigfe_setitimer:
        pushl   $_setitimer
        jmp     __sigfe

        .extern _setlinebuf
        .global __sigfe_setlinebuf
__sigfe_setlinebuf:
        pushl   $_setlinebuf
        jmp     __sigfe

        .extern _setmntent
        .global __sigfe_setmntent
__sigfe_setmntent:
        pushl   $_setmntent
        jmp     __sigfe

        .extern _setpgid
        .global __sigfe_setpgid
__sigfe_setpgid:
        pushl   $_setpgid
        jmp     __sigfe

        .extern _setpgrp
        .global __sigfe_setpgrp
__sigfe_setpgrp:
        pushl   $_setpgrp
        jmp     __sigfe

        .extern _setpriority
        .global __sigfe_setpriority
__sigfe_setpriority:
        pushl   $_setpriority
        jmp     __sigfe

        .extern _setregid
        .global __sigfe_setregid
__sigfe_setregid:
        pushl   $_setregid
        jmp     __sigfe

        .extern _setregid32
        .global __sigfe_setregid32
__sigfe_setregid32:
        pushl   $_setregid32
        jmp     __sigfe

        .extern _setreuid
        .global __sigfe_setreuid
__sigfe_setreuid:
        pushl   $_setreuid
        jmp     __sigfe

        .extern _setreuid32
        .global __sigfe_setreuid32
__sigfe_setreuid32:
        pushl   $_setreuid32
        jmp     __sigfe

        .extern _setrlimit
        .global __sigfe_setrlimit
__sigfe_setrlimit:
        pushl   $_setrlimit
        jmp     __sigfe

        .extern _setsid
        .global __sigfe_setsid
__sigfe_setsid:
        pushl   $_setsid
        jmp     __sigfe

        .extern _settimeofday
        .global __sigfe_settimeofday
__sigfe_settimeofday:
        pushl   $_settimeofday
        jmp     __sigfe

        .extern _setuid
        .global __sigfe_setuid
__sigfe_setuid:
        pushl   $_setuid
        jmp     __sigfe

        .extern _setuid32
        .global __sigfe_setuid32
__sigfe_setuid32:
        pushl   $_setuid32
        jmp     __sigfe

        .extern _setusershell
        .global __sigfe_setusershell
__sigfe_setusershell:
        pushl   $_setusershell
        jmp     __sigfe

        .extern _setutent
        .global __sigfe_setutent
__sigfe_setutent:
        pushl   $_setutent
        jmp     __sigfe

        .extern _setutxent
        .global __sigfe_setutxent
__sigfe_setutxent:
        pushl   $_setutxent
        jmp     __sigfe

        .extern _setvbuf
        .global __sigfe_setvbuf
__sigfe_setvbuf:
        pushl   $_setvbuf
        jmp     __sigfe

        .extern _setxattr
        .global __sigfe_setxattr
__sigfe_setxattr:
        pushl   $_setxattr
        jmp     __sigfe

        .extern _sexecve_is_bad
        .global __sigfe_sexecve_is_bad
__sigfe_sexecve_is_bad:
        pushl   $_sexecve_is_bad
        jmp     __sigfe

        .extern _shm_open
        .global __sigfe_shm_open
__sigfe_shm_open:
        pushl   $_shm_open
        jmp     __sigfe

        .extern _shm_unlink
        .global __sigfe_shm_unlink
__sigfe_shm_unlink:
        pushl   $_shm_unlink
        jmp     __sigfe

        .extern _shmat
        .global __sigfe_shmat
__sigfe_shmat:
        pushl   $_shmat
        jmp     __sigfe

        .extern _shmctl
        .global __sigfe_shmctl
__sigfe_shmctl:
        pushl   $_shmctl
        jmp     __sigfe

        .extern _shmdt
        .global __sigfe_shmdt
__sigfe_shmdt:
        pushl   $_shmdt
        jmp     __sigfe

        .extern _shmget
        .global __sigfe_shmget
__sigfe_shmget:
        pushl   $_shmget
        jmp     __sigfe

        .extern _sig2str
        .global __sigfe_sig2str
__sigfe_sig2str:
        pushl   $_sig2str
        jmp     __sigfe

        .extern _sigaction
        .global __sigfe_sigaction
__sigfe_sigaction:
        pushl   $_sigaction
        jmp     __sigfe

        .extern _sigaddset
        .global __sigfe_sigaddset
__sigfe_sigaddset:
        pushl   $_sigaddset
        jmp     __sigfe

        .extern _sigaltstack
        .global __sigfe_sigaltstack
__sigfe_sigaltstack:
        pushl   $_sigaltstack
        jmp     __sigfe

        .extern _sigdelset
        .global __sigfe_sigdelset
__sigfe_sigdelset:
        pushl   $_sigdelset
        jmp     __sigfe

        .extern _sighold
        .global __sigfe_sighold
__sigfe_sighold:
        pushl   $_sighold
        jmp     __sigfe

        .extern _sigignore
        .global __sigfe_sigignore
__sigfe_sigignore:
        pushl   $_sigignore
        jmp     __sigfe

        .extern _siginterrupt
        .global __sigfe_siginterrupt
__sigfe_siginterrupt:
        pushl   $_siginterrupt
        jmp     __sigfe

        .extern _sigismember
        .global __sigfe_sigismember
__sigfe_sigismember:
        pushl   $_sigismember
        jmp     __sigfe

        .extern _signal
        .global __sigfe_signal
__sigfe_signal:
        pushl   $_signal
        jmp     __sigfe

        .extern _signalfd
        .global __sigfe_signalfd
__sigfe_signalfd:
        pushl   $_signalfd
        jmp     __sigfe

        .extern _sigpause
        .global __sigfe_sigpause
__sigfe_sigpause:
        pushl   $_sigpause
        jmp     __sigfe

        .extern _sigpending
        .global __sigfe_sigpending
__sigfe_sigpending:
        pushl   $_sigpending
        jmp     __sigfe

        .extern _sigprocmask
        .global __sigfe_sigprocmask
__sigfe_sigprocmask:
        pushl   $_sigprocmask
        jmp     __sigfe

        .extern _sigqueue
        .global __sigfe_sigqueue
__sigfe_sigqueue:
        pushl   $_sigqueue
        jmp     __sigfe

        .extern _sigrelse
        .global __sigfe_sigrelse
__sigfe_sigrelse:
        pushl   $_sigrelse
        jmp     __sigfe

        .extern _sigset
        .global __sigfe_sigset
__sigfe_sigset:
        pushl   $_sigset
        jmp     __sigfe

        .extern _sigsuspend
        .global __sigfe_sigsuspend
__sigfe_sigsuspend:
        pushl   $_sigsuspend
        jmp     __sigfe

        .extern _sigtimedwait
        .global __sigfe_sigtimedwait
__sigfe_sigtimedwait:
        pushl   $_sigtimedwait
        jmp     __sigfe

        .extern _sigwait
        .global __sigfe_sigwait
__sigfe_sigwait:
        pushl   $_sigwait
        jmp     __sigfe

        .extern _sigwaitinfo
        .global __sigfe_sigwaitinfo
__sigfe_sigwaitinfo:
        pushl   $_sigwaitinfo
        jmp     __sigfe

        .extern _siprintf
        .global __sigfe_siprintf
__sigfe_siprintf:
        pushl   $_siprintf
        jmp     __sigfe

        .extern _sleep
        .global __sigfe_sleep
__sigfe_sleep:
        pushl   $_sleep
        jmp     __sigfe

        .extern _snprintf
        .global __sigfe_snprintf
__sigfe_snprintf:
        pushl   $_snprintf
        jmp     __sigfe

        .extern _sockatmark
        .global __sigfe_sockatmark
__sigfe_sockatmark:
        pushl   $_sockatmark
        jmp     __sigfe

        .extern _socketpair
        .global __sigfe_socketpair
__sigfe_socketpair:
        pushl   $_socketpair
        jmp     __sigfe

        .extern _spawnl
        .global __sigfe_spawnl
__sigfe_spawnl:
        pushl   $_spawnl
        jmp     __sigfe

        .extern _spawnle
        .global __sigfe_spawnle
__sigfe_spawnle:
        pushl   $_spawnle
        jmp     __sigfe

        .extern _spawnlp
        .global __sigfe_spawnlp
__sigfe_spawnlp:
        pushl   $_spawnlp
        jmp     __sigfe

        .extern _spawnlpe
        .global __sigfe_spawnlpe
__sigfe_spawnlpe:
        pushl   $_spawnlpe
        jmp     __sigfe

        .extern _spawnv
        .global __sigfe_spawnv
__sigfe_spawnv:
        pushl   $_spawnv
        jmp     __sigfe

        .extern _spawnve
        .global __sigfe_spawnve
__sigfe_spawnve:
        pushl   $_spawnve
        jmp     __sigfe

        .extern _spawnvp
        .global __sigfe_spawnvp
__sigfe_spawnvp:
        pushl   $_spawnvp
        jmp     __sigfe

        .extern _spawnvpe
        .global __sigfe_spawnvpe
__sigfe_spawnvpe:
        pushl   $_spawnvpe
        jmp     __sigfe

        .extern _sprintf
        .global __sigfe_sprintf
__sigfe_sprintf:
        pushl   $_sprintf
        jmp     __sigfe

        .extern _sscanf
        .global __sigfe_sscanf
__sigfe_sscanf:
        pushl   $_sscanf
        jmp     __sigfe

        .extern _stat
        .global __sigfe_stat
__sigfe_stat:
        pushl   $_stat
        jmp     __sigfe

        .extern _stat64
        .global __sigfe_stat64
__sigfe_stat64:
        pushl   $_stat64
        jmp     __sigfe

        .extern _statfs
        .global __sigfe_statfs
__sigfe_statfs:
        pushl   $_statfs
        jmp     __sigfe

        .extern _statvfs
        .global __sigfe_statvfs
__sigfe_statvfs:
        pushl   $_statvfs
        jmp     __sigfe

        .extern _stime
        .global __sigfe_stime
__sigfe_stime:
        pushl   $_stime
        jmp     __sigfe

        .extern _str2sig
        .global __sigfe_str2sig
__sigfe_str2sig:
        pushl   $_str2sig
        jmp     __sigfe

        .extern _strdup
        .global __sigfe_strdup
__sigfe_strdup:
        pushl   $_strdup
        jmp     __sigfe

        .extern _strerror
        .global __sigfe_strerror
__sigfe_strerror:
        pushl   $_strerror
        jmp     __sigfe

        .extern _strerror_l
        .global __sigfe_strerror_l
__sigfe_strerror_l:
        pushl   $_strerror_l
        jmp     __sigfe

        .extern _strerror_r
        .global __sigfe_strerror_r
__sigfe_strerror_r:
        pushl   $_strerror_r
        jmp     __sigfe

        .extern _strfmon
        .global __sigfe_strfmon
__sigfe_strfmon:
        pushl   $_strfmon
        jmp     __sigfe

        .extern _strfmon_l
        .global __sigfe_strfmon_l
__sigfe_strfmon_l:
        pushl   $_strfmon_l
        jmp     __sigfe

        .extern _strftime
        .global __sigfe_strftime
__sigfe_strftime:
        pushl   $_strftime
        jmp     __sigfe

        .extern _strftime_l
        .global __sigfe_strftime_l
__sigfe_strftime_l:
        pushl   $_strftime_l
        jmp     __sigfe

        .extern _strndup
        .global __sigfe_strndup
__sigfe_strndup:
        pushl   $_strndup
        jmp     __sigfe

        .extern _strptime
        .global __sigfe_strptime
__sigfe_strptime:
        pushl   $_strptime
        jmp     __sigfe

        .extern _strptime_l
        .global __sigfe_strptime_l
__sigfe_strptime_l:
        pushl   $_strptime_l
        jmp     __sigfe

        .extern _strsignal
        .global __sigfe_strsignal
__sigfe_strsignal:
        pushl   $_strsignal
        jmp     __sigfe

        .extern _strtod
        .global __sigfe_strtod
__sigfe_strtod:
        pushl   $_strtod
        jmp     __sigfe

        .extern _strtod_l
        .global __sigfe_strtod_l
__sigfe_strtod_l:
        pushl   $_strtod_l
        jmp     __sigfe

        .extern _strtof
        .global __sigfe_strtof
__sigfe_strtof:
        pushl   $_strtof
        jmp     __sigfe

        .extern _strtof_l
        .global __sigfe_strtof_l
__sigfe_strtof_l:
        pushl   $_strtof_l
        jmp     __sigfe

        .extern _strtold
        .global __sigfe_strtold
__sigfe_strtold:
        pushl   $_strtold
        jmp     __sigfe

        .extern _strtold_l
        .global __sigfe_strtold_l
__sigfe_strtold_l:
        pushl   $_strtold_l
        jmp     __sigfe

        .extern _swprintf
        .global __sigfe_swprintf
__sigfe_swprintf:
        pushl   $_swprintf
        jmp     __sigfe

        .extern _swscanf
        .global __sigfe_swscanf
__sigfe_swscanf:
        pushl   $_swscanf
        jmp     __sigfe

        .extern _symlink
        .global __sigfe_symlink
__sigfe_symlink:
        pushl   $_symlink
        jmp     __sigfe

        .extern _symlinkat
        .global __sigfe_symlinkat
__sigfe_symlinkat:
        pushl   $_symlinkat
        jmp     __sigfe

        .extern _sync
        .global __sigfe_sync
__sigfe_sync:
        pushl   $_sync
        jmp     __sigfe

        .extern _sysconf
        .global __sigfe_sysconf
__sigfe_sysconf:
        pushl   $_sysconf
        jmp     __sigfe

        .extern _sysinfo
        .global __sigfe_sysinfo
__sigfe_sysinfo:
        pushl   $_sysinfo
        jmp     __sigfe

        .extern _syslog
        .global __sigfe_syslog
__sigfe_syslog:
        pushl   $_syslog
        jmp     __sigfe

        .extern _system
        .global __sigfe_system
__sigfe_system:
        pushl   $_system
        jmp     __sigfe

        .extern _tcdrain
        .global __sigfe_tcdrain
__sigfe_tcdrain:
        pushl   $_tcdrain
        jmp     __sigfe

        .extern _tcflow
        .global __sigfe_tcflow
__sigfe_tcflow:
        pushl   $_tcflow
        jmp     __sigfe

        .extern _tcflush
        .global __sigfe_tcflush
__sigfe_tcflush:
        pushl   $_tcflush
        jmp     __sigfe

        .extern _tcgetattr
        .global __sigfe_tcgetattr
__sigfe_tcgetattr:
        pushl   $_tcgetattr
        jmp     __sigfe

        .extern _tcgetpgrp
        .global __sigfe_tcgetpgrp
__sigfe_tcgetpgrp:
        pushl   $_tcgetpgrp
        jmp     __sigfe

        .extern _tcgetsid
        .global __sigfe_tcgetsid
__sigfe_tcgetsid:
        pushl   $_tcgetsid
        jmp     __sigfe

        .extern _tcsendbreak
        .global __sigfe_tcsendbreak
__sigfe_tcsendbreak:
        pushl   $_tcsendbreak
        jmp     __sigfe

        .extern _tcsetattr
        .global __sigfe_tcsetattr
__sigfe_tcsetattr:
        pushl   $_tcsetattr
        jmp     __sigfe

        .extern _tcsetpgrp
        .global __sigfe_tcsetpgrp
__sigfe_tcsetpgrp:
        pushl   $_tcsetpgrp
        jmp     __sigfe

        .extern _tdelete
        .global __sigfe_tdelete
__sigfe_tdelete:
        pushl   $_tdelete
        jmp     __sigfe

        .extern _telldir
        .global __sigfe_telldir
__sigfe_telldir:
        pushl   $_telldir
        jmp     __sigfe

        .extern _telldir64
        .global __sigfe_telldir64
__sigfe_telldir64:
        pushl   $_telldir64
        jmp     __sigfe

        .extern _tempnam
        .global __sigfe_tempnam
__sigfe_tempnam:
        pushl   $_tempnam
        jmp     __sigfe

        .extern _thrd_create
        .global __sigfe_thrd_create
__sigfe_thrd_create:
        pushl   $_thrd_create
        jmp     __sigfe

        .extern _thrd_current
        .global __sigfe_thrd_current
__sigfe_thrd_current:
        pushl   $_thrd_current
        jmp     __sigfe

        .extern _thrd_detach
        .global __sigfe_thrd_detach
__sigfe_thrd_detach:
        pushl   $_thrd_detach
        jmp     __sigfe

        .extern _thrd_equal
        .global __sigfe_thrd_equal
__sigfe_thrd_equal:
        pushl   $_thrd_equal
        jmp     __sigfe

        .extern _thrd_exit
        .global __sigfe_thrd_exit
__sigfe_thrd_exit:
        pushl   $_thrd_exit
        jmp     __sigfe

        .extern _thrd_join
        .global __sigfe_thrd_join
__sigfe_thrd_join:
        pushl   $_thrd_join
        jmp     __sigfe

        .extern _thrd_sleep
        .global __sigfe_thrd_sleep
__sigfe_thrd_sleep:
        pushl   $_thrd_sleep
        jmp     __sigfe

        .extern _thrd_yield
        .global __sigfe_thrd_yield
__sigfe_thrd_yield:
        pushl   $_thrd_yield
        jmp     __sigfe

        .extern _time
        .global __sigfe_time
__sigfe_time:
        pushl   $_time
        jmp     __sigfe

        .extern _timelocal
        .global __sigfe_timelocal
__sigfe_timelocal:
        pushl   $_timelocal
        jmp     __sigfe

        .extern _timer_create
        .global __sigfe_timer_create
__sigfe_timer_create:
        pushl   $_timer_create
        jmp     __sigfe

        .extern _timer_delete
        .global __sigfe_timer_delete
__sigfe_timer_delete:
        pushl   $_timer_delete
        jmp     __sigfe

        .extern _timer_getoverrun
        .global __sigfe_timer_getoverrun
__sigfe_timer_getoverrun:
        pushl   $_timer_getoverrun
        jmp     __sigfe

        .extern _timer_gettime
        .global __sigfe_timer_gettime
__sigfe_timer_gettime:
        pushl   $_timer_gettime
        jmp     __sigfe

        .extern _timer_settime
        .global __sigfe_timer_settime
__sigfe_timer_settime:
        pushl   $_timer_settime
        jmp     __sigfe

        .extern _timerfd_create
        .global __sigfe_timerfd_create
__sigfe_timerfd_create:
        pushl   $_timerfd_create
        jmp     __sigfe

        .extern _timerfd_gettime
        .global __sigfe_timerfd_gettime
__sigfe_timerfd_gettime:
        pushl   $_timerfd_gettime
        jmp     __sigfe

        .extern _timerfd_settime
        .global __sigfe_timerfd_settime
__sigfe_timerfd_settime:
        pushl   $_timerfd_settime
        jmp     __sigfe

        .extern _times
        .global __sigfe_times
__sigfe_times:
        pushl   $_times
        jmp     __sigfe

        .extern _timezone
        .global __sigfe_timezone
__sigfe_timezone:
        pushl   $_timezone
        jmp     __sigfe

        .extern _tmpfile
        .global __sigfe_tmpfile
__sigfe_tmpfile:
        pushl   $_tmpfile
        jmp     __sigfe

        .extern _tmpfile64
        .global __sigfe_tmpfile64
__sigfe_tmpfile64:
        pushl   $_tmpfile64
        jmp     __sigfe

        .extern _tmpnam
        .global __sigfe_tmpnam
__sigfe_tmpnam:
        pushl   $_tmpnam
        jmp     __sigfe

        .extern _truncate
        .global __sigfe_truncate
__sigfe_truncate:
        pushl   $_truncate
        jmp     __sigfe

        .extern _truncate64
        .global __sigfe_truncate64
__sigfe_truncate64:
        pushl   $_truncate64
        jmp     __sigfe

        .extern _tsearch
        .global __sigfe_tsearch
__sigfe_tsearch:
        pushl   $_tsearch
        jmp     __sigfe

        .extern _tss_create
        .global __sigfe_tss_create
__sigfe_tss_create:
        pushl   $_tss_create
        jmp     __sigfe

        .extern _tss_delete
        .global __sigfe_tss_delete
__sigfe_tss_delete:
        pushl   $_tss_delete
        jmp     __sigfe

        .extern _tss_get
        .global __sigfe_tss_get
__sigfe_tss_get:
        pushl   $_tss_get
        jmp     __sigfe

        .extern _tss_set
        .global __sigfe_tss_set
__sigfe_tss_set:
        pushl   $_tss_set
        jmp     __sigfe

        .extern _ttyname
        .global __sigfe_ttyname
__sigfe_ttyname:
        pushl   $_ttyname
        jmp     __sigfe

        .extern _ttyname_r
        .global __sigfe_ttyname_r
__sigfe_ttyname_r:
        pushl   $_ttyname_r
        jmp     __sigfe

        .extern _tzset
        .global __sigfe_tzset
__sigfe_tzset:
        pushl   $_tzset
        jmp     __sigfe

        .extern _ualarm
        .global __sigfe_ualarm
__sigfe_ualarm:
        pushl   $_ualarm
        jmp     __sigfe

        .extern _umount
        .global __sigfe_umount
__sigfe_umount:
        pushl   $_umount
        jmp     __sigfe

        .extern _uname
        .global __sigfe_uname
__sigfe_uname:
        pushl   $_uname
        jmp     __sigfe

        .extern _uname_x
        .global __sigfe_uname_x
__sigfe_uname_x:
        pushl   $_uname_x
        jmp     __sigfe

        .extern _ungetc
        .global __sigfe_ungetc
__sigfe_ungetc:
        pushl   $_ungetc
        jmp     __sigfe

        .extern _ungetwc
        .global __sigfe_ungetwc
__sigfe_ungetwc:
        pushl   $_ungetwc
        jmp     __sigfe

        .extern _unlink
        .global __sigfe_unlink
__sigfe_unlink:
        pushl   $_unlink
        jmp     __sigfe

        .extern _unlinkat
        .global __sigfe_unlinkat
__sigfe_unlinkat:
        pushl   $_unlinkat
        jmp     __sigfe

        .extern _unsetenv
        .global __sigfe_unsetenv
__sigfe_unsetenv:
        pushl   $_unsetenv
        jmp     __sigfe

        .extern _updwtmp
        .global __sigfe_updwtmp
__sigfe_updwtmp:
        pushl   $_updwtmp
        jmp     __sigfe

        .extern _updwtmpx
        .global __sigfe_updwtmpx
__sigfe_updwtmpx:
        pushl   $_updwtmpx
        jmp     __sigfe

        .extern _uselocale
        .global __sigfe_uselocale
__sigfe_uselocale:
        pushl   $_uselocale
        jmp     __sigfe

        .extern _usleep
        .global __sigfe_usleep
__sigfe_usleep:
        pushl   $_usleep
        jmp     __sigfe

        .extern _utime
        .global __sigfe_utime
__sigfe_utime:
        pushl   $_utime
        jmp     __sigfe

        .extern _utimensat
        .global __sigfe_utimensat
__sigfe_utimensat:
        pushl   $_utimensat
        jmp     __sigfe

        .extern _utimes
        .global __sigfe_utimes
__sigfe_utimes:
        pushl   $_utimes
        jmp     __sigfe

        .extern _utmpname
        .global __sigfe_utmpname
__sigfe_utmpname:
        pushl   $_utmpname
        jmp     __sigfe

        .extern _utmpxname
        .global __sigfe_utmpxname
__sigfe_utmpxname:
        pushl   $_utmpxname
        jmp     __sigfe

        .extern _valloc
        .global __sigfe_valloc
__sigfe_valloc:
        pushl   $_valloc
        jmp     __sigfe

        .extern _vasnprintf
        .global __sigfe_vasnprintf
__sigfe_vasnprintf:
        pushl   $_vasnprintf
        jmp     __sigfe

        .extern _vasprintf
        .global __sigfe_vasprintf
__sigfe_vasprintf:
        pushl   $_vasprintf
        jmp     __sigfe

        .extern _vdprintf
        .global __sigfe_vdprintf
__sigfe_vdprintf:
        pushl   $_vdprintf
        jmp     __sigfe

        .extern _verr
        .global __sigfe_verr
__sigfe_verr:
        pushl   $_verr
        jmp     __sigfe

        .extern _verrx
        .global __sigfe_verrx
__sigfe_verrx:
        pushl   $_verrx
        jmp     __sigfe

        .extern _vfiprintf
        .global __sigfe_vfiprintf
__sigfe_vfiprintf:
        pushl   $_vfiprintf
        jmp     __sigfe

        .extern _vfork
        .global __sigfe_vfork
__sigfe_vfork:
        pushl   $_vfork
        jmp     __sigfe

        .extern _vfprintf
        .global __sigfe_vfprintf
__sigfe_vfprintf:
        pushl   $_vfprintf
        jmp     __sigfe

        .extern _vfscanf
        .global __sigfe_vfscanf
__sigfe_vfscanf:
        pushl   $_vfscanf
        jmp     __sigfe

        .extern _vfwprintf
        .global __sigfe_vfwprintf
__sigfe_vfwprintf:
        pushl   $_vfwprintf
        jmp     __sigfe

        .extern _vfwscanf
        .global __sigfe_vfwscanf
__sigfe_vfwscanf:
        pushl   $_vfwscanf
        jmp     __sigfe

        .extern _vhangup
        .global __sigfe_vhangup
__sigfe_vhangup:
        pushl   $_vhangup
        jmp     __sigfe

        .extern _vprintf
        .global __sigfe_vprintf
__sigfe_vprintf:
        pushl   $_vprintf
        jmp     __sigfe

        .extern _vscanf
        .global __sigfe_vscanf
__sigfe_vscanf:
        pushl   $_vscanf
        jmp     __sigfe

        .extern _vsnprintf
        .global __sigfe_vsnprintf
__sigfe_vsnprintf:
        pushl   $_vsnprintf
        jmp     __sigfe

        .extern _vsprintf
        .global __sigfe_vsprintf
__sigfe_vsprintf:
        pushl   $_vsprintf
        jmp     __sigfe

        .extern _vsscanf
        .global __sigfe_vsscanf
__sigfe_vsscanf:
        pushl   $_vsscanf
        jmp     __sigfe

        .extern _vswprintf
        .global __sigfe_vswprintf
__sigfe_vswprintf:
        pushl   $_vswprintf
        jmp     __sigfe

        .extern _vswscanf
        .global __sigfe_vswscanf
__sigfe_vswscanf:
        pushl   $_vswscanf
        jmp     __sigfe

        .extern _vsyslog
        .global __sigfe_vsyslog
__sigfe_vsyslog:
        pushl   $_vsyslog
        jmp     __sigfe

        .extern _vwarn
        .global __sigfe_vwarn
__sigfe_vwarn:
        pushl   $_vwarn
        jmp     __sigfe

        .extern _vwarnx
        .global __sigfe_vwarnx
__sigfe_vwarnx:
        pushl   $_vwarnx
        jmp     __sigfe

        .extern _vwprintf
        .global __sigfe_vwprintf
__sigfe_vwprintf:
        pushl   $_vwprintf
        jmp     __sigfe

        .extern _vwscanf
        .global __sigfe_vwscanf
__sigfe_vwscanf:
        pushl   $_vwscanf
        jmp     __sigfe

        .extern _wait
        .global __sigfe_wait
__sigfe_wait:
        pushl   $_wait
        jmp     __sigfe

        .extern _wait3
        .global __sigfe_wait3
__sigfe_wait3:
        pushl   $_wait3
        jmp     __sigfe

        .extern _wait4
        .global __sigfe_wait4
__sigfe_wait4:
        pushl   $_wait4
        jmp     __sigfe

        .extern _waitpid
        .global __sigfe_waitpid
__sigfe_waitpid:
        pushl   $_waitpid
        jmp     __sigfe

        .extern _warn
        .global __sigfe_warn
__sigfe_warn:
        pushl   $_warn
        jmp     __sigfe

        .extern _warnx
        .global __sigfe_warnx
__sigfe_warnx:
        pushl   $_warnx
        jmp     __sigfe

        .extern _wcsftime
        .global __sigfe_wcsftime
__sigfe_wcsftime:
        pushl   $_wcsftime
        jmp     __sigfe

        .extern _wcsftime_l
        .global __sigfe_wcsftime_l
__sigfe_wcsftime_l:
        pushl   $_wcsftime_l
        jmp     __sigfe

        .extern _wprintf
        .global __sigfe_wprintf
__sigfe_wprintf:
        pushl   $_wprintf
        jmp     __sigfe

        .extern _write
        .global __sigfe_write
__sigfe_write:
        pushl   $_write
        jmp     __sigfe

        .extern _writev
        .global __sigfe_writev
__sigfe_writev:
        pushl   $_writev
        jmp     __sigfe

        .extern _wscanf
        .global __sigfe_wscanf
__sigfe_wscanf:
        pushl   $_wscanf
        jmp     __sigfe

        .extern _xdr_array
        .global __sigfe_xdr_array
__sigfe_xdr_array:
        pushl   $_xdr_array
        jmp     __sigfe

        .extern _xdr_bool
        .global __sigfe_xdr_bool
__sigfe_xdr_bool:
        pushl   $_xdr_bool
        jmp     __sigfe

        .extern _xdr_bytes
        .global __sigfe_xdr_bytes
__sigfe_xdr_bytes:
        pushl   $_xdr_bytes
        jmp     __sigfe

        .extern _xdr_char
        .global __sigfe_xdr_char
__sigfe_xdr_char:
        pushl   $_xdr_char
        jmp     __sigfe

        .extern _xdr_double
        .global __sigfe_xdr_double
__sigfe_xdr_double:
        pushl   $_xdr_double
        jmp     __sigfe

        .extern _xdr_enum
        .global __sigfe_xdr_enum
__sigfe_xdr_enum:
        pushl   $_xdr_enum
        jmp     __sigfe

        .extern _xdr_float
        .global __sigfe_xdr_float
__sigfe_xdr_float:
        pushl   $_xdr_float
        jmp     __sigfe

        .extern _xdr_free
        .global __sigfe_xdr_free
__sigfe_xdr_free:
        pushl   $_xdr_free
        jmp     __sigfe

        .extern _xdr_hyper
        .global __sigfe_xdr_hyper
__sigfe_xdr_hyper:
        pushl   $_xdr_hyper
        jmp     __sigfe

        .extern _xdr_int
        .global __sigfe_xdr_int
__sigfe_xdr_int:
        pushl   $_xdr_int
        jmp     __sigfe

        .extern _xdr_int16_t
        .global __sigfe_xdr_int16_t
__sigfe_xdr_int16_t:
        pushl   $_xdr_int16_t
        jmp     __sigfe

        .extern _xdr_int32_t
        .global __sigfe_xdr_int32_t
__sigfe_xdr_int32_t:
        pushl   $_xdr_int32_t
        jmp     __sigfe

        .extern _xdr_int64_t
        .global __sigfe_xdr_int64_t
__sigfe_xdr_int64_t:
        pushl   $_xdr_int64_t
        jmp     __sigfe

        .extern _xdr_int8_t
        .global __sigfe_xdr_int8_t
__sigfe_xdr_int8_t:
        pushl   $_xdr_int8_t
        jmp     __sigfe

        .extern _xdr_long
        .global __sigfe_xdr_long
__sigfe_xdr_long:
        pushl   $_xdr_long
        jmp     __sigfe

        .extern _xdr_longlong_t
        .global __sigfe_xdr_longlong_t
__sigfe_xdr_longlong_t:
        pushl   $_xdr_longlong_t
        jmp     __sigfe

        .extern _xdr_netobj
        .global __sigfe_xdr_netobj
__sigfe_xdr_netobj:
        pushl   $_xdr_netobj
        jmp     __sigfe

        .extern _xdr_opaque
        .global __sigfe_xdr_opaque
__sigfe_xdr_opaque:
        pushl   $_xdr_opaque
        jmp     __sigfe

        .extern _xdr_pointer
        .global __sigfe_xdr_pointer
__sigfe_xdr_pointer:
        pushl   $_xdr_pointer
        jmp     __sigfe

        .extern _xdr_reference
        .global __sigfe_xdr_reference
__sigfe_xdr_reference:
        pushl   $_xdr_reference
        jmp     __sigfe

        .extern _xdr_short
        .global __sigfe_xdr_short
__sigfe_xdr_short:
        pushl   $_xdr_short
        jmp     __sigfe

        .extern _xdr_sizeof
        .global __sigfe_xdr_sizeof
__sigfe_xdr_sizeof:
        pushl   $_xdr_sizeof
        jmp     __sigfe

        .extern _xdr_string
        .global __sigfe_xdr_string
__sigfe_xdr_string:
        pushl   $_xdr_string
        jmp     __sigfe

        .extern _xdr_u_char
        .global __sigfe_xdr_u_char
__sigfe_xdr_u_char:
        pushl   $_xdr_u_char
        jmp     __sigfe

        .extern _xdr_u_hyper
        .global __sigfe_xdr_u_hyper
__sigfe_xdr_u_hyper:
        pushl   $_xdr_u_hyper
        jmp     __sigfe

        .extern _xdr_u_int
        .global __sigfe_xdr_u_int
__sigfe_xdr_u_int:
        pushl   $_xdr_u_int
        jmp     __sigfe

        .extern _xdr_u_int16_t
        .global __sigfe_xdr_u_int16_t
__sigfe_xdr_u_int16_t:
        pushl   $_xdr_u_int16_t
        jmp     __sigfe

        .extern _xdr_u_int32_t
        .global __sigfe_xdr_u_int32_t
__sigfe_xdr_u_int32_t:
        pushl   $_xdr_u_int32_t
        jmp     __sigfe

        .extern _xdr_u_int64_t
        .global __sigfe_xdr_u_int64_t
__sigfe_xdr_u_int64_t:
        pushl   $_xdr_u_int64_t
        jmp     __sigfe

        .extern _xdr_u_int8_t
        .global __sigfe_xdr_u_int8_t
__sigfe_xdr_u_int8_t:
        pushl   $_xdr_u_int8_t
        jmp     __sigfe

        .extern _xdr_u_long
        .global __sigfe_xdr_u_long
__sigfe_xdr_u_long:
        pushl   $_xdr_u_long
        jmp     __sigfe

        .extern _xdr_u_longlong_t
        .global __sigfe_xdr_u_longlong_t
__sigfe_xdr_u_longlong_t:
        pushl   $_xdr_u_longlong_t
        jmp     __sigfe

        .extern _xdr_u_short
        .global __sigfe_xdr_u_short
__sigfe_xdr_u_short:
        pushl   $_xdr_u_short
        jmp     __sigfe

        .extern _xdr_uint16_t
        .global __sigfe_xdr_uint16_t
__sigfe_xdr_uint16_t:
        pushl   $_xdr_uint16_t
        jmp     __sigfe

        .extern _xdr_uint32_t
        .global __sigfe_xdr_uint32_t
__sigfe_xdr_uint32_t:
        pushl   $_xdr_uint32_t
        jmp     __sigfe

        .extern _xdr_uint64_t
        .global __sigfe_xdr_uint64_t
__sigfe_xdr_uint64_t:
        pushl   $_xdr_uint64_t
        jmp     __sigfe

        .extern _xdr_uint8_t
        .global __sigfe_xdr_uint8_t
__sigfe_xdr_uint8_t:
        pushl   $_xdr_uint8_t
        jmp     __sigfe

        .extern _xdr_union
        .global __sigfe_xdr_union
__sigfe_xdr_union:
        pushl   $_xdr_union
        jmp     __sigfe

        .extern _xdr_vector
        .global __sigfe_xdr_vector
__sigfe_xdr_vector:
        pushl   $_xdr_vector
        jmp     __sigfe

        .extern _xdr_void
        .global __sigfe_xdr_void
__sigfe_xdr_void:
        pushl   $_xdr_void
        jmp     __sigfe

        .extern _xdr_wrapstring
        .global __sigfe_xdr_wrapstring
__sigfe_xdr_wrapstring:
        pushl   $_xdr_wrapstring
        jmp     __sigfe

        .extern _xdrmem_create
        .global __sigfe_xdrmem_create
__sigfe_xdrmem_create:
        pushl   $_xdrmem_create
        jmp     __sigfe

        .extern _xdrrec_create
        .global __sigfe_xdrrec_create
__sigfe_xdrrec_create:
        pushl   $_xdrrec_create
        jmp     __sigfe

        .extern _xdrrec_endofrecord
        .global __sigfe_xdrrec_endofrecord
__sigfe_xdrrec_endofrecord:
        pushl   $_xdrrec_endofrecord
        jmp     __sigfe

        .extern _xdrrec_eof
        .global __sigfe_xdrrec_eof
__sigfe_xdrrec_eof:
        pushl   $_xdrrec_eof
        jmp     __sigfe

        .extern _xdrrec_skiprecord
        .global __sigfe_xdrrec_skiprecord
__sigfe_xdrrec_skiprecord:
        pushl   $_xdrrec_skiprecord
        jmp     __sigfe

        .extern _xdrstdio_create
        .global __sigfe_xdrstdio_create
__sigfe_xdrstdio_create:
        pushl   $_xdrstdio_create
        jmp     __sigfe

