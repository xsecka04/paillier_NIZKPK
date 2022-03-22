gcc join.c -o join nizkpk_join.c -lgmp
gcc tests.c -o tests nizkpk_join.c -lgmp
gcc tests_rpi.c -o tests_rpi nizkpk_join.c -lgmp