# Password Manager written in C
This is a shoddy command-line program that manages a "Deck" of "Cards", where each card is an entry containing 
a service name, service website, username, and password. The deck may be saved/loaded to/from a file, and also
has the ability to "lock". 

## Progress
UPDATE: Previously, the locking mechanism was a simple XOR on the password and the key,
but now performs AES on each password, with the master key being the AES key. Security is YOUR due dilligence now. 
It is also important to note that when loading a deck from file, locking it, and then saving the locked deck to file, 
it is possible that unlocking the locked file will produce passwords that do not match what they were prior to encryption.
Check your encoding. 

## Memory
I am not well-versed in securing the memory of programs, but after many many many runs using valgrind, it appears that all
allocated memory is properly freed when using the program as intended. However, I have made no attempts to attack the program 
and reach data that should not be reachable. Use at your own risk!

## Usage
### pwgen
`make pwgen` will compile the simple command-line password generator. Running the program with no arguments will
execute it in interactive mode. Alternatively, 
1. `./pwgen -n <LENGTH>` will output a simple password of provided length. 
2. `./pwgen -n <LENGTH> [-d or -s]` will output either a dashed (-d) or simple (-s) password of provided length.
3. `./pwgen -n <LENGTH> [-d or -s] -h` will do the same as above with the addition of the generated password's SHA3-256 hash.
### test
`make test` will compile the password manager with the testing function loadTest, which inserts 16 example cards into the deck.
### pwm
`make` will check for existing binary and remove it if it exists, and compile the password manager program which may be executed 
with `./pwm`. `make build` will simply build the binary without checking for an already existing one. `make clean` will check for
and remove the binary if it exists.

## Example
Clone the repo and run `make` which will produce the `pwm` binary. Run the program and come up with a master key, make sure you keep track
of it because the program does not save it across reruns. Select option 1. Enter the service nickname, the service website, your username 
for the service, and your password for the service. This will create a new Card in your Deck. Select option 3 to view the current deck.
Try option 4, enter the master key, and then option 3 again. Notice how the passwords are encrypted? This is the state you should save the
deck in, so run option 7 and enter the filename you wish to save the deck into. You may exit now. To get the deck decrypted, simply rerun the 
program, load the deck with option 8, unlock the deck with option 5, and option 3 will display the plaintext.
