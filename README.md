*********mini project**************<br>
*********Akshay Abhyankar***********<br>
*******111503001**************<br>
# encryption<br>
blowfish encryption<br>
usage:<br>
enc : ./project enc key [source file1] [output_file1] [source file2] [output file2].........[OPTIONS]<br>
dec : ./project dec key [source file1] [output_file1] [source file2] [output file2].........[OPTIONS]<br>
Options :<br>
-c : Creates output files<br>
Usage : ./project enc/dec key [source file1] [source file2].....-c<br>
-d : Deletes source files<br>
Usage : ./project enc/dec key [source file1] [output file1] [source file2] [output file2].....-d<br>
-cd : Creates output files and deletes source files<br>
Usage : ./project enc/dec key [source file1] [source file2].....-cd<br>
Checks if the password is strong and recommends user to have a strong password(4 - 56bytes).<br>

Here I have used 2 unsigned long variables to read "8" bytes from a file as blowfish requires 16 bytes to operate on at a time.
I have functions Blowfish_Encrypt and Blowfish_Decrypt who accept 16 bytes in the form of 2 "8" bytes and encrypt or decrypt them respectively.
While writing to an output file I made a character pointer point to these unsigned long variables and then wrote byte by byte.
I have made sure that the password is strong enough.
