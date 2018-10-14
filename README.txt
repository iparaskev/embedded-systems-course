Complile

Με την εντολή make γίνεται complile ο κώδικας και στον φάκελο bin δημιουργούνται 
τα εκτελέσιμα. 
Για να γίνει compile για την αρχιτεκτονική του zsun πρέπει να συμπληρωθεί 
το path του compiler στην μεταβλητή cross_gcc στο makefile. 

---------------------------------------------------

Εκτέλεση

Server: ./bin/server PORT
Client: ./bin/client SERVER_IP PORT
Server local: ./bin/server_loc PORT

---------------------------------------------------

Έλεγχος(φάκελος tests)

* Για την εκτέλεση των παρακάτω scripts ο server πρέπει να τρέξει με το flag -t *

Τα scripts ελέγχου είναι τα εξής:

- limits.sh: ./limits.sh SERVER_IP PORT ID ITERATIONS MSG SL1 SL2 MSGS
Το οποίο καλεί ITERATIONS*MSG φορές τον client και στέλνει MSGS μηνύματα στον 
server. Τα sl1 sl2 είναι ο χρόνος που κάνει sleep μεταξύ κάθε MSG και ITERATION
αντίστοιχα. 
- complete_tests.sh/complete_tests_loc.sh: Τα ορίσματα τους είναι ίδια με το 
limits.sh με την διαφορά ότι αντί για ID δέχεται τον αριθμό των ταυτόχρονων clients
. Η λειτουργία του είναι να καλεί το limits και στην συνέχεια να ελέγχει
αν όλα τα μηνύματα που στάλθηκαν ληφθήθηκαν κιόλας. Επίσης κρατάει στατιστικά 
της χρήσης της cpu. Το _loc είναι για server που τρέχει local ενώ το άλλο 
είναι για server που εκτελείται στο zsun. 
- validate_results.py: Ελέγχει την σωστή αποστολή και παραλαβή των μηνυμάτων.

Οπότε είναι:

limits: ./tests/limits.sh IP PORT ID ITERATIONS MSG SL1 SL2 MSGS
complete: ./tests/complete_tests.sh IP PORT CLIENT ITERATIONS MSG SL1 SL2 MSGS

