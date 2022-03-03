gcc manager.c -o manager2 paillier.c -lgmp;
gcc sender.c -o sender2 paillier.c -lgmp;
gcc random.c -o test nizkpk_join.c -lgmp