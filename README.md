## Password Manager written in C
This is a shoddy command-line program that manages a "Deck" of "Cards", where each card is an entry containing 
a service name, service website, username, and password. The deck may be saved/loaded to/from a file, and also
has the ability to "lock" the file. WARNING: this is not secure. The locking mechanism simply takes the master
key and performs XOR on each character of each password with the master key. It is also important to note that
when loading a deck from file, locking it, and then saving the locked deck to file, it is almost guaranteed that
unlocking the locked file will produce incorrect passwords that do not match what they were prior to encryption.
This is due to the character representations being altered when reading/writing to file. There are currently no
known issues when reading/writing a plaintext deck file since all characters are within a-zA-Z0-9!@#$_-

## Usage
`make pwgen` will compile the simple command-line password generator. Running the program with no arguments will
execute it in interactive mode. Alternatively, 
1. `./pwgen -n <LENGTH>` will output a simple password of provided length. 
2. `./pwgen -n <LENGTH> [-d or -s]` will output either a dashed (-d) or simple (-s) password of provided length.
3. `./pwgen -n <LENGTH> [-d or -s] -h` will do the same as above with the addition of the generated password's SHA3-256 hash.
