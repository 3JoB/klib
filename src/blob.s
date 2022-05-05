    .global cacert
    .global cacert_size
    .global magic
    .global magic_size
    .section .rodata
cacert:
    .incbin "cacert.pem"
cacert_end:
cacert_size:
    .int cacert_end - cacert

magic:
    .incbin "/usr/local/share/misc/magic.mgc"
magic_end:
magic_size:
    .int magic_end - magic
