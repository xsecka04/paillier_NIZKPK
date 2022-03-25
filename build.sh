gcc manager.c -o manager2 paillier.c -lgmp;
gcc sender.c -o sender2 paillier.c -lgmp;
gcc test.c -o test join.c paillier.c -lgmp