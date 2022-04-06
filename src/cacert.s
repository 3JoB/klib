    .global cacert
    .global cacert_size
    .section .rodata
cacert:
    .incbin "cacert.pem"
cacert_end:
cacert_size:
    .int cacert_end - cacert
